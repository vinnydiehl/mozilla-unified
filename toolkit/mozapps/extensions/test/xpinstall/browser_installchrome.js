// ----------------------------------------------------------------------------
// Tests that calling InstallTrigger.installChrome works
function test() {
  // This test depends on InstallTrigger.installChrome availability.
  setInstallTriggerPrefs();

  Harness.installEndedCallback = install_ended;
  Harness.installsCompletedCallback = finish_test;
  Harness.setup();

  PermissionTestUtils.add(
    "http://example.com/",
    "install",
    Services.perms.ALLOW_ACTION
  );

  gBrowser.selectedTab = BrowserTestUtils.addTab(gBrowser);
  BrowserTestUtils.startLoadingURIString(
    gBrowser,
    TESTROOT +
      "installchrome.html? " +
      encodeURIComponent(TESTROOT + "amosigned.xpi")
  );
}

function install_ended(install, addon) {
  return addon.uninstall();
}

function finish_test(count) {
  is(count, 1, "1 Add-on should have been successfully installed");
  PermissionTestUtils.remove("http://example.com", "install");

  gBrowser.removeCurrentTab();
  Harness.finish();
}
