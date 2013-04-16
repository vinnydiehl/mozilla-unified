/* Any copyright is dedicated to the Public Domain.
   http://creativecommons.org/publicdomain/zero/1.0/ */

"use strict";
const Cc = Components.classes;
const Ci = Components.interfaces;
const Cu = Components.utils;
const Cr = Components.results;

Cu.import("resource://gre/modules/Services.jsm");

// Always log packets when running tests. runxpcshelltests.py will throw
// the output away anyway, unless you give it the --verbose flag.
Services.prefs.setBoolPref("devtools.debugger.log", true);
// Enable remote debugging for the relevant tests.
Services.prefs.setBoolPref("devtools.debugger.remote-enabled", true);

Cu.import("resource://gre/modules/devtools/dbg-server.jsm");
Cu.import("resource://gre/modules/devtools/dbg-client.jsm");

// Convert an nsIScriptError 'aFlags' value into an appropriate string.
function scriptErrorFlagsToKind(aFlags) {
  var kind;
  if (aFlags & Ci.nsIScriptError.warningFlag)
    kind = "warning";
  if (aFlags & Ci.nsIScriptError.exceptionFlag)
    kind = "exception";
  else
    kind = "error";

  if (aFlags & Ci.nsIScriptError.strictFlag)
    kind = "strict " + kind;

  return kind;
}

// Register a console listener, so console messages don't just disappear
// into the ether.
let errorCount = 0;
let listener = {
  observe: function (aMessage) {
    errorCount++;
    try {
      // If we've been given an nsIScriptError, then we can print out
      // something nicely formatted, for tools like Emacs to pick up.
      var scriptError = aMessage.QueryInterface(Ci.nsIScriptError);
      dump(aMessage.sourceName + ":" + aMessage.lineNumber + ": " +
           scriptErrorFlagsToKind(aMessage.flags) + ": " +
           aMessage.errorMessage + "\n");
      var string = aMessage.errorMessage;
    } catch (x) {
      // Be a little paranoid with message, as the whole goal here is to lose
      // no information.
      try {
        var string = "" + aMessage.message;
      } catch (x) {
        var string = "<error converting error message to string>";
      }
    }

    // Make sure we exit all nested event loops so that the test can finish.
    while (DebuggerServer.xpcInspector.eventLoopNestLevel > 0) {
      DebuggerServer.xpcInspector.exitNestedEventLoop();
    }
    do_throw("head_dbg.js got console message: " + string + "\n");
  }
};

let consoleService = Cc["@mozilla.org/consoleservice;1"].getService(Ci.nsIConsoleService);
consoleService.registerListener(listener);

function check_except(func)
{
  try {
    func();
  } catch (e) {
    do_check_true(true);
    return;
  }
  dump("Should have thrown an exception: " + func.toString());
  do_check_true(false);
}

function testGlobal(aName) {
  let systemPrincipal = Cc["@mozilla.org/systemprincipal;1"]
    .createInstance(Ci.nsIPrincipal);

  let sandbox = Cu.Sandbox(systemPrincipal);
  Cu.evalInSandbox("this.__name = '" + aName + "'", sandbox);
  return sandbox;
}

function addTestGlobal(aName)
{
  let global = testGlobal(aName);
  DebuggerServer.addTestGlobal(global);
  return global;
}

function getTestGlobalContext(aClient, aName, aCallback) {
  aClient.request({ "to": "root", "type": "listContexts" }, function(aResponse) {
    for each (let context in aResponse.contexts) {
      if (context.global == aName) {
        aCallback(context);
        return false;
      }
    }
    aCallback(null);
  });
}

function attachTestGlobalClient(aClient, aName, aCallback) {
  getTestGlobalContext(aClient, aName, function(aContext) {
    aClient.attachThread(aContext.actor, aCallback, { useSourceMaps: true });
  });
}

function attachTestGlobalClientAndResume(aClient, aName, aCallback) {
  attachTestGlobalClient(aClient, aName, function(aResponse, aThreadClient) {
    aThreadClient.resume(function(aResponse) {
      aCallback(aResponse, aThreadClient);
    });
  })
}

function getTestTab(aClient, aName, aCallback) {
  gClient.listTabs(function (aResponse) {
    for (let tab of aResponse.tabs) {
      if (tab.title === aName) {
        aCallback(tab);
        return;
      }
    }
    aCallback(null);
  });
}

function attachTestTab(aClient, aName, aCallback) {
  getTestTab(aClient, aName, function (aTab) {
    gClient.attachTab(aTab.actor, aCallback);
  });
}

function attachTestTabAndResume(aClient, aName, aCallback) {
  attachTestTab(aClient, aName, function (aResponse, aTabClient) {
    aClient.attachThread(aResponse.threadActor, function (aResponse, aThreadClient) {
      aThreadClient.resume(function (aResponse) {
        aCallback(aResponse, aTabClient, aThreadClient);
      });
    }, { useSourceMaps: true });
  });
}

/**
 * Initialize the testing debugger server.
 */
function initTestDebuggerServer()
{
  DebuggerServer.addActors("resource://test/testactors.js");
  // Allow incoming connections.
  DebuggerServer.init(function () { return true; });
}

function initSourcesBackwardsCompatDebuggerServer()
{
  DebuggerServer.addActors("chrome://global/content/devtools/dbg-browser-actors.js");
  DebuggerServer.addActors("resource://test/testcompatactors.js");
  DebuggerServer.init(function () { return true; });
}

function finishClient(aClient)
{
  aClient.close(function() {
    do_test_finished();
  });
}

/**
 * Takes a relative file path and returns the absolute file url for it.
 */
function getFileUrl(aName) {
  let file = do_get_file(aName);
  return Services.io.newFileURI(file).spec;
}

/**
 * Returns the full path of the file with the specified name in a
 * platform-independent and URL-like form.
 */
function getFilePath(aName)
{
  let file = do_get_file(aName);
  let path = Services.io.newFileURI(file).spec;
  let filePrePath = "file://";
  if ("nsILocalFileWin" in Ci &&
      file instanceof Ci.nsILocalFileWin) {
    filePrePath += "/";
  }
  return path.slice(filePrePath.length);
}

Cu.import("resource://gre/modules/NetUtil.jsm");

/**
 * Returns the full text contents of the given file.
 */
function readFile(aFileName) {
  let f = do_get_file(aFileName);
  let s = Cc["@mozilla.org/network/file-input-stream;1"]
    .createInstance(Ci.nsIFileInputStream);
  s.init(f, -1, -1, false);
  try {
    return NetUtil.readInputStreamToString(s, s.available());
  } finally {
    s.close();
  }
}
