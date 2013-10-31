/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

const Cu = Components.utils;
Cu.import("resource://gre/modules/Services.jsm");
Cu.import("resource:///modules/devtools/gDevTools.jsm");

const {devtools} = Cu.import("resource://gre/modules/devtools/Loader.jsm", {});
const {require} = devtools;

const {ConnectionManager, Connection} = require("devtools/client/connection-manager");
const {getDeviceFront} = require("devtools/server/actors/device");
const {getTargetForApp} = require("devtools/app-actor-front");
const DeviceStore = require("devtools/app-manager/device-store");
const WebappsStore = require("devtools/app-manager/webapps-store");
const promise = require("sdk/core/promise");

window.addEventListener("message", function(event) {
  try {
    let message = JSON.parse(event.data);
    if (message.name == "connection") {
      let cid = parseInt(message.cid);
      for (let c of ConnectionManager.connections) {
        if (c.uid == cid) {
          UI.connection = c;
          UI.onNewConnection();
          break;
        }
      }
    }
  } catch(e) {
    Cu.reportError(e);
  }
});

window.addEventListener("unload", function onUnload() {
  window.removeEventListener("unload", onUnload);
  UI.destroy();
});

let UI = {
  init: function() {
    this.showFooterIfNeeded();
    this.setTab("apps");
    if (this.connection) {
      this.onNewConnection();
    } else {
      this.hide();
    }
  },

  destroy: function() {
    if (this.connection) {
      this.connection.off(Connection.Events.STATUS_CHANGED, this._onConnectionStatusChange);
    }
    if (this.store) {
      this.store.destroy();
    }
    if (this.template) {
      this.template.destroy();
    }
  },

  showFooterIfNeeded: function() {
    let footer = document.querySelector("#connection-footer");
    if (window.parent == window) {
      // We're alone. Let's add a footer.
      footer.removeAttribute("hidden");
      footer.src = "chrome://browser/content/devtools/app-manager/connection-footer.xhtml";
    } else {
      footer.setAttribute("hidden", "true");
    }
  },

  hide: function() {
    document.body.classList.add("notconnected");
  },

  show: function() {
    document.body.classList.remove("notconnected");
  },

  onNewConnection: function() {
    this.connection.on(Connection.Events.STATUS_CHANGED, this._onConnectionStatusChange);

    this.store = Utils.mergeStores({
      "device": new DeviceStore(this.connection),
      "apps": new WebappsStore(this.connection),
    });

    if (this.template) {
      this.template.destroy();
    }
    this.template = new Template(document.body, this.store, Utils.l10n);

    this.template.start();
    this._onConnectionStatusChange();
  },

  setWallpaper: function(dataurl) {
    document.getElementById("meta").style.backgroundImage = "url(" + dataurl + ")";
  },

  _onConnectionStatusChange: function() {
    if (this.connection.status != Connection.Status.CONNECTED) {
      this.hide();
      this.listTabsResponse = null;
    } else {
      this.show();
      this.connection.client.listTabs(
        response => {
          this.listTabsResponse = response;
          let front = getDeviceFront(this.connection.client, this.listTabsResponse);
          front.getWallpaper().then(longstr => {
            longstr.string().then(dataURL => {
              longstr.release().then(null, Cu.reportError);
              this.setWallpaper(dataURL);
            });
          });
        }
      );
    }
  },

  setTab: function(name) {
    var tab = document.querySelector(".tab.selected");
    var panel = document.querySelector(".tabpanel.selected");

    if (tab) tab.classList.remove("selected");
    if (panel) panel.classList.remove("selected");

    var tab = document.querySelector(".tab." + name);
    var panel = document.querySelector(".tabpanel." + name);

    if (tab) tab.classList.add("selected");
    if (panel) panel.classList.add("selected");
  },

  screenshot: function() {
    if (!this.listTabsResponse)
      return;

    let front = getDeviceFront(this.connection.client, this.listTabsResponse);
    front.screenshotToBlob().then(blob => {
      let topWindow = Services.wm.getMostRecentWindow("navigator:browser");
      let gBrowser = topWindow.gBrowser;
      let url = topWindow.URL.createObjectURL(blob);
      let tab = gBrowser.selectedTab = gBrowser.addTab(url);
      tab.addEventListener("TabClose", function onTabClose() {
        tab.removeEventListener("TabClose", onTabClose, false);
        topWindow.URL.revokeObjectURL(url);
      }, false);
    }).then(null, console.error);
  },

  openToolbox: function(manifest) {
    if (!this.listTabsResponse)
      return;
    getTargetForApp(this.connection.client,
                    this.listTabsResponse.webappsActor,
                    manifest).then((target) => {
      gDevTools.showToolbox(target,
                            null,
                            devtools.Toolbox.HostType.WINDOW).then(toolbox => {
        this.connection.once(Connection.Events.DISCONNECTED, () => {
          toolbox.destroy();
        });
      });
    }, console.error);
  },

  startApp: function(manifest) {
    let deferred = promise.defer();

    if (!this.listTabsResponse) {
      deferred.reject();
    } else {
      let actor = this.listTabsResponse.webappsActor;
      let request = {
        to: actor,
        type: "launch",
        manifestURL: manifest,
      }
      this.connection.client.request(request, (res) => {
        deferred.resolve()
      });
    }
    return deferred.promise;
  },

  stopApp: function(manifest) {
    let deferred = promise.defer();

    if (!this.listTabsResponse) {
      deferred.reject();
    } else {
      let actor = this.listTabsResponse.webappsActor;
      let request = {
        to: actor,
        type: "close",
        manifestURL: manifest,
      }
      this.connection.client.request(request, (res) => {
        deferred.resolve()
      });
    }
    return deferred.promise;
  },
}

// This must be bound immediately, as it might be used via the message listener
// before UI.init() has been called.
UI._onConnectionStatusChange = UI._onConnectionStatusChange.bind(UI);
