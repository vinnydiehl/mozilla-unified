/*
 *  Copyright (c) 2011 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

/******************************************************************

 iLBC Speech Coder ANSI-C Source Code

 WebRtcIlbcfix_NearestNeighbor.h

******************************************************************/

#ifndef WEBRTC_MODULES_AUDIO_CODING_CODECS_ILBC_MAIN_SOURCE_NEAREST_NEIGHBOR_H_
#define WEBRTC_MODULES_AUDIO_CODING_CODECS_ILBC_MAIN_SOURCE_NEAREST_NEIGHBOR_H_

#include "defines.h"

/*----------------------------------------------------------------*
 * Find index in array such that the array element with said
 * index is the element of said array closest to "value"
 * according to the squared-error criterion
 *---------------------------------------------------------------*/

void WebRtcIlbcfix_NearestNeighbor(
    int16_t *index, /* (o) index of array element closest to value */
    int16_t *array, /* (i) data array (Q2) */
    int16_t value, /* (i) value (Q2) */
    int16_t arlength /* (i) dimension of data array (==8) */
                                   );

#endif
