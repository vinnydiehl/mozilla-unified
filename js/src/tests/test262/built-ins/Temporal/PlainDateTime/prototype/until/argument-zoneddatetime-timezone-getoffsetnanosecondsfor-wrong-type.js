// |reftest| skip-if(!this.hasOwnProperty('Temporal')) -- Temporal is not enabled unconditionally
// Copyright (C) 2021 Igalia, S.L. All rights reserved.
// This code is governed by the BSD license found in the LICENSE file.

/*---
esid: sec-temporal.plaindatetime.prototype.until
description: TypeError thrown if time zone reports an offset that is not a Number
features: [Temporal]
includes: [temporalHelpers.js]
---*/

[
  undefined,
  null,
  true,
  "+01:00",
  Symbol(),
  3600_000_000_000n,
  {},
  { valueOf() { return 3600_000_000_000; } },
].forEach((wrongOffset) => {
  const timeZone = TemporalHelpers.specificOffsetTimeZone(wrongOffset);
  const plain = new Temporal.PlainDateTime(2000, 5, 2, 12, 34, 56, 987, 654, 321);
  const zoned = new Temporal.ZonedDateTime(1_000_000_000_987_654_321n, timeZone);
  assert.throws(TypeError, () => plain.until(zoned));
});

reportCompare(0, 0);
