// |reftest| skip-if(!this.hasOwnProperty('Temporal')) -- Temporal is not enabled unconditionally
// Copyright (C) 2023 Igalia, S.L. All rights reserved.
// This code is governed by the BSD license found in the LICENSE file.
/*---
esid: sec-temporal.plaindatetime.prototype.compare
description: If a calendar's fields() method returns a field named 'constructor', PrepareTemporalFields should throw a RangeError.
includes: [temporalHelpers.js]
features: [Temporal]
---*/

const calendar = TemporalHelpers.calendarWithExtraFields(['constructor']);
const arg = {year: 2023, month: 5, monthCode: 'M05', day: 1, calendar: calendar};

assert.throws(RangeError, () => Temporal.PlainDateTime.compare(arg, new Temporal.PlainDateTime(1976, 11, 18)));
assert.throws(RangeError, () => Temporal.PlainDateTime.compare(new Temporal.PlainDateTime(1976, 11, 18), arg));

reportCompare(0, 0);
