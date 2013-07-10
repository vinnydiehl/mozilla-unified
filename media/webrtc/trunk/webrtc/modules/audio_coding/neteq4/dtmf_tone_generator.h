/*
 *  Copyright (c) 2012 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef WEBRTC_MODULES_AUDIO_CODING_NETEQ4_DTMF_TONE_GENERATOR_H_
#define WEBRTC_MODULES_AUDIO_CODING_NETEQ4_DTMF_TONE_GENERATOR_H_


#include "webrtc/modules/audio_coding/neteq4/audio_multi_vector.h"
#include "webrtc/system_wrappers/interface/constructor_magic.h"
#include "webrtc/typedefs.h"

namespace webrtc {

// This class provides a generator for DTMF tones.
class DtmfToneGenerator {
 public:
  enum ReturnCodes {
    kNotInitialized = -1,
    kParameterError = -2,
    kStereoNotSupported = -3,
  };

  DtmfToneGenerator();
  virtual ~DtmfToneGenerator() {}
  virtual int Init(int fs, int event, int attenuation);
  virtual void Reset();
  virtual int Generate(int num_samples, AudioMultiVector<int16_t>* output);
  virtual bool initialized() const { return initialized_; }

 private:
  static const int kCoeff1[4][16];  // 1st oscillator model coefficient table.
  static const int kCoeff2[4][16];  // 2nd oscillator model coefficient table.
  static const int kInitValue1[4][16];  // Initialization for 1st oscillator.
  static const int kInitValue2[4][16];  // Initialization for 2nd oscillator.
  static const int kAmplitude[37];  // Amplitude for 0 through -36 dBm0.
  static const int16_t kAmpMultiplier = 23171;  // 3 dB attenuation (in Q15).

  bool initialized_;            // True if generator is initialized properly.
  int coeff1_;                  // 1st oscillator coefficient for this event.
  int coeff2_;                  // 2nd oscillator coefficient for this event.
  int amplitude_;               // Amplitude for this event.
  int16_t sample_history1_[2];  // Last 2 samples for the 1st oscillator.
  int16_t sample_history2_[2];  // Last 2 samples for the 2nd oscillator.

  DISALLOW_COPY_AND_ASSIGN(DtmfToneGenerator);
};

}  // namespace webrtc
#endif  // WEBRTC_MODULES_AUDIO_CODING_NETEQ4_DTMF_TONE_GENERATOR_H_
