/*
 *  Copyright (c) 2012 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include <stdio.h>

#include "gtest/gtest.h"
#include "modules/utility/interface/process_thread.h"
#include "webrtc/modules/video_capture/include/video_capture.h"
#include "webrtc/modules/video_capture/include/video_capture_factory.h"
#include "common_video/interface/i420_video_frame.h"
#include "common_video/libyuv/include/webrtc_libyuv.h"
#include "system_wrappers/interface/critical_section_wrapper.h"
#include "system_wrappers/interface/scoped_ptr.h"
#include "system_wrappers/interface/scoped_refptr.h"
#include "system_wrappers/interface/sleep.h"
#include "system_wrappers/interface/tick_util.h"

using webrtc::CriticalSectionWrapper;
using webrtc::CriticalSectionScoped;
using webrtc::scoped_ptr;
using webrtc::SleepMs;
using webrtc::TickTime;
using webrtc::VideoCaptureAlarm;
using webrtc::VideoCaptureCapability;
using webrtc::VideoCaptureDataCallback;
using webrtc::VideoCaptureFactory;
using webrtc::VideoCaptureFeedBack;
using webrtc::VideoCaptureModule;


#define WAIT_(ex, timeout, res) \
  do { \
    res = (ex); \
    int64_t start = TickTime::MillisecondTimestamp(); \
    while (!res && TickTime::MillisecondTimestamp() < start + timeout) { \
      SleepMs(5); \
      res = (ex); \
    } \
  } while (0);\

#define EXPECT_TRUE_WAIT(ex, timeout) \
  do { \
    bool res; \
    WAIT_(ex, timeout, res); \
    if (!res) EXPECT_TRUE(ex); \
  } while (0);


static const int kTimeOut = 5000;
static const int kTestHeight = 288;
static const int kTestWidth = 352;
static const int kTestFramerate = 30;

// Compares the content of two video frames.
static bool CompareFrames(const webrtc::I420VideoFrame& frame1,
                          const webrtc::I420VideoFrame& frame2) {
  bool result =
      (frame1.stride(webrtc::kYPlane) == frame2.stride(webrtc::kYPlane)) &&
      (frame1.stride(webrtc::kUPlane) == frame2.stride(webrtc::kUPlane)) &&
      (frame1.stride(webrtc::kVPlane) == frame2.stride(webrtc::kVPlane)) &&
      (frame1.width() == frame2.width()) &&
      (frame1.height() == frame2.height());

  if (!result)
    return false;
  for (int plane = 0; plane < webrtc::kNumOfPlanes; plane ++) {
      webrtc::PlaneType plane_type = static_cast<webrtc::PlaneType>(plane);
      int allocated_size1 = frame1.allocated_size(plane_type);
      int allocated_size2 = frame2.allocated_size(plane_type);
      if (allocated_size1 != allocated_size2)
        return false;
      const uint8_t* plane_buffer1 = frame1.buffer(plane_type);
      const uint8_t* plane_buffer2 = frame2.buffer(plane_type);
      if (memcmp(plane_buffer1, plane_buffer2, allocated_size1))
        return false;
    }
    return true;
}

// Compares the content of a I420 frame in planar form and the new video frame.
static bool CompareFrames(const webrtc::VideoFrameI420& frame1,
                          const webrtc::I420VideoFrame& frame2) {
  if (frame1.width != frame2.width() ||
      frame1.height != frame2.height()) {
      return false;
  }

  // Compare Y
  const unsigned char* y_plane = frame1.y_plane;
  const unsigned char* y_plane2 = frame2.buffer(webrtc::kYPlane);
  for (int i = 0; i < frame2.height(); ++i) {
    for (int j = 0; j < frame2.width(); ++j) {
      if (*y_plane != *y_plane2)
        return false;
      ++y_plane;
      ++y_plane2;
    }
    y_plane += frame1.y_pitch - frame1.width;
    y_plane2 += frame2.stride(webrtc::kYPlane) - frame2.width();
  }

  // Compare U
  const unsigned char* u_plane = frame1.u_plane;
  const unsigned char* u_plane2 = frame2.buffer(webrtc::kUPlane);
  for (int i = 0; i < (frame2.height() + 1) / 2; ++i) {
    for (int j = 0; j < (frame2.width() + 1) / 2; ++j) {
      if (*u_plane != *u_plane2)
        return false;
      ++u_plane;
      ++u_plane2;
    }
    u_plane += frame1.u_pitch - (frame1.width + 1) / 2;
    u_plane2+= frame2.stride(webrtc::kUPlane) - (frame2.width() + 1) / 2;
  }

  // Compare V
  unsigned char* v_plane = frame1.v_plane;
  const unsigned char* v_plane2 = frame2.buffer(webrtc::kVPlane);
  for (int i = 0; i < frame2.height() /2; ++i) {
    for (int j = 0; j < frame2.width() /2; ++j) {
      if (*u_plane != *u_plane2) {
        return false;
      }
      ++v_plane;
      ++v_plane2;
    }
    v_plane += frame1.v_pitch - (frame1.width + 1) / 2;
    u_plane2+= frame2.stride(webrtc::kVPlane) - (frame2.width() + 1) / 2;
  }
  return true;
}


class TestVideoCaptureCallback : public VideoCaptureDataCallback {
 public:
  TestVideoCaptureCallback()
    : capture_cs_(CriticalSectionWrapper::CreateCriticalSection()),
      capture_delay_(0),
      last_render_time_ms_(0),
      incoming_frames_(0),
      timing_warnings_(0),
      rotate_frame_(webrtc::kCameraRotate0){
  }

  ~TestVideoCaptureCallback() {
    if (timing_warnings_ > 0)
      printf("No of timing warnings %d\n", timing_warnings_);
  }

  virtual void OnIncomingCapturedFrame(const int32_t id,
                                       webrtc::I420VideoFrame& videoFrame) {
    CriticalSectionScoped cs(capture_cs_.get());

    int height = videoFrame.height();
    int width = videoFrame.width();
    if (rotate_frame_ == webrtc::kCameraRotate90 ||
        rotate_frame_ == webrtc::kCameraRotate270) {
      EXPECT_EQ(width, capability_.height);
      EXPECT_EQ(height, capability_.width);
    } else {
      EXPECT_EQ(height, capability_.height);
      EXPECT_EQ(width, capability_.width);
    }
    // RenderTimstamp should be the time now.
    EXPECT_TRUE(
        videoFrame.render_time_ms() >= TickTime::MillisecondTimestamp()-30 &&
        videoFrame.render_time_ms() <= TickTime::MillisecondTimestamp());

    if ((videoFrame.render_time_ms() >
            last_render_time_ms_ + (1000 * 1.1) / capability_.maxFPS &&
            last_render_time_ms_ > 0) ||
        (videoFrame.render_time_ms() <
            last_render_time_ms_ + (1000 * 0.9) / capability_.maxFPS &&
            last_render_time_ms_ > 0)) {
      timing_warnings_++;
    }

    incoming_frames_++;
    last_render_time_ms_ = videoFrame.render_time_ms();
    last_frame_.CopyFrame(videoFrame);
  }
  virtual void OnIncomingCapturedEncodedFrame(const int32_t id,
                                              webrtc::VideoFrame& videoFrame,
                                              webrtc::VideoCodecType codecType)
 {
     assert(false);
 }

  virtual void OnCaptureDelayChanged(const int32_t id,
                                     const int32_t delay) {
    CriticalSectionScoped cs(capture_cs_.get());
    capture_delay_ = delay;
  }

  void SetExpectedCapability(VideoCaptureCapability capability) {
    CriticalSectionScoped cs(capture_cs_.get());
    capability_= capability;
    incoming_frames_ = 0;
    last_render_time_ms_ = 0;
    capture_delay_ = 0;
  }
  int incoming_frames() {
    CriticalSectionScoped cs(capture_cs_.get());
    return incoming_frames_;
  }

  int capture_delay() {
    CriticalSectionScoped cs(capture_cs_.get());
    return capture_delay_;
  }
  int timing_warnings() {
    CriticalSectionScoped cs(capture_cs_.get());
    return timing_warnings_;
  }
  VideoCaptureCapability capability() {
    CriticalSectionScoped cs(capture_cs_.get());
    return capability_;
  }

  bool CompareLastFrame(const webrtc::I420VideoFrame& frame) {
    CriticalSectionScoped cs(capture_cs_.get());
    return CompareFrames(last_frame_, frame);
  }

  bool CompareLastFrame(const webrtc::VideoFrameI420& frame) {
    CriticalSectionScoped cs(capture_cs_.get());
    return CompareFrames(frame, last_frame_);
  }

  void SetExpectedCaptureRotation(webrtc::VideoCaptureRotation rotation) {
    CriticalSectionScoped cs(capture_cs_.get());
    rotate_frame_ = rotation;
  }

 private:
  scoped_ptr<CriticalSectionWrapper> capture_cs_;
  VideoCaptureCapability capability_;
  int capture_delay_;
  int64_t last_render_time_ms_;
  int incoming_frames_;
  int timing_warnings_;
  webrtc::I420VideoFrame last_frame_;
  webrtc::VideoCaptureRotation rotate_frame_;
};

class TestVideoCaptureFeedBack : public VideoCaptureFeedBack {
 public:
  TestVideoCaptureFeedBack() :
    capture_cs_(CriticalSectionWrapper::CreateCriticalSection()),
    frame_rate_(0),
    alarm_(webrtc::Cleared) {
  }

  virtual void OnCaptureFrameRate(const int32_t id,
                                  const uint32_t frameRate) {
    CriticalSectionScoped cs(capture_cs_.get());
    frame_rate_ = frameRate;
  }

  virtual void OnNoPictureAlarm(const int32_t id,
                                const VideoCaptureAlarm reported_alarm) {
    CriticalSectionScoped cs(capture_cs_.get());
    alarm_ = reported_alarm;
  }
  int frame_rate() {
    CriticalSectionScoped cs(capture_cs_.get());
    return frame_rate_;

  }
  VideoCaptureAlarm alarm() {
    CriticalSectionScoped cs(capture_cs_.get());
    return alarm_;
  }

 private:
  scoped_ptr<CriticalSectionWrapper> capture_cs_;
  unsigned int frame_rate_;
  VideoCaptureAlarm alarm_;
};

class VideoCaptureTest : public testing::Test {
 public:
  VideoCaptureTest() : number_of_devices_(0) {}

  void SetUp() {
    device_info_.reset(VideoCaptureFactory::CreateDeviceInfo(5));
    number_of_devices_ = device_info_->NumberOfDevices();
    ASSERT_GT(number_of_devices_, 0u);
  }

  webrtc::scoped_refptr<VideoCaptureModule> OpenVideoCaptureDevice(
      unsigned int device,
      VideoCaptureDataCallback* callback) {
    char device_name[256];
    char unique_name[256];

    EXPECT_EQ(0, device_info_->GetDeviceName(
        device, device_name, 256, unique_name, 256));

    webrtc::scoped_refptr<VideoCaptureModule> module(
        VideoCaptureFactory::Create(device, unique_name));
    if (module.get() == NULL)
      return NULL;

    EXPECT_FALSE(module->CaptureStarted());

    EXPECT_EQ(0, module->RegisterCaptureDataCallback(*callback));
    return module;
  }

  void StartCapture(VideoCaptureModule* capture_module,
                    VideoCaptureCapability capability) {
    EXPECT_EQ(0, capture_module->StartCapture(capability));
    EXPECT_TRUE(capture_module->CaptureStarted());

    VideoCaptureCapability resulting_capability;
    EXPECT_EQ(0, capture_module->CaptureSettings(resulting_capability));
    EXPECT_EQ(capability.width, resulting_capability.width);
    EXPECT_EQ(capability.height, resulting_capability.height);
  }

  scoped_ptr<VideoCaptureModule::DeviceInfo> device_info_;
  unsigned int number_of_devices_;
};

TEST_F(VideoCaptureTest, CreateDelete) {
  for (int i = 0; i < 5; ++i) {
    int64_t start_time = TickTime::MillisecondTimestamp();
    TestVideoCaptureCallback capture_observer;
    webrtc::scoped_refptr<VideoCaptureModule> module(OpenVideoCaptureDevice(
        0, &capture_observer));
    ASSERT_TRUE(module.get() != NULL);

    VideoCaptureCapability capability;
#ifndef WEBRTC_MAC
    device_info_->GetCapability(module->CurrentDeviceName(), 0, capability);
#else
    capability.width = kTestWidth;
    capability.height = kTestHeight;
    capability.maxFPS = kTestFramerate;
    capability.rawType = webrtc::kVideoUnknown;
#endif
    capture_observer.SetExpectedCapability(capability);
    StartCapture(module.get(), capability);

    // Less than 4s to start the camera.
    EXPECT_LE(TickTime::MillisecondTimestamp() - start_time, 4000);

    // Make sure 5 frames are captured.
    EXPECT_TRUE_WAIT(capture_observer.incoming_frames() >= 5, kTimeOut);

    EXPECT_GT(capture_observer.capture_delay(), 0);

    int64_t stop_time = TickTime::MillisecondTimestamp();
    EXPECT_EQ(0, module->StopCapture());
    EXPECT_FALSE(module->CaptureStarted());

    // Less than 3s to stop the camera.
    EXPECT_LE(TickTime::MillisecondTimestamp() - stop_time, 3000);
  }
}

TEST_F(VideoCaptureTest, Capabilities) {
#ifdef WEBRTC_MAC
  printf("Video capture capabilities are not supported on Mac.\n");
  return;
#endif

  TestVideoCaptureCallback capture_observer;

  webrtc::scoped_refptr<VideoCaptureModule> module(OpenVideoCaptureDevice(
          0, &capture_observer));
  ASSERT_TRUE(module.get() != NULL);

  int number_of_capabilities = device_info_->NumberOfCapabilities(
      module->CurrentDeviceName());
  EXPECT_GT(number_of_capabilities, 0);
  for (int i = 0; i < number_of_capabilities; ++i) {
    VideoCaptureCapability capability;
    EXPECT_EQ(0, device_info_->GetCapability(module->CurrentDeviceName(), i,
                                             capability));
    capture_observer.SetExpectedCapability(capability);
    StartCapture(module.get(), capability);
    // Make sure 5 frames are captured.
    EXPECT_TRUE_WAIT(capture_observer.incoming_frames() >= 5, kTimeOut);

    EXPECT_EQ(0, module->StopCapture());
  }
}

// NOTE: flaky, crashes sometimes.
// http://code.google.com/p/webrtc/issues/detail?id=777
TEST_F(VideoCaptureTest, DISABLED_TestTwoCameras) {
  if (number_of_devices_ < 2) {
    printf("There are not two cameras available. Aborting test. \n");
    return;
  }

  TestVideoCaptureCallback capture_observer1;
  webrtc::scoped_refptr<VideoCaptureModule> module1(OpenVideoCaptureDevice(
          0, &capture_observer1));
  ASSERT_TRUE(module1.get() != NULL);
  VideoCaptureCapability capability1;
#ifndef WEBRTC_MAC
  device_info_->GetCapability(module1->CurrentDeviceName(), 0, capability1);
#else
  capability1.width = kTestWidth;
  capability1.height = kTestHeight;
  capability1.maxFPS = kTestFramerate;
  capability1.rawType = webrtc::kVideoUnknown;
#endif
  capture_observer1.SetExpectedCapability(capability1);

  TestVideoCaptureCallback capture_observer2;
  webrtc::scoped_refptr<VideoCaptureModule> module2(OpenVideoCaptureDevice(
          1, &capture_observer2));
  ASSERT_TRUE(module1.get() != NULL);


  VideoCaptureCapability capability2;
#ifndef WEBRTC_MAC
  device_info_->GetCapability(module2->CurrentDeviceName(), 0, capability2);
#else
  capability2.width = kTestWidth;
  capability2.height = kTestHeight;
  capability2.maxFPS = kTestFramerate;
  capability2.rawType = webrtc::kVideoUnknown;
#endif
  capture_observer2.SetExpectedCapability(capability2);

  StartCapture(module1.get(), capability1);
  StartCapture(module2.get(), capability2);
  EXPECT_TRUE_WAIT(capture_observer1.incoming_frames() >= 5, kTimeOut);
  EXPECT_TRUE_WAIT(capture_observer2.incoming_frames() >= 5, kTimeOut);
}

// Test class for testing external capture and capture feedback information
// such as frame rate and picture alarm.
class VideoCaptureExternalTest : public testing::Test {
 public:
  void SetUp() {
    capture_module_ = VideoCaptureFactory::Create(0, capture_input_interface_);
    process_module_ = webrtc::ProcessThread::CreateProcessThread();
    process_module_->Start();
    process_module_->RegisterModule(capture_module_);

    VideoCaptureCapability capability;
    capability.width = kTestWidth;
    capability.height = kTestHeight;
    capability.rawType = webrtc::kVideoYV12;
    capability.maxFPS = kTestFramerate;
    capture_callback_.SetExpectedCapability(capability);

    test_frame_.CreateEmptyFrame(kTestWidth, kTestHeight, kTestWidth,
                                 ((kTestWidth + 1) / 2), (kTestWidth + 1) / 2);
    SleepMs(1); // Wait 1ms so that two tests can't have the same timestamp.
    memset(test_frame_.buffer(webrtc::kYPlane), 127, kTestWidth * kTestHeight);
    memset(test_frame_.buffer(webrtc::kUPlane), 127,
           ((kTestWidth + 1) / 2) * ((kTestHeight + 1) / 2));
    memset(test_frame_.buffer(webrtc::kVPlane), 127,
           ((kTestWidth + 1) / 2) * ((kTestHeight + 1) / 2));

    EXPECT_EQ(0, capture_module_->RegisterCaptureDataCallback(
        capture_callback_));
    EXPECT_EQ(0, capture_module_->RegisterCaptureCallback(capture_feedback_));
    EXPECT_EQ(0, capture_module_->EnableFrameRateCallback(true));
    EXPECT_EQ(0, capture_module_->EnableNoPictureAlarm(true));
  }

  void TearDown() {
    process_module_->Stop();
    webrtc::ProcessThread::DestroyProcessThread(process_module_);
  }

  webrtc::VideoCaptureExternal* capture_input_interface_;
  webrtc::scoped_refptr<VideoCaptureModule> capture_module_;
  webrtc::ProcessThread* process_module_;
  webrtc::I420VideoFrame test_frame_;
  TestVideoCaptureCallback capture_callback_;
  TestVideoCaptureFeedBack capture_feedback_;
};

// Test input of external video frames.
TEST_F(VideoCaptureExternalTest, TestExternalCapture) {
  unsigned int length = webrtc::CalcBufferSize(webrtc::kI420,
                                               test_frame_.width(),
                                               test_frame_.height());
  webrtc::scoped_array<uint8_t> test_buffer(new uint8_t[length]);
  webrtc::ExtractBuffer(test_frame_, length, test_buffer.get());
  EXPECT_EQ(0, capture_input_interface_->IncomingFrame(test_buffer.get(),
      length, capture_callback_.capability(), 0));
  EXPECT_TRUE(capture_callback_.CompareLastFrame(test_frame_));
}

// Test input of planar I420 frames.
// NOTE: flaky, sometimes fails on the last CompareLastFrame.
// http://code.google.com/p/webrtc/issues/detail?id=777
TEST_F(VideoCaptureExternalTest, DISABLED_TestExternalCaptureI420) {
  webrtc::VideoFrameI420 frame_i420;
  frame_i420.width = kTestWidth;
  frame_i420.height = kTestHeight;
  frame_i420.y_plane = test_frame_.buffer(webrtc::kYPlane);
  frame_i420.u_plane = frame_i420.y_plane + (kTestWidth * kTestHeight);
  frame_i420.v_plane = frame_i420.u_plane + ((kTestWidth * kTestHeight) >> 2);
  frame_i420.y_pitch = kTestWidth;
  frame_i420.u_pitch = kTestWidth / 2;
  frame_i420.v_pitch = kTestWidth / 2;
  EXPECT_EQ(0, capture_input_interface_->IncomingFrameI420(frame_i420, 0));
  EXPECT_TRUE(capture_callback_.CompareLastFrame(frame_i420));

  // Test with a frame with pitch not equal to width
  memset(test_frame_.buffer(webrtc::kYPlane), 0xAA,
         test_frame_.allocated_size(webrtc::kYPlane));
  memset(test_frame_.buffer(webrtc::kUPlane), 0xAA,
         test_frame_.allocated_size(webrtc::kUPlane));
  memset(test_frame_.buffer(webrtc::kVPlane), 0xAA,
         test_frame_.allocated_size(webrtc::kVPlane));
  webrtc::I420VideoFrame aligned_test_frame;
  int y_pitch = kTestWidth + 2;
  int u_pitch = kTestWidth / 2 + 1;
  int v_pitch = u_pitch;
  aligned_test_frame.CreateEmptyFrame(kTestWidth, kTestHeight,
                                      y_pitch, u_pitch, v_pitch);
  memset(aligned_test_frame.buffer(webrtc::kYPlane), 0,
         kTestWidth * kTestHeight);
  memset(aligned_test_frame.buffer(webrtc::kUPlane), 0,
         (kTestWidth + 1) / 2  * (kTestHeight + 1) / 2);
  memset(aligned_test_frame.buffer(webrtc::kVPlane), 0,
         (kTestWidth + 1) / 2  * (kTestHeight + 1) / 2);
  // Copy the test_frame_ to aligned_test_frame.
  int y_width = kTestWidth;
  int uv_width = kTestWidth / 2;
  int y_rows = kTestHeight;
  int uv_rows = kTestHeight / 2;
  unsigned char* y_plane = test_frame_.buffer(webrtc::kYPlane);
  unsigned char* u_plane = test_frame_.buffer(webrtc::kUPlane);
  unsigned char* v_plane = test_frame_.buffer(webrtc::kVPlane);
  // Copy Y
  unsigned char* current_pointer = aligned_test_frame.buffer(webrtc::kYPlane);
  for (int i = 0; i < y_rows; ++i) {
    memcpy(current_pointer, y_plane, y_width);
    // Remove the alignment which ViE doesn't support.
    current_pointer += y_pitch;
    y_plane += y_width;
  }
  // Copy U
  current_pointer = aligned_test_frame.buffer(webrtc::kUPlane);
  for (int i = 0; i < uv_rows; ++i) {
    memcpy(current_pointer, u_plane, uv_width);
    // Remove the alignment which ViE doesn't support.
    current_pointer += u_pitch;
    u_plane += uv_width;
  }
  // Copy V
  current_pointer = aligned_test_frame.buffer(webrtc::kVPlane);
  for (int i = 0; i < uv_rows; ++i) {
    memcpy(current_pointer, v_plane, uv_width);
    // Remove the alignment which ViE doesn't support.
    current_pointer += v_pitch;
    v_plane += uv_width;
  }
  frame_i420.width = kTestWidth;
  frame_i420.height = kTestHeight;
  frame_i420.y_plane = aligned_test_frame.buffer(webrtc::kYPlane);
  frame_i420.u_plane = aligned_test_frame.buffer(webrtc::kYPlane);
  frame_i420.v_plane = aligned_test_frame.buffer(webrtc::kVPlane);
  frame_i420.y_pitch = y_pitch;
  frame_i420.u_pitch = u_pitch;
  frame_i420.v_pitch = v_pitch;

  EXPECT_EQ(0, capture_input_interface_->IncomingFrameI420(frame_i420, 0));
  EXPECT_TRUE(capture_callback_.CompareLastFrame(test_frame_));
}

// Test frame rate and no picture alarm.
TEST_F(VideoCaptureExternalTest , FrameRate) {
  int64_t testTime = 3;
  TickTime startTime = TickTime::Now();

  while ((TickTime::Now() - startTime).Milliseconds() < testTime * 1000) {
     unsigned int length = webrtc::CalcBufferSize(webrtc::kI420,
                                                 test_frame_.width(),
                                                 test_frame_.height());
     webrtc::scoped_array<uint8_t> test_buffer(new uint8_t[length]);
     webrtc::ExtractBuffer(test_frame_, length, test_buffer.get());
     EXPECT_EQ(0, capture_input_interface_->IncomingFrame(test_buffer.get(),
       length, capture_callback_.capability(), 0));
    SleepMs(100);
  }
  EXPECT_TRUE(capture_feedback_.frame_rate() >= 8 &&
              capture_feedback_.frame_rate() <= 10);
  SleepMs(500);
  EXPECT_EQ(webrtc::Raised, capture_feedback_.alarm());

  startTime = TickTime::Now();
  while ((TickTime::Now() - startTime).Milliseconds() < testTime * 1000) {
    unsigned int length = webrtc::CalcBufferSize(webrtc::kI420,
                                                 test_frame_.width(),
                                                 test_frame_.height());
    webrtc::scoped_array<uint8_t> test_buffer(new uint8_t[length]);
    webrtc::ExtractBuffer(test_frame_, length, test_buffer.get());
    EXPECT_EQ(0, capture_input_interface_->IncomingFrame(test_buffer.get(),
      length, capture_callback_.capability(), 0));
    SleepMs(1000 / 30);
  }
  EXPECT_EQ(webrtc::Cleared, capture_feedback_.alarm());
  // Frame rate might be less than 33 since we have paused providing
  // frames for a while.
  EXPECT_TRUE(capture_feedback_.frame_rate() >= 25 &&
              capture_feedback_.frame_rate() <= 33);
}

TEST_F(VideoCaptureExternalTest, Rotation) {
  EXPECT_EQ(0, capture_module_->SetCaptureRotation(webrtc::kCameraRotate0));
  unsigned int length = webrtc::CalcBufferSize(webrtc::kI420,
                                               test_frame_.width(),
                                               test_frame_.height());
  webrtc::scoped_array<uint8_t> test_buffer(new uint8_t[length]);
  webrtc::ExtractBuffer(test_frame_, length, test_buffer.get());
  EXPECT_EQ(0, capture_input_interface_->IncomingFrame(test_buffer.get(),
    length, capture_callback_.capability(), 0));
  EXPECT_EQ(0, capture_module_->SetCaptureRotation(webrtc::kCameraRotate90));
  capture_callback_.SetExpectedCaptureRotation(webrtc::kCameraRotate90);
  EXPECT_EQ(0, capture_input_interface_->IncomingFrame(test_buffer.get(),
    length, capture_callback_.capability(), 0));
  EXPECT_EQ(0, capture_module_->SetCaptureRotation(webrtc::kCameraRotate180));
  capture_callback_.SetExpectedCaptureRotation(webrtc::kCameraRotate180);
  EXPECT_EQ(0, capture_input_interface_->IncomingFrame(test_buffer.get(),
    length, capture_callback_.capability(), 0));
  EXPECT_EQ(0, capture_module_->SetCaptureRotation(webrtc::kCameraRotate270));
  capture_callback_.SetExpectedCaptureRotation(webrtc::kCameraRotate270);
  EXPECT_EQ(0, capture_input_interface_->IncomingFrame(test_buffer.get(),
    length, capture_callback_.capability(), 0));
}

