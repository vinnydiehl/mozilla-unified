// |reftest| skip-if(!this.hasOwnProperty('Temporal')) -- Temporal is not enabled unconditionally
// Copyright (C) 2023 Igalia, S.L. All rights reserved.
// This code is governed by the BSD license found in the LICENSE file.

/*---
esid: sec-temporal.duration.prototype.subtract
description: >
  Calling the method with a relativeTo property bag with a builtin calendar
  causes no observable array iteration when getting the calendar fields.
features: [Temporal]
---*/

const arrayPrototypeSymbolIteratorOriginal = Array.prototype[Symbol.iterator];
Array.prototype[Symbol.iterator] = function arrayIterator() {
  throw new Test262Error("Array should not be iterated");
}

const timeZone = "UTC";
const instance = new Temporal.Duration(1, 0, 0, 1);
const relativeTo = { year: 2000, month: 5, day: 2, hour: 21, minute: 43, second: 5, timeZone, calendar: "iso8601" };
instance.subtract(new Temporal.Duration(0, 0, 0, 0, 24), { relativeTo });

Array.prototype[Symbol.iterator] = arrayPrototypeSymbolIteratorOriginal;

reportCompare(0, 0);
