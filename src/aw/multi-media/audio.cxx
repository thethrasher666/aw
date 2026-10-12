//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#include "aw/multi-media/audio.hxx"
#include "platform/audio.hxx"

namespace aw::media
{
    AudioDevice::AudioDevice(AudioDeviceInfo const& info, core::Object* parent) : core::Object(parent), _info(info)
    {
        _platformBackend = std::make_unique<platform::PlatformAudioBackend>(info);
    }

    AudioDevice::~AudioDevice()
    {

    }
} // namespace aw::media
