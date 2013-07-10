/*
 *  Copyright (c) 2011 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef WEBRTC_AUDIO_DEVICE_AUDIO_MIXER_MANAGER_ALSA_LINUX_H
#define WEBRTC_AUDIO_DEVICE_AUDIO_MIXER_MANAGER_ALSA_LINUX_H

#include "typedefs.h"
#include "audio_device.h"
#include "critical_section_wrapper.h"
#include "alsasymboltable_linux.h"

#include <alsa/asoundlib.h>

namespace webrtc
{

class AudioMixerManagerLinuxALSA
{
public:
    int32_t OpenSpeaker(char* deviceName);
    int32_t OpenMicrophone(char* deviceName);
    int32_t SetSpeakerVolume(uint32_t volume);
    int32_t SpeakerVolume(uint32_t& volume) const;
    int32_t MaxSpeakerVolume(uint32_t& maxVolume) const;
    int32_t MinSpeakerVolume(uint32_t& minVolume) const;
    int32_t SpeakerVolumeStepSize(uint16_t& stepSize) const;
    int32_t SpeakerVolumeIsAvailable(bool& available);
    int32_t SpeakerMuteIsAvailable(bool& available);
    int32_t SetSpeakerMute(bool enable);
    int32_t SpeakerMute(bool& enabled) const;
    int32_t MicrophoneMuteIsAvailable(bool& available);
    int32_t SetMicrophoneMute(bool enable);
    int32_t MicrophoneMute(bool& enabled) const;
    int32_t MicrophoneBoostIsAvailable(bool& available);
    int32_t SetMicrophoneBoost(bool enable);
    int32_t MicrophoneBoost(bool& enabled) const;
    int32_t MicrophoneVolumeIsAvailable(bool& available);
    int32_t SetMicrophoneVolume(uint32_t volume);
    int32_t MicrophoneVolume(uint32_t& volume) const;
    int32_t MaxMicrophoneVolume(uint32_t& maxVolume) const;
    int32_t MinMicrophoneVolume(uint32_t& minVolume) const;
    int32_t MicrophoneVolumeStepSize(uint16_t& stepSize) const;
    int32_t Close();
    int32_t CloseSpeaker();
    int32_t CloseMicrophone();
    bool SpeakerIsInitialized() const;
    bool MicrophoneIsInitialized() const;

public:
    AudioMixerManagerLinuxALSA(const int32_t id);
    ~AudioMixerManagerLinuxALSA();

private:
    int32_t LoadMicMixerElement() const;
    int32_t LoadSpeakerMixerElement() const;
    void GetControlName(char *controlName, char* deviceName) const;

private:
    CriticalSectionWrapper& _critSect;
    int32_t _id;
    mutable snd_mixer_t* _outputMixerHandle;
    char _outputMixerStr[kAdmMaxDeviceNameSize];
    mutable snd_mixer_t* _inputMixerHandle;
    char _inputMixerStr[kAdmMaxDeviceNameSize];
    mutable snd_mixer_elem_t* _outputMixerElement;
    mutable snd_mixer_elem_t* _inputMixerElement;
};

}

#endif  // MODULES_AUDIO_DEVICE_MAIN_SOURCE_LINUX_AUDIO_MIXER_MANAGER_ALSA_LINUX_H_
