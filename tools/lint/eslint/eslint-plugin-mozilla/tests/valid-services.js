/* Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/ */

"use strict";

// ------------------------------------------------------------------------------
// Requirements
// ------------------------------------------------------------------------------

var rule = require("../lib/rules/valid-services");
var RuleTester = require("eslint").RuleTester;

const ruleTester = new RuleTester({ parserOptions: { ecmaVersion: "latest" } });

// ------------------------------------------------------------------------------
// Tests
// ------------------------------------------------------------------------------

function invalidCode(code, alias) {
  return { code, errors: [{ messageId: "unknownProperty", data: { alias } }] };
}

ruleTester.run("valid-services", rule, {
  valid: ["Services.crashmanager", "lazy.Services.crashmanager"],
  invalid: [
    invalidCode("Services.foo", "foo"),
    invalidCode("Services.foo()", "foo"),
    invalidCode("lazy.Services.foo", "foo"),
    invalidCode("Services.foo.bar()", "foo"),
    invalidCode("lazy.Services.foo.bar()", "foo"),
  ],
});
