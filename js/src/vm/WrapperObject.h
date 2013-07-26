/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 4 -*-
 * vim: set ts=8 sts=4 et sw=4 tw=99:
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef vm_WrapperObject_h
#define vm_WrapperObject_h

#include "jsobj.h"
#include "jswrapper.h"

#include "vm/ProxyObject.h"

namespace js {

// Proxy family for wrappers.
extern int sWrapperFamily;

class WrapperObject : public ProxyObject
{
};

class CrossCompartmentWrapperObject : public WrapperObject
{
};

} // namespace js

template<>
inline bool
JSObject::is<js::WrapperObject>() const
{
    return IsWrapper(const_cast<JSObject*>(this));
}

template<>
inline bool
JSObject::is<js::CrossCompartmentWrapperObject>() const
{
    return IsCrossCompartmentWrapper(const_cast<JSObject*>(this));
}

#endif /* vm_WrapperObject_h */
