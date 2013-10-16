/* -*- Mode: IDL; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * The origin of this IDL file is
 * http://www.w3.org/TR/hr-time/
 *
 * Copyright © 2012 W3C® (MIT, ERCIM, Keio), All Rights Reserved. W3C
 * liability, trademark and document use rules apply.
 */

typedef double DOMHighResTimeStamp;
typedef sequence <PerformanceEntry> PerformanceEntryList;

interface Performance {
  DOMHighResTimeStamp now();

  [Constant]
  readonly attribute PerformanceTiming timing;
  [Constant]
  readonly attribute PerformanceNavigation navigation;

  jsonifier;
};

// http://www.w3c-test.org/webperf/specs/PerformanceTimeline/#sec-window.performance-attribute
partial interface Performance {
  PerformanceEntryList getEntries();
  PerformanceEntryList getEntriesByType(DOMString entryType);
  PerformanceEntryList getEntriesByName(DOMString name, optional DOMString
    entryType);
};

// http://w3c-test.org/webperf/specs/ResourceTiming/#extensions-performance-interface
partial interface Performance {
  void clearResourceTimings();
  void setResourceTimingBufferSize(unsigned long maxSize);
};
