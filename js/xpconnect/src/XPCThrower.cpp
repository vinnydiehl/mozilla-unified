/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/* vim: set ts=8 sts=4 et sw=4 tw=99: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/* Code for throwing errors into JavaScript. */

#include "xpcprivate.h"
#include "XPCWrapper.h"
#include "jsprf.h"
#include "mozilla/dom/BindingUtils.h"
#include "mozilla/dom/Exceptions.h"

using namespace mozilla;
using namespace mozilla::dom;

bool XPCThrower::sVerbose = true;

// static
void
XPCThrower::Throw(nsresult rv, JSContext* cx)
{
    const char* format;
    if (JS_IsExceptionPending(cx))
        return;
    if (!nsXPCException::NameAndFormatForNSResult(rv, nullptr, &format))
        format = "";
    dom::Throw(cx, rv, format);
}

namespace xpc {

bool
Throw(JSContext *cx, nsresult rv)
{
    XPCThrower::Throw(rv, cx);
    return false;
}

} // namespace xpc

/*
 * If there has already been an exception thrown, see if we're throwing the
 * same sort of exception, and if we are, don't clobber the old one. ccx
 * should be the current call context.
 */
// static
bool
XPCThrower::CheckForPendingException(nsresult result, JSContext *cx)
{
    nsCOMPtr<nsIException> e = XPCJSRuntime::Get()->GetPendingException();
    if (!e)
        return false;
    XPCJSRuntime::Get()->SetPendingException(nullptr);

    nsresult e_result;
    if (NS_FAILED(e->GetResult(&e_result)) || e_result != result)
        return false;

    if (!ThrowExceptionObject(cx, e))
        JS_ReportOutOfMemory(cx);
    return true;
}

// static
void
XPCThrower::Throw(nsresult rv, XPCCallContext& ccx)
{
    char* sz;
    const char* format;

    if (CheckForPendingException(rv, ccx))
        return;

    if (!nsXPCException::NameAndFormatForNSResult(rv, nullptr, &format))
        format = "";

    sz = (char*) format;

    if (sz && sVerbose)
        Verbosify(ccx, &sz, false);

    dom::Throw(ccx, rv, sz);

    if (sz && sz != format)
        JS_smprintf_free(sz);
}


// static
void
XPCThrower::ThrowBadResult(nsresult rv, nsresult result, XPCCallContext& ccx)
{
    char* sz;
    const char* format;
    const char* name;

    /*
    *  If there is a pending exception when the native call returns and
    *  it has the same error result as returned by the native call, then
    *  the native call may be passing through an error from a previous JS
    *  call. So we'll just throw that exception into our JS.
    */

    if (CheckForPendingException(result, ccx))
        return;

    // else...

    if (!nsXPCException::NameAndFormatForNSResult(rv, nullptr, &format) || !format)
        format = "";

    if (nsXPCException::NameAndFormatForNSResult(result, &name, nullptr) && name)
        sz = JS_smprintf("%s 0x%x (%s)", format, result, name);
    else
        sz = JS_smprintf("%s 0x%x", format, result);

    if (sz && sVerbose)
        Verbosify(ccx, &sz, true);

    dom::Throw(ccx, result, sz);

    if (sz)
        JS_smprintf_free(sz);
}

// static
void
XPCThrower::ThrowBadParam(nsresult rv, unsigned paramNum, XPCCallContext& ccx)
{
    char* sz;
    const char* format;

    if (!nsXPCException::NameAndFormatForNSResult(rv, nullptr, &format))
        format = "";

    sz = JS_smprintf("%s arg %d", format, paramNum);

    if (sz && sVerbose)
        Verbosify(ccx, &sz, true);

    dom::Throw(ccx, rv, sz);

    if (sz)
        JS_smprintf_free(sz);
}


// static
void
XPCThrower::Verbosify(XPCCallContext& ccx,
                      char** psz, bool own)
{
    char* sz = nullptr;

    if (ccx.HasInterfaceAndMember()) {
        XPCNativeInterface* iface = ccx.GetInterface();
        jsid id = ccx.GetMember()->GetName();
        JSAutoByteString bytes;
        const char *name = JSID_IS_VOID(id) ? "Unknown" : bytes.encodeLatin1(ccx, JSID_TO_STRING(id));
        if (!name) {
            name = "";
        }
        sz = JS_smprintf("%s [%s.%s]", *psz, iface->GetNameString(), name);
    }

    if (sz) {
        if (own)
            JS_smprintf_free(*psz);
        *psz = sz;
    }
}
