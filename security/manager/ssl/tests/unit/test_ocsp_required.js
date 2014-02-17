// -*- Mode: javascript; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*-
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
"use strict";

// In which we connect to a domain (as faked by a server running locally)
// and start up an OCSP responder (also basically faked) that gives a
// response with a bad signature. With security.OCSP.require set to true,
// this should fail (but it also shouldn't cause assertion failures).

let gOCSPRequestCount = 0;

function run_test() {
  do_get_profile();
  Services.prefs.setBoolPref("security.OCSP.require", true);

  // We don't actually make use of stapling in this test. This is just how we
  // get a TLS connection.
  add_tls_server_setup("OCSPStaplingServer");

  let args = [["bad-signature", "localhostAndExampleCom", "unused"]];
  let ocspResponses = generateOCSPResponses(args, "tlsserver");
  let ocspResponseBadSignature = ocspResponses[0];

  let ocspResponder = new HttpServer();
  ocspResponder.registerPrefixHandler("/", function (request, response) {
    response.setStatusLine(request.httpVersion, 200, "OK");
    response.setHeader("Content-Type", "application/ocsp-response");
    response.write(ocspResponseBadSignature);
    gOCSPRequestCount++;
  });
  ocspResponder.start(8080);

  add_tests_in_mode(true);
  add_tests_in_mode(false);

  add_test(function () { ocspResponder.stop(run_next_test); });

  run_next_test();
}

function add_tests_in_mode(useInsanity)
{
  add_test(function () {
    Services.prefs.setBoolPref("security.use_insanity_verification",
                               useInsanity);
    run_next_test();
  });

  add_connection_test("ocsp-stapling-none.example.com",
                      getXPCOMStatusFromNSS(SEC_ERROR_OCSP_INVALID_SIGNING_CERT));
  // bug 964493 - using a cached OCSP response with a bad signature would cause
  // the verification library to return a failure error code without calling
  // PORT_SetError with the specific error, violating the expectations
  // of the error handling code.
  add_connection_test("ocsp-stapling-none.example.com",
                      getXPCOMStatusFromNSS(SEC_ERROR_OCSP_INVALID_SIGNING_CERT));
  add_test(function () {
    // XXX(bug 915932): special case for insanity::pkix due to the temporary
    // lack of an OCSP cache.
    do_check_eq(gOCSPRequestCount, useInsanity ? 2 : 1);
    gOCSPRequestCount = 0;
    run_next_test();
  });
}
