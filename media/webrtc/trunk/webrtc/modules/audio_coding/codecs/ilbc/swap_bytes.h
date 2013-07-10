/*
 *  Copyright (c) 2012 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

/******************************************************************

 iLBC Speech Coder ANSI-C Source Code

 WebRtcIlbcfix_SwapBytes.h

******************************************************************/

#ifndef WEBRTC_MODULES_AUDIO_CODING_CODECS_ILBC_MAIN_SOURCE_SWAP_BYTES_H_
#define WEBRTC_MODULES_AUDIO_CODING_CODECS_ILBC_MAIN_SOURCE_SWAP_BYTES_H_

#include "defines.h"

/*----------------------------------------------------------------*
 * Swap bytes (to simplify operations on Little Endian machines)
 *---------------------------------------------------------------*/

void WebRtcIlbcfix_SwapBytes(
    const uint16_t* input,   /* (i) the sequence to swap */
    int16_t wordLength,      /* (i) number or uint16_t to swap */
    uint16_t* output         /* (o) the swapped sequence */
                              );

#endif
