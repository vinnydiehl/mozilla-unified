// |reftest| skip-if(!this.hasOwnProperty("Intl"))

var log;
var proxy = new Proxy({
    year: "numeric",
    hour: "numeric",
}, new Proxy({
    get(t, pk, r) {
        log.push(pk);
        return Reflect.get(t, pk, r);
    }
}, {
    get(t, pk, r) {
        assertEq(pk, "get");
        return Reflect.get(t, pk, r);
    }
}));

var constructorAccesses = [
    // InitializeDateTimeFormat
    "localeMatcher", "calendar", "numberingSystem", "hour12", "hourCycle", "timeZone",

    // Table 5: Components of date and time formats
    "weekday", "era", "year", "month", "day", "dayPeriod", "hour", "minute", "second",
    "fractionalSecondDigits", "timeZoneName",

    // InitializeDateTimeFormat
    "formatMatcher",
    "dateStyle", "timeStyle",
];

log = [];
new Intl.DateTimeFormat(undefined, proxy);

assertEqArray(log, constructorAccesses);

log = [];
new Date().toLocaleString(undefined, proxy);

assertEqArray(log, constructorAccesses);

log = [];
new Date().toLocaleDateString(undefined, proxy);

assertEqArray(log, constructorAccesses);

log = [];
new Date().toLocaleTimeString(undefined, proxy);

assertEqArray(log, constructorAccesses);

if (typeof reportCompare === "function")
    reportCompare(0, 0);
