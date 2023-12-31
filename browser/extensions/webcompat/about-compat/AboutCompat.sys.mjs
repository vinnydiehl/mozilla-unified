/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

const addonID = "webcompat@mozilla.org";
const addonPageRelativeURL = "/about-compat/aboutCompat.html";

export function AboutCompat() {
  this.chromeURL =
    WebExtensionPolicy.getByID(addonID).getURL(addonPageRelativeURL);
}

AboutCompat.prototype = {
  QueryInterface: ChromeUtils.generateQI(["nsIAboutModule"]),
  getURIFlags() {
    return (
      Ci.nsIAboutModule.URI_MUST_LOAD_IN_EXTENSION_PROCESS |
      Ci.nsIAboutModule.IS_SECURE_CHROME_UI
    );
  },

  newChannel(aURI, aLoadInfo) {
    const uri = Services.io.newURI(this.chromeURL);
    const channel = Services.io.newChannelFromURIWithLoadInfo(uri, aLoadInfo);
    channel.originalURI = aURI;

    channel.owner = (
      Services.scriptSecurityManager.createContentPrincipal ||
      // Handles fallback to earlier versions.
      // eslint-disable-next-line mozilla/valid-services-property
      Services.scriptSecurityManager.createCodebasePrincipal
    )(uri, aLoadInfo.originAttributes);
    return channel;
  },
};
