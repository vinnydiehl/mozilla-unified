// |reftest| shell-option(--enable-new-set-methods) skip-if(!Set.prototype.isDisjointFrom)

assertEq(typeof Set.prototype.isDisjointFrom, "function");
assertDeepEq(Object.getOwnPropertyDescriptor(Set.prototype.isDisjointFrom, "length"), {
  value: 1, writable: false, enumerable: false, configurable: true,
});
assertDeepEq(Object.getOwnPropertyDescriptor(Set.prototype.isDisjointFrom, "name"), {
  value: "isDisjointFrom", writable: false, enumerable: false, configurable: true,
});

const emptySet = new Set();
const emptySetLike = new SetLike();
const emptyMap = new Map();

function asMap(values) {
  return new Map(values.map(v => [v, v]));
}

// Empty set is disjoint from the empty set.
assertEq(emptySet.isDisjointFrom(emptySet), true);
assertEq(emptySet.isDisjointFrom(emptySetLike), true);
assertEq(emptySet.isDisjointFrom(emptyMap), true);

// Test native Set, Set-like, and Map objects.
for (let values of [
  [], [1], [1, 2], [1, true, null, {}],
]) {
  // Disjoint operation with an empty set.
  assertEq(new Set(values).isDisjointFrom(emptySet), true);
  assertEq(new Set(values).isDisjointFrom(emptySetLike), true);
  assertEq(new Set(values).isDisjointFrom(emptyMap), true);
  assertEq(emptySet.isDisjointFrom(new Set(values)), true);
  assertEq(emptySet.isDisjointFrom(new SetLike(values)), true);
  assertEq(emptySet.isDisjointFrom(asMap(values)), true);

  // Two sets with the exact same values.
  assertEq(new Set(values).isDisjointFrom(new Set(values)), values.length === 0);
  assertEq(new Set(values).isDisjointFrom(new SetLike(values)), values.length === 0);
  assertEq(new Set(values).isDisjointFrom(asMap(values)), values.length === 0);

  // Disjoint operation of the same set object.
  let set = new Set(values);
  assertEq(set.isDisjointFrom(set), values.length === 0);
}

// Check property accesses are in the correct order.
{
  let log = [];

  let sizeValue = 0;

  let {proxy: keysIter} = LoggingProxy({
    next() {
      log.push("next()");
      return {done: true};
    }
  }, log);

  let {proxy: setLike} = LoggingProxy({
    size: {
      valueOf() {
        log.push("valueOf()");
        return sizeValue;
      }
    },
    has() {
      throw new Error("Unexpected call to |has| method");
    },
    keys() {
      log.push("keys()");
      return keysIter;
    },
  }, log);

  assertEq(emptySet.isDisjointFrom(setLike), true);

  assertEqArray(log, [
    "[[get]]", "size",
    "valueOf()",
    "[[get]]", "has",
    "[[get]]", "keys",
  ]);

  // |keys| is called when the this-value has more elements.

  log.length = 0;
  assertEq(new Set([1]).isDisjointFrom(setLike), true);

  assertEqArray(log, [
    "[[get]]", "size",
    "valueOf()",
    "[[get]]", "has",
    "[[get]]", "keys",
    "keys()",
    "[[get]]", "next",
    "next()",
  ]);
}

// Check input validation.
{
  let log = [];

  const nonCallable = {};
  let sizeValue = 0;

  let {proxy: keysIter} = LoggingProxy({
    next: nonCallable,
  }, log);

  let {proxy: setLike, obj: setLikeObj} = LoggingProxy({
    size: {
      valueOf() {
        log.push("valueOf()");
        return sizeValue;
      }
    },
    has() {
      throw new Error("Unexpected call to |has| method");
    },
    keys() {
      log.push("keys()");
      return keysIter;
    },
  }, log);

  log.length = 0;
  assertEq(emptySet.isDisjointFrom(setLike), true);

  assertEqArray(log, [
    "[[get]]", "size",
    "valueOf()",
    "[[get]]", "has",
    "[[get]]", "keys",
  ]);

  log.length = 0;
  assertThrowsInstanceOf(() => new Set([1]).isDisjointFrom(setLike), TypeError);

  assertEqArray(log, [
    "[[get]]", "size",
    "valueOf()",
    "[[get]]", "has",
    "[[get]]", "keys",
    "keys()",
    "[[get]]", "next",
  ]);

  // Change |keys| to return a non-object value.
  setLikeObj.keys = () => 123;

  log.length = 0;
  assertEq(emptySet.isDisjointFrom(setLike), true);

  assertEqArray(log, [
    "[[get]]", "size",
    "valueOf()",
    "[[get]]", "has",
    "[[get]]", "keys",
  ]);

  log.length = 0;
  assertThrowsInstanceOf(() => new Set([1]).isDisjointFrom(setLike), TypeError);

  assertEqArray(log, [
    "[[get]]", "size",
    "valueOf()",
    "[[get]]", "has",
    "[[get]]", "keys",
  ]);

  // Change |keys| to a non-callable value.
  setLikeObj.keys = nonCallable;

  log.length = 0;
  assertThrowsInstanceOf(() => emptySet.isDisjointFrom(setLike), TypeError);

  assertEqArray(log, [
    "[[get]]", "size",
    "valueOf()",
    "[[get]]", "has",
    "[[get]]", "keys",
  ]);

  // Change |has| to a non-callable value.
  setLikeObj.has = nonCallable;

  log.length = 0;
  assertThrowsInstanceOf(() => emptySet.isDisjointFrom(setLike), TypeError);

  assertEqArray(log, [
    "[[get]]", "size",
    "valueOf()",
    "[[get]]", "has",
  ]);

  // Change |size| to NaN.
  sizeValue = NaN;

  log.length = 0;
  assertThrowsInstanceOf(() => emptySet.isDisjointFrom(setLike), TypeError);

  assertEqArray(log, [
    "[[get]]", "size",
    "valueOf()",
  ]);

  // Change |size| to undefined.
  sizeValue = undefined;

  log.length = 0;
  assertThrowsInstanceOf(() => emptySet.isDisjointFrom(setLike), TypeError);

  assertEqArray(log, [
    "[[get]]", "size",
    "valueOf()",
  ]);
}

// Doesn't accept Array as an input.
assertThrowsInstanceOf(() => emptySet.isDisjointFrom([]), TypeError);

// Works with Set subclasses.
{
  class MySet extends Set {}

  let myEmptySet = new MySet;

  assertEq(emptySet.isDisjointFrom(myEmptySet), true);
  assertEq(myEmptySet.isDisjointFrom(myEmptySet), true);
  assertEq(myEmptySet.isDisjointFrom(emptySet), true);
}

// Doesn't access any properties on the this-value.
{
  let log = [];

  let {proxy: setProto} = LoggingProxy(Set.prototype, log);

  let set = new Set([]);
  Object.setPrototypeOf(set, setProto);

  assertEq(Set.prototype.isDisjointFrom.call(set, emptySet), true);

  assertEqArray(log, []);
}

// Throws a TypeError when the this-value isn't a Set object.
for (let thisValue of [
  null, undefined, true, "", {}, new Map, new Proxy(new Set, {}),
]) {
  assertThrowsInstanceOf(() => Set.prototype.isDisjointFrom.call(thisValue, emptySet), TypeError);
}

// Calls |has| when the this-value has fewer keys.
{
  const keys = [1, 2, 3];

  for (let size of [keys.length, 100, Infinity]) {
    let i = 0;

    let setLike = {
      size,
      has(v) {
        assertEq(this, setLike);
        assertEq(arguments.length, 1);
        assertEq(i < keys.length, true);
        assertEq(v, keys[i++]);
        return false;
      },
      keys() {
        throw new Error("Unexpected call to |keys| method");
      },
    };

    assertEq(new Set([1, 2, 3]).isDisjointFrom(setLike), true);
  }
}

// Calls |keys| when the this-value has more keys.
{
  const keys = [1, 2, 3];

  for (let size of [0, 1, 2]) {
    let setLike = {
      size,
      has() {
        throw new Error("Unexpected call to |has| method");
      },
      *keys() {
        yield* [4, 5, 6];
      },
    };

    assertEq(new Set(keys).isDisjointFrom(setLike), true);
  }

  // Also test early return after first match.
  for (let size of [0, 1, 2]) {
    let setLike = {
      size,
      has() {
        throw new Error("Unexpected call to |has| method");
      },
      *keys() {
        yield keys[0];

        throw new Error("keys iterator called too many times");
      },
    };

    assertEq(new Set(keys).isDisjointFrom(setLike), false);
  }
}

// Test when this-value is modified during iteration.
{
  let set = new Set([]);

  // |setLike| has more entries than |set|.
  let setLike = {
    size: 100,
    has(v) {
      assertEq(set.has(v), true);
      set.delete(v);
      return false;
    },
    keys() {
      throw new Error("Unexpected call to |keys| method");
    },
  };

  assertEq(set.isDisjointFrom(setLike), true);
  assertSetContainsExactOrderedItems(set, []);
}
{
  let set = new Set([0]);

  let keys = [1, 2, 3];

  let lastValue;
  let keysIter = {
    next() {
      if (lastValue !== undefined) {
        assertEq(set.has(lastValue), false);
        set.add(lastValue);

        lastValue = undefined;
      }

      if (keys.length) {
        let value = keys.shift();
        lastValue = value;
        return {
          done: false,
          get value() {
            return value;
          }
        };
      }
      return {
        done: true,
        get value() {
          throw new Error("Unexpected call to |value| getter");
        }
      };
    }
  };

  // |setLike| has fewer entries than |set|.
  let setLike = {
    size: 0,
    has() {
      throw new Error("Unexpected call to |has| method");
    },
    keys() {
      return keysIter;
    },
  };

  assertEq(set.isDisjointFrom(setLike), true);
  assertSetContainsExactOrderedItems(set, [0, 1, 2, 3]);
}

// IteratorClose is called for early returns.
{
  let log = [];

  let keysIter = {
    next() {
      log.push("next");
      return {done: false, value: 1};
    },
    return() {
      log.push("return");
      return {
        get value() { throw new Error("Unexpected call to |value| getter"); },
        get done() { throw new Error("Unexpected call to |done| getter"); },
      };
    }
  };

  let setLike = {
    size: 0,
    has() {
      throw new Error("Unexpected call to |has| method");
    },
    keys() {
      return keysIter;
    },
  };

  assertEq(new Set([1, 2, 3]).isDisjointFrom(setLike), false);

  assertEqArray(log, ["next", "return"]);
}

// IteratorClose isn't called for non-early returns.
{
  let setLike = new SetLike([4, 5, 6]);

  assertEq(new Set([1, 2, 3]).isDisjointFrom(setLike), true);
}

// Tests which modify any built-ins should appear last, because modifications may disable
// optimised code paths.

// Doesn't call the built-in |Set.prototype.{has, keys, size}| functions.
const SetPrototypeHas = Object.getOwnPropertyDescriptor(Set.prototype, "has");
const SetPrototypeKeys = Object.getOwnPropertyDescriptor(Set.prototype, "keys");
const SetPrototypeSize = Object.getOwnPropertyDescriptor(Set.prototype, "size");

delete Set.prototype.has;
delete Set.prototype.keys;
delete Set.prototype.size;

try {
  let set = new Set([1, 2, 3]);
  let other = new SetLike([1, 2, 3]);
  assertEq(set.isDisjointFrom(other), false);
} finally {
  Object.defineProperty(Set.prototype, "has", SetPrototypeHas);
  Object.defineProperty(Set.prototype, "keys", SetPrototypeKeys);
  Object.defineProperty(Set.prototype, "size", SetPrototypeSize);
}

if (typeof reportCompare === "function")
  reportCompare(true, true);
