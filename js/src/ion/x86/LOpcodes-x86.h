/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 4 -*-
 * vim: set ts=8 sts=4 et sw=4 tw=99:
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef ion_x86_LOpcodes_x86_h
#define ion_x86_LOpcodes_x86_h

#define LIR_CPU_OPCODE_LIST(_)  \
    _(Unbox)                    \
    _(UnboxDouble)              \
    _(Box)                      \
    _(BoxDouble)                \
    _(DivI)                     \
    _(DivPowTwoI)               \
    _(ModI)                     \
    _(ModPowTwoI)               \
    _(PowHalfD)                 \
    _(UInt32ToDouble)           \
    _(AsmJSLoadFuncPtr)         \
    _(UDivOrMod)

#endif /* ion_x86_LOpcodes_x86_h */
