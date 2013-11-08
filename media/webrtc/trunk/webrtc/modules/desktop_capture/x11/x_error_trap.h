/*
 *  Copyright (c) 2013 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef WEBRTC_MODULES_DESKTOP_CAPTURE_X11_X_ERROR_TRAP_H_
#define WEBRTC_MODULES_DESKTOP_CAPTURE_X11_X_ERROR_TRAP_H_

#include <X11/Xlib.h>

#include "webrtc/system_wrappers/interface/constructor_magic.h"

namespace webrtc {

// Helper class that registers X Window error handler. Caller can use
// GetLastErrorAndDisable() to get the last error that was caught, if any.
class XErrorTrap {
 public:
  explicit XErrorTrap(Display* display);
  ~XErrorTrap();

  // Returns last error and removes unregisters the error handler.
  int GetLastErrorAndDisable();

 private:
  XErrorHandler original_error_handler_;
  bool enabled_;

  DISALLOW_COPY_AND_ASSIGN(XErrorTrap);
};

}  // namespace webrtc

#endif  // WEBRTC_MODULES_DESKTOP_CAPTURE_X11_X_ERROR_TRAP_H_
