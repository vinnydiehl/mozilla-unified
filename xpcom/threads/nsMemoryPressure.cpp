/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim:set ts=2 sw=2 sts=2 et cindent: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "nsMemoryPressure.h"
#include "mozilla/Assertions.h"
#include "mozilla/Atomics.h"

#include "nsThreadUtils.h"

using namespace mozilla;

static Atomic<int32_t, Relaxed> sMemoryPressurePending;
MOZ_STATIC_ASSERT(MemPressure_None == 0,
                  "Bad static initialization with the default constructor.");

MemoryPressureState
NS_GetPendingMemoryPressure()
{
  int32_t value = sMemoryPressurePending.exchange(MemPressure_None);
  return MemoryPressureState(value);
}

void
NS_DispatchEventualMemoryPressure(MemoryPressureState state)
{
  /*
   * A new memory pressure event erases an ongoing memory pressure, but an
   * existing "new" memory pressure event takes precedence over a new "ongoing"
   * memory pressure event.
   */
  switch (state) {
    case MemPressure_None:
      sMemoryPressurePending = MemPressure_None;
      break;
    case MemPressure_New:
      sMemoryPressurePending = MemPressure_New;
      break;
    case MemPressure_Ongoing:
      sMemoryPressurePending.compareExchange(MemPressure_None, MemPressure_Ongoing);
      break;
  }
}

nsresult
NS_DispatchMemoryPressure(MemoryPressureState state)
{
  NS_DispatchEventualMemoryPressure(state);
  nsCOMPtr<nsIRunnable> event = new nsRunnable;
  NS_ENSURE_TRUE(event, NS_ERROR_OUT_OF_MEMORY);
  return NS_DispatchToMainThread(event);
}
