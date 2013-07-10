/*
 *  Copyright (c) 2012 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

// This file contains a class that can write audio and/or video to file in
// multiple file formats. The unencoded input data is written to file in the
// encoded format specified.

#ifndef WEBRTC_MODULES_UTILITY_SOURCE_FILE_RECORDER_IMPL_H_
#define WEBRTC_MODULES_UTILITY_SOURCE_FILE_RECORDER_IMPL_H_

#include "coder.h"
#include "common_types.h"
#include "engine_configurations.h"
#include "event_wrapper.h"
#include "file_recorder.h"
#include "media_file_defines.h"
#include "media_file.h"
#include "module_common_types.h"
#include "resampler.h"
#include "thread_wrapper.h"
#include "tick_util.h"
#include "typedefs.h"

#ifdef WEBRTC_MODULE_UTILITY_VIDEO
    #include "frame_scaler.h"
    #include "video_coder.h"
    #include "video_frames_queue.h"
#endif

namespace webrtc {
// The largest decoded frame size in samples (60ms with 32kHz sample rate).
enum { MAX_AUDIO_BUFFER_IN_SAMPLES = 60*32};
enum { MAX_AUDIO_BUFFER_IN_BYTES = MAX_AUDIO_BUFFER_IN_SAMPLES*2};
enum { kMaxAudioBufferQueueLength = 100 };

class FileRecorderImpl : public FileRecorder
{
public:
    FileRecorderImpl(uint32_t instanceID, FileFormats fileFormat);
    virtual ~FileRecorderImpl();

    // FileRecorder functions.
    virtual int32_t RegisterModuleFileCallback(FileCallback* callback);
    virtual FileFormats RecordingFileFormat() const;
    virtual int32_t StartRecordingAudioFile(
        const char* fileName,
        const CodecInst& codecInst,
        uint32_t notificationTimeMs,
        ACMAMRPackingFormat amrFormat = AMRFileStorage);
    virtual int32_t StartRecordingAudioFile(
        OutStream& destStream,
        const CodecInst& codecInst,
        uint32_t notificationTimeMs,
        ACMAMRPackingFormat amrFormat = AMRFileStorage);
    virtual int32_t StopRecording();
    virtual bool IsRecording() const;
    virtual int32_t codec_info(CodecInst& codecInst) const;
    virtual int32_t RecordAudioToFile(
        const AudioFrame& frame,
        const TickTime* playoutTS = NULL);
    virtual int32_t StartRecordingVideoFile(
        const char* fileName,
        const CodecInst& audioCodecInst,
        const VideoCodec& videoCodecInst,
        ACMAMRPackingFormat amrFormat = AMRFileStorage,
        bool videoOnly = false)
    {
        return -1;
    }
    virtual int32_t RecordVideoToFile(const I420VideoFrame& videoFrame)
    {
        return -1;
    }

protected:
    virtual int32_t WriteEncodedAudioData(
        const int8_t* audioBuffer,
        uint16_t bufferLength,
        uint16_t millisecondsOfData,
        const TickTime* playoutTS);

    int32_t SetUpAudioEncoder();

    uint32_t _instanceID;
    FileFormats _fileFormat;
    MediaFile* _moduleFile;

private:
    CodecInst codec_info_;
    ACMAMRPackingFormat _amrFormat;

    int8_t _audioBuffer[MAX_AUDIO_BUFFER_IN_BYTES];
    AudioCoder _audioEncoder;
    Resampler _audioResampler;
};


#ifdef WEBRTC_MODULE_UTILITY_VIDEO
class AviRecorder : public FileRecorderImpl
{
public:
    AviRecorder(uint32_t instanceID, FileFormats fileFormat);
    virtual ~AviRecorder();

    // FileRecorder functions.
    virtual int32_t StartRecordingVideoFile(
        const char* fileName,
        const CodecInst& audioCodecInst,
        const VideoCodec& videoCodecInst,
        ACMAMRPackingFormat amrFormat = AMRFileStorage,
        bool videoOnly = false);
    virtual int32_t StopRecording();
    virtual int32_t RecordVideoToFile(const I420VideoFrame& videoFrame);

protected:
    virtual int32_t WriteEncodedAudioData(
        const int8_t*  audioBuffer,
        uint16_t bufferLength,
        uint16_t millisecondsOfData,
        const TickTime* playoutTS);
private:
    static bool Run(ThreadObj threadObj);
    bool Process();

    bool StartThread();
    bool StopThread();

    int32_t EncodeAndWriteVideoToFile(I420VideoFrame& videoFrame);
    int32_t ProcessAudio();

    int32_t CalcI420FrameSize() const;
    int32_t SetUpVideoEncoder();

    VideoCodec _videoCodecInst;
    bool _videoOnly;

    ListWrapper _audioFramesToWrite;
    bool _firstAudioFrameReceived;

    VideoFramesQueue* _videoFramesQueue;

    FrameScaler* _frameScaler;
    VideoCoder* _videoEncoder;
    int32_t _videoMaxPayloadSize;
    EncodedVideoData _videoEncodedData;

    ThreadWrapper* _thread;
    EventWrapper& _timeEvent;
    CriticalSectionWrapper* _critSec;
    int64_t _writtenVideoFramesCounter;
    int64_t _writtenAudioMS;
    int64_t _writtenVideoMS;
};
#endif // WEBRTC_MODULE_UTILITY_VIDEO
} // namespace webrtc
#endif // WEBRTC_MODULES_UTILITY_SOURCE_FILE_RECORDER_IMPL_H_
