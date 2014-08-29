/* -*- indent-tabs-mode: nil; js-indent-level: 2 -*- */
/* vim: set ft=javascript ts=2 et sw=2 tw=80: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
"use strict";

const { classes: Cc, interfaces: Ci, utils: Cu, results: Cr } = Components;

const DBG_XUL = "chrome://browser/content/devtools/framework/toolbox-process-window.xul";
const CHROME_DEBUGGER_PROFILE_NAME = "chrome_debugger_profile";

Cu.import("resource://gre/modules/Services.jsm");
Cu.import("resource://gre/modules/XPCOMUtils.jsm")

XPCOMUtils.defineLazyModuleGetter(this, "DevToolsLoader",
  "resource://gre/modules/devtools/Loader.jsm");
XPCOMUtils.defineLazyModuleGetter(this, "devtools",
  "resource://gre/modules/devtools/Loader.jsm");

XPCOMUtils.defineLazyGetter(this, "Telemetry", function () {
  return devtools.require("devtools/shared/telemetry");
});
XPCOMUtils.defineLazyGetter(this, "EventEmitter", function () {
  return devtools.require("devtools/toolkit/event-emitter");
});
const { Promise: promise } = Cu.import("resource://gre/modules/Promise.jsm", {});

this.EXPORTED_SYMBOLS = ["BrowserToolboxProcess"];

let processes = Set();

/**
 * Constructor for creating a process that will hold a chrome toolbox.
 *
 * @param function aOnClose [optional]
 *        A function called when the process stops running.
 * @param function aOnRun [optional]
 *        A function called when the process starts running.
 * @param object aOptions [optional]
 *        An object with properties for configuring BrowserToolboxProcess.
 */
this.BrowserToolboxProcess = function BrowserToolboxProcess(aOnClose, aOnRun, aOptions) {
  let emitter = new EventEmitter();
  this.on = emitter.on.bind(emitter);
  this.off = emitter.off.bind(emitter);
  this.once = emitter.once.bind(emitter);
  // Forward any events to the shared emitter.
  this.emit = function(...args) {
    emitter.emit(...args);
    BrowserToolboxProcess.emit(...args);
  }

  // If first argument is an object, use those properties instead of
  // all three arguments
  if (typeof aOnClose === "object") {
    if (aOnClose.onClose) {
      this.on("close", aOnClose.onClose);
    }
    if (aOnClose.onRun) {
      this.on("run", aOnClose.onRun);
    }
    this._options = aOnClose;
  } else {
    if (aOnClose) {
      this.on("close", aOnClose);
    }
    if (aOnRun) {
      this.on("run", aOnRun);
    }
    this._options = aOptions || {};
  }

  this._telemetry = new Telemetry();

  this.close = this.close.bind(this);
  Services.obs.addObserver(this.close, "quit-application", false);
  this._initServer();
  this._initProfile();
  this._create();

  processes.add(this);
};

EventEmitter.decorate(BrowserToolboxProcess);

/**
 * Initializes and starts a chrome toolbox process.
 * @return object
 */
BrowserToolboxProcess.init = function(aOnClose, aOnRun, aOptions) {
  return new BrowserToolboxProcess(aOnClose, aOnRun, aOptions);
};

/**
 * Passes a set of options to the BrowserAddonActors for the given ID.
 *
 * @param aId string
 *        The ID of the add-on to pass the options to
 * @param aOptions object
 *        The options.
 * @return a promise that will be resolved when complete.
 */
BrowserToolboxProcess.setAddonOptions = function DSC_setAddonOptions(aId, aOptions) {
  let promises = [];

  for (let process of processes.values()) {
    promises.push(process.debuggerServer.setAddonOptions(aId, aOptions));
  }

  return promise.all(promises);
};

BrowserToolboxProcess.prototype = {
  /**
   * Initializes the debugger server.
   */
  _initServer: function() {
    dumpn("Initializing the chrome toolbox server.");

    if (!this.loader) {
      // Create a separate loader instance, so that we can be sure to receive a
      // separate instance of the DebuggingServer from the rest of the devtools.
      // This allows us to safely use the tools against even the actors and
      // DebuggingServer itself, especially since we can mark this loader as
      // invisible to the debugger (unlike the usual loader settings).
      this.loader = new DevToolsLoader();
      this.loader.invisibleToDebugger = true;
      this.loader.main("devtools/server/main");
      this.debuggerServer = this.loader.DebuggerServer;
      dumpn("Created a separate loader instance for the DebuggerServer.");

      // Forward interesting events.
      this.debuggerServer.on("connectionchange", this.emit.bind(this));
    }

    if (!this.debuggerServer.initialized) {
      this.debuggerServer.init();
      this.debuggerServer.addBrowserActors();
      dumpn("initialized and added the browser actors for the DebuggerServer.");
    }

    let chromeDebuggingPort =
      Services.prefs.getIntPref("devtools.debugger.chrome-debugging-port");
    this.debuggerServer.openListener(chromeDebuggingPort);

    dumpn("Finished initializing the chrome toolbox server.");
    dumpn("Started listening on port: " + chromeDebuggingPort);
  },

  /**
   * Initializes a profile for the remote debugger process.
   */
  _initProfile: function() {
    dumpn("Initializing the chrome toolbox user profile.");

    let debuggingProfileDir = Services.dirsvc.get("ProfLD", Ci.nsIFile);
    debuggingProfileDir.append(CHROME_DEBUGGER_PROFILE_NAME);
    try {
      debuggingProfileDir.create(Ci.nsIFile.DIRECTORY_TYPE, 0o755);
    } catch (ex) {
      if (ex.result !== Cr.NS_ERROR_FILE_ALREADY_EXISTS) {
        dumpn("Error trying to create a profile directory, failing.");
        dumpn("Error: " + (ex.message || ex));
        return;
      }
    }

    this._dbgProfilePath = debuggingProfileDir.path;

    // We would like to copy prefs into this new profile...
    let prefsFile = debuggingProfileDir.clone();
    prefsFile.append("prefs.js");
    // ... but unfortunately, when we run tests, it seems the starting profile
    // clears out the prefs file before re-writing it, and in practice the
    // file is empty when we get here. So just copying doesn't work in that
    // case.
    // We could force a sync pref flush and then copy it... but if we're doing
    // that, we might as well just flush directly to the new profile, which
    // always works:
    Services.prefs.savePrefFile(prefsFile);

    dumpn("Finished creating the chrome toolbox user profile at: " + this._dbgProfilePath);
  },

  /**
   * Creates and initializes the profile & process for the remote debugger.
   */
  _create: function() {
    dumpn("Initializing chrome debugging process.");
    let process = this._dbgProcess = Cc["@mozilla.org/process/util;1"].createInstance(Ci.nsIProcess);
    process.init(Services.dirsvc.get("XREExeF", Ci.nsIFile));

    let xulURI = DBG_XUL;

    if (this._options.addonID) {
      xulURI += "?addonID=" + this._options.addonID;
    }

    dumpn("Running chrome debugging process.");
    let args = ["-no-remote", "-foreground", "-profile", this._dbgProfilePath, "-chrome", xulURI];

    process.runwAsync(args, args.length, { observe: () => this.close() });

    this._telemetry.toolOpened("jsbrowserdebugger");

    dumpn("Chrome toolbox is now running...");
    this.emit("run", this);
  },

  /**
   * Closes the remote debugging server and kills the toolbox process.
   */
  close: function() {
    if (this.closed) {
      return;
    }

    dumpn("Cleaning up the chrome debugging process.");
    Services.obs.removeObserver(this.close, "quit-application");

    if (this._dbgProcess.isRunning) {
      this._dbgProcess.kill();
    }

    this._telemetry.toolClosed("jsbrowserdebugger");
    if (this.debuggerServer) {
      this.debuggerServer.destroy();
    }

    dumpn("Chrome toolbox is now closed...");
    this.closed = true;
    this.emit("close", this);
    processes.delete(this);
  }
};

/**
 * Helper method for debugging.
 * @param string
 */
function dumpn(str) {
  if (wantLogging) {
    dump("DBG-FRONTEND: " + str + "\n");
  }
}

let wantLogging = Services.prefs.getBoolPref("devtools.debugger.log");

Services.prefs.addObserver("devtools.debugger.log", {
  observe: (...args) => wantLogging = Services.prefs.getBoolPref(args.pop())
}, false);

Services.obs.notifyObservers(null, "ToolboxProcessLoaded", null);
