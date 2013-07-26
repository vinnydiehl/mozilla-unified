// |reftest| skip-if(!this.hasOwnProperty("Type"))
var BUGNUMBER = 578700;
var summary = 'BinaryData numeric types';
var actual = '';
var expect = '';

function runTests()
{
    printBugNumber(BUGNUMBER);
    printStatus(summary);

    var TestPassCount = 0;
    var TestFailCount = 0;
    var TestTodoCount = 0;

    var TODO = 1;

    function check(fun, todo) {
        var thrown = null;
        var success = false;
        try {
            success = fun();
        } catch (x) {
            thrown = x;
        }

        if (thrown)
            success = false;

        if (todo) {
            TestTodoCount++;

            if (success) {
                var ex = new Error;
                print ("=== TODO but PASSED? ===");
                print (ex.stack);
                print ("========================");
            }

            return;
        }

        if (success) {
            TestPassCount++;
        } else {
            TestFailCount++;

            var ex = new Error;
            print ("=== FAILED ===");
            print (ex.stack);
            if (thrown) {
                print ("    threw exception:");
                print (thrown);
            }
            print ("==============");
        }
    }

    function checkThrows(fun, todo) {
        var thrown = false;
        try {
            fun();
        } catch (x) {
            thrown = true;
        }

        check(function() thrown, todo);
    }

    var types = [uint8, uint16, uint32, uint64, int8, int16, int32, int64];
    var strings = ["uint8", "uint16", "uint32", "uint64", "int8", "int16", "int32", "int64"];
    for (var i = 0; i < types.length; i++) {
        var type = types[i];

        check(function() type(true) === 1);
        check(function() type(false) === 0);
        check(function() type(+Infinity) === 0);
        check(function() type(-Infinity) === 0);
        check(function() type(NaN) === 0);
        check(function() type.toString() === strings[i]);
        check(function() type(null) == 0);
        check(function() type(undefined) == 0);
        check(function() type([]) == 0);
        check(function() type({}) == 0);
        check(function() type(/abcd/) == 0);

        checkThrows(function() new type());
    }

    var floatTypes = [float32, float64];
    var floatStrings = ["float32", "float64"];
    for (var i = 0; i < floatTypes.length; i++) {
        var type = floatTypes[i];

        check(function() type(true) === 1);
        check(function() type(false) === 0);
        check(function() type(+Infinity) === Infinity);
        check(function() type(-Infinity) === -Infinity);
        check(function() Number.isNaN(type(NaN)));
        check(function() type.toString() === floatStrings[i]);
        check(function() type(null) == 0);
        check(function() Number.isNaN(type(undefined)));
        check(function() Number.isNaN(type([])));
        check(function() Number.isNaN(type({})));
        check(function() Number.isNaN(type(/abcd/)));

        checkThrows(function() new type());
    }

    ///// test ranges and creation
    /// uint8
    // valid
    check(function() uint8(0) == 0);
    check(function() uint8(-0) == 0);
    check(function() uint8(129) == 129);
    check(function() uint8(255) == 255);

    if (typeof ctypes != "undefined") {
        check(function() uint8(ctypes.Uint64(99)) == 99);
        check(function() uint8(ctypes.Int64(99)) == 99);
    }

    // overflow is allowed for explicit conversions
    check(function() uint8(-1) == 255);
    check(function() uint8(-255) == 1);
    check(function() uint8(256) == 0);
    check(function() uint8(2345678) == 206);
    check(function() uint8(3.14) == 3);
    check(function() uint8(342.56) == 86);
    check(function() uint8(-342.56) == 170);

    if (typeof ctypes != "undefined") {
        checkThrows(function() uint8(ctypes.Uint64("18446744073709551615")) == 255);
        checkThrows(function() uint8(ctypes.Int64("0xcafebabe")) == 190);
    }

    // strings
    check(function() uint8("0") == 0);
    check(function() uint8("255") == 255);
    check(function() uint8("256") == 0);
    check(function() uint8("0x0f") == 15);
    check(function() uint8("0x00") == 0);
    check(function() uint8("0xff") == 255);
    check(function() uint8("0x1ff") == 255);
    // in JS, string literals with leading zeroes are interpreted as decimal
    check(function() uint8("-0777") == 247);
    check(function() uint8("-0xff") == 0);

    /// uint16
    // valid
    check(function() uint16(65535) == 65535);

    if (typeof ctypes != "undefined") {
        check(function() uint16(ctypes.Uint64("0xb00")) == 2816);
        check(function() uint16(ctypes.Int64("0xb00")) == 2816);
    }

    // overflow is allowed for explicit conversions
    check(function() uint16(-1) == 65535);
    check(function() uint16(-65535) == 1);
    check(function() uint16(-65536) == 0);
    check(function() uint16(65536) == 0);

    if (typeof ctypes != "undefined") {
        check(function() uint16(ctypes.Uint64("18446744073709551615")) == 65535);
        check(function() uint16(ctypes.Int64("0xcafebabe")) == 47806);
    }

    // strings
    check(function() uint16("0x1234") == 0x1234);
    check(function() uint16("0x00") == 0);
    check(function() uint16("0xffff") == 65535);
    check(function() uint16("-0xffff") == 0);
    check(function() uint16("0xffffff") == 0xffff);

    // wrong types
    check(function() uint16(3.14) == 3); // c-like casts in explicit conversion

    print("done");

    reportCompare(0, TestFailCount, "BinaryData numeric type tests");
}

runTests();
