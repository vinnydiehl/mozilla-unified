/*
 *  Copyright (c) 2012 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "webrtc/modules/audio_coding/neteq4/merge.h"

#include <assert.h>

#include <algorithm>  // min, max
#include <cstring>  // memmove, memcpy, memset, size_t

#include "webrtc/common_audio/signal_processing/include/signal_processing_library.h"
#include "webrtc/modules/audio_coding/neteq4/audio_multi_vector.h"
#include "webrtc/modules/audio_coding/neteq4/dsp_helper.h"
#include "webrtc/modules/audio_coding/neteq4/expand.h"
#include "webrtc/modules/audio_coding/neteq4/sync_buffer.h"

namespace webrtc {

int Merge::Process(int16_t* input, int input_length,
                   int16_t* external_mute_factor_array,
                   AudioMultiVector<int16_t>* output) {
  // TODO(hlundin): Change to an enumerator and skip assert.
  assert(fs_hz_ == 8000 || fs_hz_ == 16000 || fs_hz_ ==  32000 ||
         fs_hz_ == 48000);
  assert(fs_hz_ <= kMaxSampleRate);  // Should not be possible.

  int old_length;
  int expand_period;
  // Get expansion data to overlap and mix with.
  int expanded_length = GetExpandedSignal(&old_length, &expand_period);

  // Transfer input signal to an AudioMultiVector.
  AudioMultiVector<int16_t> input_vector(num_channels_);
  input_vector.PushBackInterleaved(input, input_length);
  size_t input_length_per_channel = input_vector.Size();
  assert(input_length_per_channel == input_length / num_channels_);

  int16_t best_correlation_index = 0;
  size_t output_length = 0;

  for (size_t channel = 0; channel < num_channels_; ++channel) {
    int16_t* input_channel = &input_vector[channel][0];
    int16_t* expanded_channel = &expanded_[channel][0];
    int16_t expanded_max, input_max;
    int16_t new_mute_factor = SignalScaling(input_channel,
                                            input_length_per_channel,
                                            expanded_channel, &expanded_max,
                                            &input_max);

    // Adjust muting factor (product of "main" muting factor and expand muting
    // factor).
    int16_t* external_mute_factor = &external_mute_factor_array[channel];
    *external_mute_factor =
        (*external_mute_factor * expand_->MuteFactor(channel)) >> 14;

    // Update |external_mute_factor| if it is lower than |new_mute_factor|.
    if (new_mute_factor > *external_mute_factor) {
      *external_mute_factor = std::min(new_mute_factor,
                                       static_cast<int16_t>(16384));
    }

    if (channel == 0) {
      // Downsample, correlate, and find strongest correlation period for the
      // master (i.e., first) channel only.
      // Downsample to 4kHz sample rate.
      Downsample(input_channel, input_length_per_channel, expanded_channel,
                 expanded_length);

      // Calculate the lag of the strongest correlation period.
      best_correlation_index = CorrelateAndPeakSearch(expanded_max,
                                                      input_max,
                                                      old_length,
                                                      input_length_per_channel,
                                                      expand_period);
    }

    static const int kTempDataSize = 3600;
    int16_t temp_data[kTempDataSize];  // TODO(hlundin) Remove this.
    int16_t* decoded_output = temp_data + best_correlation_index;

    // Mute the new decoded data if needed (and unmute it linearly).
    // This is the overlapping part of expanded_signal.
    int interpolation_length = std::min(
        kMaxCorrelationLength * fs_mult_,
        expanded_length - best_correlation_index);
    interpolation_length = std::min(interpolation_length,
                                    static_cast<int>(input_length_per_channel));
    if (*external_mute_factor < 16384) {
      // Set a suitable muting slope (Q20). 0.004 for NB, 0.002 for WB,
      // and so on.
      int increment = 4194 / fs_mult_;
      *external_mute_factor = DspHelper::RampSignal(input_channel,
                                                    interpolation_length,
                                                    *external_mute_factor,
                                                    increment);
      DspHelper::UnmuteSignal(&input_channel[interpolation_length],
                              input_length_per_channel - interpolation_length,
                              external_mute_factor, increment,
                              &decoded_output[interpolation_length]);
    } else {
      // No muting needed.
      memmove(
          &decoded_output[interpolation_length],
          &input_channel[interpolation_length],
          sizeof(int16_t) * (input_length_per_channel - interpolation_length));
    }

    // Do overlap and mix linearly.
    int increment = 16384 / (interpolation_length + 1);  // In Q14.
    int16_t mute_factor = 16384 - increment;
    memmove(temp_data, expanded_channel,
            sizeof(int16_t) * best_correlation_index);
    DspHelper::CrossFade(&expanded_channel[best_correlation_index],
                         input_channel, interpolation_length,
                         &mute_factor, increment, decoded_output);

    output_length = best_correlation_index + input_length_per_channel;
    if (channel == 0) {
      assert(output->Empty());  // Output should be empty at this point.
      output->AssertSize(output_length);
    } else {
      assert(output->Size() == output_length);
    }
    memcpy(&(*output)[channel][0], temp_data,
           sizeof(temp_data[0]) * output_length);
  }

  // Copy back the first part of the data to |sync_buffer_| and remove it from
  // |output|.
  sync_buffer_->ReplaceAtIndex(*output, old_length, sync_buffer_->next_index());
  output->PopFront(old_length);

  // Return new added length. |old_length| samples were borrowed from
  // |sync_buffer_|.
  return output_length - old_length;
}

int Merge::GetExpandedSignal(int* old_length, int* expand_period) {
  // Check how much data that is left since earlier.
  *old_length = sync_buffer_->FutureLength();
  // Should never be less than overlap_length.
  assert(*old_length >= static_cast<int>(expand_->overlap_length()));
  // Generate data to merge the overlap with using expand.
  expand_->SetParametersForMergeAfterExpand();

  if (*old_length >= 210 * kMaxSampleRate / 8000) {
    // TODO(hlundin): Write test case for this.
    // The number of samples available in the sync buffer is more than what fits
    // in expanded_signal. Keep the first 210 * kMaxSampleRate / 8000 samples,
    // but shift them towards the end of the buffer. This is ok, since all of
    // the buffer will be expand data anyway, so as long as the beginning is
    // left untouched, we're fine.
    int16_t length_diff = *old_length - 210 * kMaxSampleRate / 8000;
    sync_buffer_->InsertZerosAtIndex(length_diff, sync_buffer_->next_index());
    *old_length = 210 * kMaxSampleRate / 8000;
    // This is the truncated length.
  }
  // This assert should always be true thanks to the if statement above.
  assert(210 * kMaxSampleRate / 8000 - *old_length >= 0);

  AudioMultiVector<int16_t> expanded_temp(num_channels_);
  expand_->Process(&expanded_temp);
  *expand_period = expanded_temp.Size();  // Samples per channel.

  expanded_.Clear();
  // Copy what is left since earlier into the expanded vector.
  expanded_.PushBackFromIndex(*sync_buffer_, sync_buffer_->next_index());
  assert(expanded_.Size() == static_cast<size_t>(*old_length));
  assert(expanded_temp.Size() > 0);
  // Do "ugly" copy and paste from the expanded in order to generate more data
  // to correlate (but not interpolate) with.
  const int required_length = (120 + 80 + 2) * fs_mult_;
  if (expanded_.Size() < static_cast<size_t>(required_length)) {
    while (expanded_.Size() < static_cast<size_t>(required_length)) {
      // Append one more pitch period each time.
      expanded_.PushBack(expanded_temp);
    }
    // Trim the length to exactly |required_length|.
    expanded_.PopBack(expanded_.Size() - required_length);
  }
  assert(expanded_.Size() >= static_cast<size_t>(required_length));
  return required_length;
}

int16_t Merge::SignalScaling(const int16_t* input, int input_length,
                             const int16_t* expanded_signal,
                             int16_t* expanded_max, int16_t* input_max) const {
  // Adjust muting factor if new vector is more or less of the BGN energy.
  const int mod_input_length = std::min(64 * fs_mult_, input_length);
  *expanded_max = WebRtcSpl_MaxAbsValueW16(expanded_signal, mod_input_length);
  *input_max = WebRtcSpl_MaxAbsValueW16(input, mod_input_length);

  // Calculate energy of expanded signal.
  // |log_fs_mult| is log2(fs_mult_), but is not exact for 48000 Hz.
  int log_fs_mult = 30 - WebRtcSpl_NormW32(fs_mult_);
  int expanded_shift = 6 + log_fs_mult
      - WebRtcSpl_NormW32(*expanded_max * *expanded_max);
  expanded_shift = std::max(expanded_shift, 0);
  int32_t energy_expanded = WebRtcSpl_DotProductWithScale(expanded_signal,
                                                          expanded_signal,
                                                          mod_input_length,
                                                          expanded_shift);

  // Calculate energy of input signal.
  int input_shift = 6 + log_fs_mult -
      WebRtcSpl_NormW32(*input_max * *input_max);
  input_shift = std::max(input_shift, 0);
  int32_t energy_input = WebRtcSpl_DotProductWithScale(input, input,
                                                       mod_input_length,
                                                       input_shift);

  // Align to the same Q-domain.
  if (input_shift > expanded_shift) {
    energy_expanded = energy_expanded >> (input_shift - expanded_shift);
  } else {
    energy_input = energy_input >> (expanded_shift - input_shift);
  }

  // Calculate muting factor to use for new frame.
  int16_t mute_factor;
  if (energy_input > energy_expanded) {
    // Normalize |energy_input| to 14 bits.
    int16_t temp_shift = WebRtcSpl_NormW32(energy_input) - 17;
    energy_input = WEBRTC_SPL_SHIFT_W32(energy_input, temp_shift);
    // Put |energy_expanded| in a domain 14 higher, so that
    // energy_expanded / energy_input is in Q14.
    energy_expanded = WEBRTC_SPL_SHIFT_W32(energy_expanded, temp_shift + 14);
    // Calculate sqrt(energy_expanded / energy_input) in Q14.
    mute_factor = WebRtcSpl_SqrtFloor((energy_expanded / energy_input) << 14);
  } else {
    // Set to 1 (in Q14) when |expanded| has higher energy than |input|.
    mute_factor = 16384;
  }

  return mute_factor;
}

// TODO(hlundin): There are some parameter values in this method that seem
// strange. Compare with Expand::Correlation.
void Merge::Downsample(const int16_t* input, int input_length,
                       const int16_t* expanded_signal, int expanded_length) {
  const int16_t* filter_coefficients;
  int num_coefficients;
  int decimation_factor = fs_hz_ / 4000;
  static const int kCompensateDelay = 0;
  int length_limit = fs_hz_ / 100;
  if (fs_hz_ == 8000) {
    filter_coefficients = DspHelper::kDownsample8kHzTbl;
    num_coefficients = 3;
  } else if (fs_hz_ == 16000) {
    filter_coefficients = DspHelper::kDownsample16kHzTbl;
    num_coefficients = 5;
  } else if (fs_hz_ == 32000) {
    filter_coefficients = DspHelper::kDownsample32kHzTbl;
    num_coefficients = 7;
  } else {  // fs_hz_ == 48000
    filter_coefficients = DspHelper::kDownsample48kHzTbl;
    num_coefficients = 7;
    // TODO(hlundin) Why is |length_limit| not 480 (legacy)?
    length_limit = 320;
  }
  int signal_offset = num_coefficients - 1;
  WebRtcSpl_DownsampleFast(&expanded_signal[signal_offset],
                           expanded_length - signal_offset,
                           expanded_downsampled_, kExpandDownsampLength,
                           filter_coefficients, num_coefficients,
                           decimation_factor, kCompensateDelay);
  if (input_length <= length_limit) {
    // Not quite long enough, so we have to cheat a bit.
    int16_t temp_len = input_length - signal_offset;
    // TODO(hlundin): Should |downsamp_temp_len| be corrected for round-off
    // errors? I.e., (temp_len + decimation_factor - 1) / decimation_factor?
    int16_t downsamp_temp_len = temp_len / decimation_factor;
    WebRtcSpl_DownsampleFast(&input[signal_offset], temp_len,
                             input_downsampled_, downsamp_temp_len,
                             filter_coefficients, num_coefficients,
                             decimation_factor, kCompensateDelay);
    memset(&input_downsampled_[downsamp_temp_len], 0,
           sizeof(int16_t) * (kInputDownsampLength - downsamp_temp_len));
  } else {
    WebRtcSpl_DownsampleFast(&input[signal_offset],
                             input_length - signal_offset, input_downsampled_,
                             kInputDownsampLength, filter_coefficients,
                             num_coefficients, decimation_factor,
                             kCompensateDelay);
  }
}

int16_t Merge::CorrelateAndPeakSearch(int16_t expanded_max, int16_t input_max,
                                      int start_position, int input_length,
                                      int expand_period) const {
  // Calculate correlation without any normalization.
  const int max_corr_length = kMaxCorrelationLength;
  int stop_position_downsamp = std::min(
      max_corr_length, expand_->max_lag() / (fs_mult_ * 2) + 1);
  int16_t correlation_shift = 0;
  if (expanded_max * input_max > 26843546) {
    correlation_shift = 3;
  }

  int32_t correlation[kMaxCorrelationLength];
  WebRtcSpl_CrossCorrelation(correlation, input_downsampled_,
                             expanded_downsampled_, kInputDownsampLength,
                             stop_position_downsamp, correlation_shift, 1);

  // Normalize correlation to 14 bits and copy to a 16-bit array.
  static const int kPadLength = 4;
  int16_t correlation16[kPadLength + kMaxCorrelationLength + kPadLength] = {0};
  int16_t* correlation_ptr = &correlation16[kPadLength];
  int32_t max_correlation = WebRtcSpl_MaxAbsValueW32(correlation,
                                                     stop_position_downsamp);
  int16_t norm_shift = std::max(0, 17 - WebRtcSpl_NormW32(max_correlation));
  WebRtcSpl_VectorBitShiftW32ToW16(correlation_ptr, stop_position_downsamp,
                                   correlation, norm_shift);

  // Calculate allowed starting point for peak finding.
  // The peak location bestIndex must fulfill two criteria:
  // (1) w16_bestIndex + input_length <
  //     timestamps_per_call_ + expand_->overlap_length();
  // (2) w16_bestIndex + input_length < start_position.
  int start_index = timestamps_per_call_ + expand_->overlap_length();
  start_index = std::max(start_position, start_index);
  start_index = std::max(start_index - input_length, 0);
  // Downscale starting index to 4kHz domain. (fs_mult_ * 2 = fs_hz_ / 4000.)
  int start_index_downsamp = start_index / (fs_mult_ * 2);

  // Calculate a modified |stop_position_downsamp| to account for the increased
  // start index |start_index_downsamp| and the effective array length.
  int16_t modified_stop_pos =
      std::min(stop_position_downsamp,
               kMaxCorrelationLength + kPadLength - start_index_downsamp);
  int best_correlation_index;
  int16_t best_correlation;
  static const int kNumCorrelationCandidates = 1;
  DspHelper::PeakDetection(&correlation_ptr[start_index_downsamp],
                           modified_stop_pos, kNumCorrelationCandidates,
                           fs_mult_, &best_correlation_index,
                           &best_correlation);
  // Compensate for modified start index.
  best_correlation_index += start_index;

  // Ensure that underrun does not occur for 10ms case => we have to get at
  // least 10ms + overlap . (This should never happen thanks to the above
  // modification of peak-finding starting point.)
  while ((best_correlation_index + input_length) <
      static_cast<int>(timestamps_per_call_ + expand_->overlap_length()) ||
      best_correlation_index + input_length < start_position) {
    assert(false);  // Should never happen.
    best_correlation_index += expand_period;  // Jump one lag ahead.
  }
  return best_correlation_index;
}

}  // namespace webrtc
