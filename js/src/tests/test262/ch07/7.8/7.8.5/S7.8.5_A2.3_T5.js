// Copyright 2009 the Sputnik authors.  All rights reserved.
// This code is governed by the BSD license found in the LICENSE file.

/**
 * RegularExpressionChar :: LineTerminator is incorrect
 *
 * @path ch07/7.8/7.8.5/S7.8.5_A2.3_T5.js
 * @description Line separator, with eval
 */

//CHECK#1
try {
   eval("/a\u2028/").source;
   $ERROR('#1.1: RegularExpressionChar :: Line separator is incorrect. Actual: ' + (eval("/a\u2028/").source)); 
}
catch (e) {
  if ((e instanceof SyntaxError) !== true) {
     $ERROR('#1.2: RegularExpressionChar :: Line separator is incorrect. Actual: ' + (e));
  }
}     

