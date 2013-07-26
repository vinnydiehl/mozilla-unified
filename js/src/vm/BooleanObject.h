/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 4 -*-
 * vim: set ts=8 sts=4 et sw=4 tw=99:
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef vm_BooleanObject_h
#define vm_BooleanObject_h

#include "jsbool.h"
#include "jsobj.h"

namespace js {

class BooleanObject : public JSObject
{
    /* Stores this Boolean object's [[PrimitiveValue]]. */
    static const unsigned PRIMITIVE_VALUE_SLOT = 0;

  public:
    static const unsigned RESERVED_SLOTS = 1;

    static Class class_;

    /*
     * Creates a new Boolean object boxing the given primitive bool.  The
     * object's [[Prototype]] is determined from context.
     */
    static inline BooleanObject *create(JSContext *cx, bool b);

    bool unbox() const {
        return getFixedSlot(PRIMITIVE_VALUE_SLOT).toBoolean();
    }

  private:
    inline void setPrimitiveValue(bool b) {
        setFixedSlot(PRIMITIVE_VALUE_SLOT, BooleanValue(b));
    }

    /* For access to init, as Boolean.prototype is special. */
    friend JSObject *
    ::js_InitBooleanClass(JSContext *cx, js::HandleObject global);
};

} // namespace js

#endif /* vm_BooleanObject_h */
