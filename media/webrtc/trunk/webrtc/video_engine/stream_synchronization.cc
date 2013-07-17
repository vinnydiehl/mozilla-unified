/*
 *  Copyright (c) 2012 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "video_engine/stream_synchronization.h"

#include <assert.h>
#include <algorithm>
#include <cmath>
#include <cstdlib>

#include "system_wrappers/interface/trace.h"

namespace webrtc {

static const int kMaxChangeMs = 80;
static const int kMaxDeltaDelayMs = 10000;
static const int kFilterLength = 4;
// Minimum difference between audio and video to warrant a change.
static const int kMinDeltaMs = 30;

struct ViESyncDelay {
  ViESyncDelay() {
    extra_video_delay_ms = 0;
    last_video_delay_ms = 0;
    extra_audio_delay_ms = 0;
    network_delay = 120;
  }

  int extra_video_delay_ms;
  int last_video_delay_ms;
  int extra_audio_delay_ms;
  int network_delay;
};

StreamSynchronization::StreamSynchronization(int audio_channel_id,
                                             int video_channel_id)
    : channel_delay_(new ViESyncDelay),
      audio_channel_id_(audio_channel_id),
      video_channel_id_(video_channel_id),
      base_target_delay_ms_(0),
      avg_diff_ms_(0) {}

StreamSynchronization::~StreamSynchronization() {
  delete channel_delay_;
}

bool StreamSynchronization::ComputeRelativeDelay(
    const Measurements& audio_measurement,
    const Measurements& video_measurement,
    int* relative_delay_ms) {
  assert(relative_delay_ms);
  if (audio_measurement.rtcp.size() < 2 || video_measurement.rtcp.size() < 2) {
    // We need two RTCP SR reports per stream to do synchronization.
    return false;
  }
  int64_t audio_last_capture_time_ms;
  if (!synchronization::RtpToNtpMs(audio_measurement.latest_timestamp,
                                   audio_measurement.rtcp,
                                   &audio_last_capture_time_ms)) {
    return false;
  }
  int64_t video_last_capture_time_ms;
  if (!synchronization::RtpToNtpMs(video_measurement.latest_timestamp,
                                   video_measurement.rtcp,
                                   &video_last_capture_time_ms)) {
    return false;
  }
  if (video_last_capture_time_ms < 0) {
    return false;
  }
  // Positive diff means that video_measurement is behind audio_measurement.
  *relative_delay_ms = video_measurement.latest_receive_time_ms -
      audio_measurement.latest_receive_time_ms -
      (video_last_capture_time_ms - audio_last_capture_time_ms);
  if (*relative_delay_ms > kMaxDeltaDelayMs ||
      *relative_delay_ms < -kMaxDeltaDelayMs) {
    return false;
  }
  return true;
}

bool StreamSynchronization::ComputeDelays(int relative_delay_ms,
                                          int current_audio_delay_ms,
                                          int* extra_audio_delay_ms,
                                          int* total_video_delay_target_ms) {
  assert(extra_audio_delay_ms && total_video_delay_target_ms);

  int current_video_delay_ms = *total_video_delay_target_ms;
  WEBRTC_TRACE(webrtc::kTraceInfo, webrtc::kTraceVideo, video_channel_id_,
               "Audio delay is: %d for voice channel: %d",
               current_audio_delay_ms, audio_channel_id_);
  WEBRTC_TRACE(webrtc::kTraceInfo, webrtc::kTraceVideo, video_channel_id_,
               "Network delay diff is: %d for voice channel: %d",
               channel_delay_->network_delay, audio_channel_id_);
  // Calculate the difference between the lowest possible video delay and
  // the current audio delay.
  WEBRTC_TRACE(webrtc::kTraceInfo, webrtc::kTraceVideo, video_channel_id_,
               "Current diff is: %d for audio channel: %d",
               relative_delay_ms, audio_channel_id_);

  int current_diff_ms = current_video_delay_ms - current_audio_delay_ms +
      relative_delay_ms;

  avg_diff_ms_ = ((kFilterLength - 1) * avg_diff_ms_ +
      current_diff_ms) / kFilterLength;
  if (abs(avg_diff_ms_) < kMinDeltaMs) {
    // Don't adjust if the diff is within our margin.
    return false;
  }

  // Make sure we don't move too fast.
  int diff_ms = avg_diff_ms_ / 2;
  diff_ms = std::min(diff_ms, kMaxChangeMs);
  diff_ms = std::max(diff_ms, -kMaxChangeMs);

  // Reset the average after a move to prevent overshooting reaction.
  avg_diff_ms_ = 0;

  if (diff_ms > 0) {
    // The minimum video delay is longer than the current audio delay.
    // We need to decrease extra video delay, or add extra audio delay.
    if (channel_delay_->extra_video_delay_ms > base_target_delay_ms_) {
      // We have extra delay added to ViE. Reduce this delay before adding
      // extra delay to VoE.
      channel_delay_->extra_video_delay_ms -= diff_ms;
      channel_delay_->extra_audio_delay_ms = base_target_delay_ms_;
    } else {  // channel_delay_->extra_video_delay_ms > 0
      // We have no extra video delay to remove, increase the audio delay.
      channel_delay_->extra_audio_delay_ms += diff_ms;
      channel_delay_->extra_video_delay_ms = base_target_delay_ms_;
    }
  } else {  // if (diff_ms > 0)
    // The video delay is lower than the current audio delay.
    // We need to decrease extra audio delay, or add extra video delay.
    if (channel_delay_->extra_audio_delay_ms > base_target_delay_ms_) {
      // We have extra delay in VoiceEngine.
      // Start with decreasing the voice delay.
      // Note: diff_ms is negative; add the negative difference.
      channel_delay_->extra_audio_delay_ms += diff_ms;
      channel_delay_->extra_video_delay_ms = base_target_delay_ms_;
    } else {  // channel_delay_->extra_audio_delay_ms > base_target_delay_ms_
      // We have no extra delay in VoiceEngine, increase the video delay.
      // Note: diff_ms is negative; subtract the negative difference.
      channel_delay_->extra_video_delay_ms -= diff_ms;  // X - (-Y) = X + Y.
      channel_delay_->extra_audio_delay_ms = base_target_delay_ms_;
    }
  }

  // Make sure that video is never below our target.
  channel_delay_->extra_video_delay_ms = std::max(
      channel_delay_->extra_video_delay_ms, base_target_delay_ms_);

  int new_video_delay_ms;
  if (channel_delay_->extra_video_delay_ms > base_target_delay_ms_) {
    new_video_delay_ms = channel_delay_->extra_video_delay_ms;
  } else {
    // No change to the extra video delay. We are changing audio and we only
    // allow to change one at the time.
    new_video_delay_ms = channel_delay_->last_video_delay_ms;
  }

  // Make sure that we don't go below the extra video delay.
  new_video_delay_ms = std::max(
      new_video_delay_ms, channel_delay_->extra_video_delay_ms);

  // Verify we don't go above the maximum allowed video delay.
  new_video_delay_ms =
      std::min(new_video_delay_ms, base_target_delay_ms_ + kMaxDeltaDelayMs);

  // Make sure that audio is never below our target.
  channel_delay_->extra_audio_delay_ms =
      std::max(base_target_delay_ms_, channel_delay_->extra_audio_delay_ms);

  // Verify we don't go above the maximum allowed audio delay.
  channel_delay_->extra_audio_delay_ms = std::min(
      channel_delay_->extra_audio_delay_ms,
      base_target_delay_ms_ + kMaxDeltaDelayMs);

  // Remember our last video delay.
  channel_delay_->last_video_delay_ms = new_video_delay_ms;

  WEBRTC_TRACE(webrtc::kTraceInfo, webrtc::kTraceVideo, video_channel_id_,
      "Sync video delay %d ms for video channel and audio delay %d for audio "
      "channel %d",
      new_video_delay_ms, channel_delay_->extra_audio_delay_ms,
      audio_channel_id_);

  // Return values.
  *extra_audio_delay_ms = channel_delay_->extra_audio_delay_ms;
  *total_video_delay_target_ms = new_video_delay_ms;
  return true;
}

void StreamSynchronization::SetTargetBufferingDelay(int target_delay_ms) {
  // Initial extra delay for audio (accounting for existing extra delay).
  channel_delay_->extra_audio_delay_ms +=
      target_delay_ms - base_target_delay_ms_;

  // The video delay is compared to the last value (and how much we can update
  // is limited by that as well).
  channel_delay_->last_video_delay_ms +=
      target_delay_ms - base_target_delay_ms_;

  channel_delay_->extra_video_delay_ms +=
      target_delay_ms - base_target_delay_ms_;

  // Video is already delayed by the desired amount.
  base_target_delay_ms_ = target_delay_ms;
}

}  // namespace webrtc
