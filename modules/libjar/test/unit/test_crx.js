/* -*- indent-tabs-mode: nil; js-indent-level: 2 -*- */
/* vim:set ts=2 sw=2 sts=2 et: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

function wrapInputStream(input) {
  let nsIScriptableInputStream = Ci.nsIScriptableInputStream;
  let factory = Cc["@mozilla.org/scriptableinputstream;1"];
  let wrapper = factory.createInstance(nsIScriptableInputStream);
  wrapper.init(input);
  return wrapper;
}

function extract_crx(filepath) {
  let file = do_get_file(filepath);

  let zipreader = Cc["@mozilla.org/libjar/zip-reader;1"].createInstance(
    Ci.nsIZipReader
  );
  zipreader.open(file);
  // do crc stuff
  function check_archive_crc() {
    zipreader.test(null);
    return true;
  }
  Assert.ok(check_archive_crc());
  zipreader.findEntries(null);
  let stream = wrapInputStream(
    zipreader.getInputStream("modules/libjar/test/Makefile.in")
  );
  let dirstream = wrapInputStream(
    zipreader.getInputStream("modules/libjar/test/")
  );
  zipreader.close();
  zipreader = null;
  Cu.forceGC();
  Assert.ok(!!stream.read(1024).length);
  Assert.ok(!!dirstream.read(100).length);
}

// Make sure that we can read from CRX files as if they were ZIP files.
function run_test() {
  // Note: test_crx_dummy.crx is a dummy crx file created for this test. The
  // public key and signature fields in the header are both empty.
  extract_crx("data/test_crx_dummy.crx");

  // Note: test_crx_v3_dummy.crx is a dummy crx file created for this test. The
  // header length field is set to 4 bytes.
  extract_crx("data/test_crx_v3_dummy.crx");
}
