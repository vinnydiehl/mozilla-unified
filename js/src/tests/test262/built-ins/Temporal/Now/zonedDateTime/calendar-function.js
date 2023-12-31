// |reftest| skip-if(!this.hasOwnProperty('Temporal')) -- Temporal is not enabled unconditionally
// Copyright (C) 2020 Igalia, S.L. All rights reserved.
// This code is governed by the BSD license found in the LICENSE file.
/*---
esid: sec-temporal.now.zoneddatetime
description: Behavior when provided calendar value is a function
includes: [compareArray.js, temporalHelpers.js]
features: [BigInt, Proxy, Temporal]
---*/
const actual = [];
const expected = [
  "has timeZone.getOffsetNanosecondsFor",
  "has timeZone.getPossibleInstantsFor",
  "has timeZone.id",
];

const calendar = function() {};
calendar.dateAdd = () => {};
calendar.dateFromFields = () => {};
calendar.dateUntil = () => {};
calendar.day = () => {};
calendar.dayOfWeek = () => {};
calendar.dayOfYear = () => {};
calendar.daysInMonth = () => {};
calendar.daysInWeek = () => {};
calendar.daysInYear = () => {};
calendar.fields = () => {};
calendar.id = "test-calendar";
calendar.inLeapYear = () => {};
calendar.mergeFields = () => {};
calendar.month = () => {};
calendar.monthCode = () => {};
calendar.monthDayFromFields = () => {};
calendar.monthsInYear = () => {};
calendar.weekOfYear = () => {};
calendar.year = () => {};
calendar.yearMonthFromFields = () => {};
calendar.yearOfWeek = () => {};

const timeZone = TemporalHelpers.timeZoneObserver(actual, "timeZone", {
  getOffsetNanosecondsFor(instant) {
    return -Number(instant.epochNanoseconds % 86400000000000n);
  }
});

Object.defineProperty(Temporal.Calendar, 'from', {
  get() {
    actual.push('get Temporal.Calendar.from');
    return undefined;
  }
});

const result = Temporal.Now.zonedDateTime(calendar, timeZone);

assert.compareArray(actual, expected, 'order of observable operations');

for (const property of ['hour', 'minute', 'second', 'millisecond', 'microsecond', 'nanosecond']) {
  assert.sameValue(result[property], 0, 'The value of result[property] is expected to be 0');
}

reportCompare(0, 0);
