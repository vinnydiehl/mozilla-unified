/*
 *  Copyright (c) 2012 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef WEBRTC_MODULES_AUDIO_PROCESSING_NS_MAIN_SOURCE_NSX_CORE_H_
#define WEBRTC_MODULES_AUDIO_PROCESSING_NS_MAIN_SOURCE_NSX_CORE_H_

#ifdef NS_FILEDEBUG
#include <stdio.h>
#endif

#include "webrtc/common_audio/signal_processing/include/signal_processing_library.h"
#include "webrtc/modules/audio_processing/ns/nsx_defines.h"
#include "webrtc/typedefs.h"

typedef struct NsxInst_t_ {
  uint32_t                fs;

  const int16_t*          window;
  int16_t                 analysisBuffer[ANAL_BLOCKL_MAX];
  int16_t                 synthesisBuffer[ANAL_BLOCKL_MAX];
  uint16_t                noiseSupFilter[HALF_ANAL_BLOCKL];
  uint16_t                overdrive; /* Q8 */
  uint16_t                denoiseBound; /* Q14 */
  const int16_t*          factor2Table;
  int16_t                 noiseEstLogQuantile[SIMULT* HALF_ANAL_BLOCKL];
  int16_t                 noiseEstDensity[SIMULT* HALF_ANAL_BLOCKL];
  int16_t                 noiseEstCounter[SIMULT];
  int16_t                 noiseEstQuantile[HALF_ANAL_BLOCKL];

  int                     anaLen;
  int                     anaLen2;
  int                     magnLen;
  int                     aggrMode;
  int                     stages;
  int                     initFlag;
  int                     gainMap;

  int32_t                 maxLrt;
  int32_t                 minLrt;
  // Log LRT factor with time-smoothing in Q8.
  int32_t                 logLrtTimeAvgW32[HALF_ANAL_BLOCKL];
  int32_t                 featureLogLrt;
  int32_t                 thresholdLogLrt;
  int16_t                 weightLogLrt;

  uint32_t                featureSpecDiff;
  uint32_t                thresholdSpecDiff;
  int16_t                 weightSpecDiff;

  uint32_t                featureSpecFlat;
  uint32_t                thresholdSpecFlat;
  int16_t                 weightSpecFlat;

  // Conservative estimate of noise spectrum.
  int32_t                 avgMagnPause[HALF_ANAL_BLOCKL];
  uint32_t                magnEnergy;
  uint32_t                sumMagn;
  uint32_t                curAvgMagnEnergy;
  uint32_t                timeAvgMagnEnergy;
  uint32_t                timeAvgMagnEnergyTmp;

  uint32_t                whiteNoiseLevel;  // Initial noise estimate.
  // Initial magnitude spectrum estimate.
  uint32_t                initMagnEst[HALF_ANAL_BLOCKL];
  // Pink noise parameters:
  int32_t                 pinkNoiseNumerator;  // Numerator.
  int32_t                 pinkNoiseExp;  // Power of freq.
  int                     minNorm;  // Smallest normalization factor.
  int                     zeroInputSignal;  // Zero input signal flag.

  // Noise spectrum from previous frame.
  uint32_t                prevNoiseU32[HALF_ANAL_BLOCKL];
  // Magnitude spectrum from previous frame.
  uint16_t                prevMagnU16[HALF_ANAL_BLOCKL];
  // Prior speech/noise probability in Q14.
  int16_t                 priorNonSpeechProb;

  int                     blockIndex;  // Frame index counter.
  // Parameter for updating or estimating thresholds/weights for prior model.
  int                     modelUpdate;
  int                     cntThresUpdate;

  // Histograms for parameter estimation.
  int16_t                 histLrt[HIST_PAR_EST];
  int16_t                 histSpecFlat[HIST_PAR_EST];
  int16_t                 histSpecDiff[HIST_PAR_EST];

  // Quantities for high band estimate.
  int16_t                 dataBufHBFX[ANAL_BLOCKL_MAX];  // Q0

  int                     qNoise;
  int                     prevQNoise;
  int                     prevQMagn;
  int                     blockLen10ms;

  int16_t                 real[ANAL_BLOCKL_MAX];
  int16_t                 imag[ANAL_BLOCKL_MAX];
  int32_t                 energyIn;
  int                     scaleEnergyIn;
  int                     normData;

  struct RealFFT* real_fft;
} NsxInst_t;

#ifdef __cplusplus
extern "C"
{
#endif

/****************************************************************************
 * WebRtcNsx_InitCore(...)
 *
 * This function initializes a noise suppression instance
 *
 * Input:
 *      - inst          : Instance that should be initialized
 *      - fs            : Sampling frequency
 *
 * Output:
 *      - inst          : Initialized instance
 *
 * Return value         :  0 - Ok
 *                        -1 - Error
 */
int32_t WebRtcNsx_InitCore(NsxInst_t* inst, uint32_t fs);

/****************************************************************************
 * WebRtcNsx_set_policy_core(...)
 *
 * This changes the aggressiveness of the noise suppression method.
 *
 * Input:
 *      - inst       : Instance that should be initialized
 *      - mode       : 0: Mild (6 dB), 1: Medium (10 dB), 2: Aggressive (15 dB)
 *
 * Output:
 *      - inst       : Initialized instance
 *
 * Return value      :  0 - Ok
 *                     -1 - Error
 */
int WebRtcNsx_set_policy_core(NsxInst_t* inst, int mode);

/****************************************************************************
 * WebRtcNsx_ProcessCore
 *
 * Do noise suppression.
 *
 * Input:
 *      - inst          : Instance that should be initialized
 *      - inFrameLow    : Input speech frame for lower band
 *      - inFrameHigh   : Input speech frame for higher band
 *
 * Output:
 *      - inst          : Updated instance
 *      - outFrameLow   : Output speech frame for lower band
 *      - outFrameHigh  : Output speech frame for higher band
 *
 * Return value         :  0 - OK
 *                        -1 - Error
 */
int WebRtcNsx_ProcessCore(NsxInst_t* inst,
                          short* inFrameLow,
                          short* inFrameHigh,
                          short* outFrameLow,
                          short* outFrameHigh);

/****************************************************************************
 * Some function pointers, for internal functions shared by ARM NEON and 
 * generic C code.
 */
// Noise Estimation.
typedef void (*NoiseEstimation)(NsxInst_t* inst,
                                uint16_t* magn,
                                uint32_t* noise,
                                int16_t* q_noise);
extern NoiseEstimation WebRtcNsx_NoiseEstimation;

// Filter the data in the frequency domain, and create spectrum.
typedef void (*PrepareSpectrum)(NsxInst_t* inst,
                                int16_t* freq_buff);
extern PrepareSpectrum WebRtcNsx_PrepareSpectrum;

// For the noise supression process, synthesis, read out fully processed
// segment, and update synthesis buffer.
typedef void (*SynthesisUpdate)(NsxInst_t* inst,
                                int16_t* out_frame,
                                int16_t gain_factor);
extern SynthesisUpdate WebRtcNsx_SynthesisUpdate;

// Update analysis buffer for lower band, and window data before FFT.
typedef void (*AnalysisUpdate)(NsxInst_t* inst,
                               int16_t* out,
                               int16_t* new_speech);
extern AnalysisUpdate WebRtcNsx_AnalysisUpdate;

// Denormalize the input buffer.
typedef void (*Denormalize)(NsxInst_t* inst,
                            int16_t* in,
                            int factor);
extern Denormalize WebRtcNsx_Denormalize;

// Create a complex number buffer, as the intput interleaved with zeros,
// and normalize it.
typedef void (*CreateComplexBuffer)(NsxInst_t* inst,
                                    int16_t* in,
                                    int16_t* out);
extern CreateComplexBuffer WebRtcNsx_CreateComplexBuffer;

#if (defined WEBRTC_DETECT_ARM_NEON) || defined (WEBRTC_ARCH_ARM_NEON)
// For the above function pointers, functions for generic platforms are declared
// and defined as static in file nsx_core.c, while those for ARM Neon platforms
// are declared below and defined in file nsx_core_neon.S.
void WebRtcNsx_NoiseEstimationNeon(NsxInst_t* inst,
                                   uint16_t* magn,
                                   uint32_t* noise,
                                   int16_t* q_noise);
void WebRtcNsx_CreateComplexBufferNeon(NsxInst_t* inst,
                                       int16_t* in,
                                       int16_t* out);
void WebRtcNsx_SynthesisUpdateNeon(NsxInst_t* inst,
                                   int16_t* out_frame,
                                   int16_t gain_factor);
void WebRtcNsx_AnalysisUpdateNeon(NsxInst_t* inst,
                                  int16_t* out,
                                  int16_t* new_speech);
void WebRtcNsx_DenormalizeNeon(NsxInst_t* inst, int16_t* in, int factor);
void WebRtcNsx_PrepareSpectrumNeon(NsxInst_t* inst, int16_t* freq_buff);
#endif

#ifdef __cplusplus
}
#endif

#endif  // WEBRTC_MODULES_AUDIO_PROCESSING_NS_MAIN_SOURCE_NSX_CORE_H_
