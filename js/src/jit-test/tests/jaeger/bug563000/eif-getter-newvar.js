// |jit-test| debug
setDebug(true);

this.__defineGetter__("someProperty", function () { evalInFrame(1, "var x = 'success'"); });
function caller(obj) {
  var x = 'ignominy';
  obj.someProperty;
  return x;
}
assertEq(caller(this), "success");
