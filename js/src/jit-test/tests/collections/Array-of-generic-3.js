// Array.of can be transplanted to builtin constructors.

load(libdir + "asserts.js");

Uint8Array.of = Array.of;
assertDeepEq(Uint8Array.of(0x12, 0x34, 0x5678, 0x9abcdef), Uint8Array([0x12, 0x34, 0x78, 0xef]));
