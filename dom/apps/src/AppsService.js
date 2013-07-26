/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

"use strict"

function debug(s) {
  //dump("-*- AppsService.js: " + s + "\n");
}

const Cc = Components.classes;
const Ci = Components.interfaces;
const Cu = Components.utils;

Cu.import("resource://gre/modules/XPCOMUtils.jsm");
Cu.import("resource://gre/modules/Services.jsm");

const APPS_SERVICE_CID = Components.ID("{05072afa-92fe-45bf-ae22-39b69c117058}");

function AppsService()
{
  debug("AppsService Constructor");
  let inParent = Cc["@mozilla.org/xre/app-info;1"].getService(Ci.nsIXULRuntime)
                   .processType == Ci.nsIXULRuntime.PROCESS_TYPE_DEFAULT;
  debug("inParent: " + inParent);
  Cu.import(inParent ? "resource://gre/modules/Webapps.jsm" :
                       "resource://gre/modules/AppsServiceChild.jsm");
}

AppsService.prototype = {

  getCSPByLocalId: function getCSPByLocalId(localId) {
    debug("GetCSPByLocalId( " + localId + " )");
    return DOMApplicationRegistry.getCSPByLocalId(localId);
  },

  getAppByManifestURL: function getAppByManifestURL(aManifestURL) {
    debug("GetAppByManifestURL( " + aManifestURL + " )");
    return DOMApplicationRegistry.getAppByManifestURL(aManifestURL);
  },

  getAppLocalIdByManifestURL: function getAppLocalIdByManifestURL(aManifestURL) {
    debug("getAppLocalIdByManifestURL( " + aManifestURL + " )");
    return DOMApplicationRegistry.getAppLocalIdByManifestURL(aManifestURL);
  },

  getAppLocalIdByStoreId: function getAppLocalIdByStoreId(aStoreId) {
    debug("getAppLocalIdByStoreId( " + aStoreId + " )");
    return DOMApplicationRegistry.getAppLocalIdByStoreId(aStoreId);
  },

  getAppByLocalId: function getAppByLocalId(aLocalId) {
    debug("getAppByLocalId( " + aLocalId + " )");
    return DOMApplicationRegistry.getAppByLocalId(aLocalId);
  },

  getManifestURLByLocalId: function getManifestURLByLocalId(aLocalId) {
    debug("getManifestURLByLocalId( " + aLocalId + " )");
    return DOMApplicationRegistry.getManifestURLByLocalId(aLocalId);
  },

  getCoreAppsBasePath: function getCoreAppsBasePath() {
    debug("getCoreAppsBasePath()");
    return DOMApplicationRegistry.getCoreAppsBasePath();
  },

  getWebAppsBasePath: function getWebAppsBasePath() {
    debug("getWebAppsBasePath()");
    return DOMApplicationRegistry.getWebAppsBasePath();
  },

  getAppInfo: function getAppInfo(aAppId) {
    debug("getAppInfo()");
    return DOMApplicationRegistry.getAppInfo(aAppId);
  },

  getRedirect: function getRedirect(aLocalId, aURI) {
    debug("getRedirect for " + aLocalId + " " + aURI.spec);
    if (aLocalId == Ci.nsIScriptSecurityManager.NO_APP_ID ||
        aLocalId == Ci.nsIScriptSecurityManager.UNKNOWN_APP_ID) {
      return null;
    }

    let app = DOMApplicationRegistry.getAppByLocalId(aLocalId);
    if (app && app.redirects) {
      let spec = aURI.spec;
      for (let i = 0; i < app.redirects.length; i++) {
        let redirect = app.redirects[i];
        if (spec.startsWith(redirect.from)) {
          // Prepend the app origin to the redirection. We need that since
          // the origin of packaged apps is a uuid created at install time.
          let to = app.origin + redirect.to;
          // If we have a ? or a # in the current URL, add this part to the
          // redirection.
          let index = -1;
          index = spec.indexOf('?');
          if (index == -1) {
            index = spec.indexOf('#');
          }

          if (index != -1) {
            to += spec.substring(index);
          }
          debug('App specific redirection from ' + spec + ' to ' + to);
          return Services.io.newURI(to, null, null);
        }
      }
    }
    // No matching redirect.
    return null;
  },

  classID : APPS_SERVICE_CID,
  QueryInterface : XPCOMUtils.generateQI([Ci.nsIAppsService])
}

this.NSGetFactory = XPCOMUtils.generateNSGetFactory([AppsService])
