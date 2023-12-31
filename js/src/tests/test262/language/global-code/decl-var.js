// Copyright (C) 2016 the V8 project authors. All rights reserved.
// This code is governed by the BSD license found in the LICENSE file.
/*---
esid: sec-globaldeclarationinstantiation
es6id: 15.1.8
description: Declaration of variable where permissible
info: |
  [...]
  11. Let declaredVarNames be a new empty List.
  12. For each d in varDeclarations, do
      a. If d is a VariableDeclaration or a ForBinding, then
         i. For each String vn in the BoundNames of d, do
            1. If vn is not an element of declaredFunctionNames, then
               a. Let vnDefinable be ? envRec.CanDeclareGlobalVar(vn).
               b. If vnDefinable is false, throw a TypeError exception.
               c. If vn is not an element of declaredVarNames, then
                  i. Append vn to declaredVarNames.
  [...]
  18. For each String vn in declaredVarNames, in list order do
      a. Perform ? envRec.CreateGlobalVarBinding(vn, false).
  [...]

  8.1.1.4.15 CanDeclareGlobalVar

  1. Let envRec be the global Environment Record for which the method was
     invoked.
  2. Let ObjRec be envRec.[[ObjectRecord]].
  3. Let globalObject be the binding object for ObjRec.
  4. Let hasProperty be ? HasOwnProperty(globalObject, N).
  5. If hasProperty is true, return true.
  6. Return ? IsExtensible(globalObject).
includes: [propertyHelper.js]
---*/

verifyProperty(this, "brandNew", {
  value: undefined,
  writable: true,
  enumerable: true,
  configurable: false,
});

var brandNew;

reportCompare(0, 0);
