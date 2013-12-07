/*
 *  Copyright (c) 2013 Mozilla Foundation. All Rights Reserved.
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.
 */

#if defined(_WIN64)
/* 64 bit Windows */
#include "vpx_scale_rtcd_x86_64-win64-vs8.h"

#elif defined(_WIN32)
/* 32 bit Windows, MSVC. */
#include "vpx_scale_rtcd_x86-win32-vs8.h"

#elif defined(__APPLE__) && defined(__x86_64__)
/* 64 bit MacOS. */
#include "vpx_scale_rtcd_x86_64-darwin9-gcc.h"

#elif defined(__APPLE__) && defined(__i386__)
/* 32 bit MacOS. */
#include "vpx_scale_rtcd_x86-darwin9-gcc.h"

#elif defined(__ELF__) && (defined(__i386) || defined(__i386__))
/* 32 bit ELF platforms. */
#include "vpx_scale_rtcd_x86-linux-gcc.h"

#elif defined(__ELF__) && (defined(__x86_64) || defined(__x86_64__))
/* 64 bit ELF platforms. */
#include "vpx_scale_rtcd_x86_64-linux-gcc.h"

#elif defined(VPX_ARM_ASM)
/* Android */
#include "vpx_scale_rtcd_armv7-android-gcc.h"

#else
/* Assume generic GNU/GCC configuration. */
#include "vpx_scale_rtcd_generic-gnu.h"
#endif
