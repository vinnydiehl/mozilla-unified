// |reftest| skip-if(!this.hasOwnProperty('Temporal')) -- Temporal is not enabled unconditionally
// Copyright (C) 2021 Igalia, S.L. All rights reserved.
// This code is governed by the BSD license found in the LICENSE file.

/*---
esid: sec-temporal.zoneddatetime.prototype.withcalendar
description: Objects of a subclass are never created as return values.
includes: [temporalHelpers.js]
features: [Temporal]
---*/

const customCalendar = {
  year() { return 1900; },
  month() { return 2; },
  day() { return 5; },
  toString() { return "custom-calendar"; },
  dateAdd() {},
  dateFromFields() {},
  dateUntil() {},
  dayOfWeek() {},
  dayOfYear() {},
  daysInMonth() {},
  daysInWeek() {},
  daysInYear() {},
  fields() {},
  id: "custom-calendar",
  inLeapYear() {},
  mergeFields() {},
  monthCode() {},
  monthDayFromFields() {},
  monthsInYear() {},
  weekOfYear() {},
  yearMonthFromFields() {},
  yearOfWeek() {},
};

TemporalHelpers.checkSubclassingIgnored(
  Temporal.ZonedDateTime,
  [10n, "UTC"],
  "withCalendar",
  [customCalendar],
  (result) => {
    assert.sameValue(result.epochNanoseconds, 10n, "epochNanoseconds result");
    assert.sameValue(result.year, 1900, "year result");
    assert.sameValue(result.month, 2, "month result");
    assert.sameValue(result.day, 5, "day result");
    assert.sameValue(result.hour, 0, "hour result");
    assert.sameValue(result.minute, 0, "minute result");
    assert.sameValue(result.second, 0, "second result");
    assert.sameValue(result.millisecond, 0, "millisecond result");
    assert.sameValue(result.microsecond, 0, "microsecond result");
    assert.sameValue(result.nanosecond, 10, "nanosecond result");
    assert.sameValue(result.getCalendar(), customCalendar, "calendar result");
  },
);

reportCompare(0, 0);
