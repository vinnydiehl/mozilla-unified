// |reftest| skip-if(!this.hasOwnProperty('Temporal')) -- Temporal is not enabled unconditionally
// Copyright (C) 2021 Igalia, S.L. All rights reserved.
// This code is governed by the BSD license found in the LICENSE file.

/*---
esid: sec-temporal.duration.prototype.subtract
description: The relativeTo option accepts a ZonedDateTime-like ISO 8601 string
features: [Temporal]
includes: [temporalHelpers.js]
---*/

[
  '2000-01-01[UTC]',
  '2000-01-01T00:00[UTC]',
  '2000-01-01T00:00+00:00[UTC]',
  '2000-01-01T00:00+00:00[UTC][u-ca=iso8601]',
].forEach((relativeTo) => {
  const duration = new Temporal.Duration(1, 0, 0, 41);
  const result = duration.subtract(new Temporal.Duration(0, 0, 0, 10), { relativeTo });
  TemporalHelpers.assertDuration(result, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0);
});

reportCompare(0, 0);
