//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#include "aw/multi-media/player.hxx"
#include "platform/audio.hxx"

namespace aw::media
{
    MediaPlayer::MediaPlayer(Object* parent) : Object(parent)
    {

    }

    MediaPlayer::~MediaPlayer()
    {

    }

    auto MediaPlayer::defaultAudioDevice() -> std::optional<AudioDeviceInfo>
    {
        return platform::platformDefaultAudioDevice();
    }

    auto MediaPlayer::enumerateAudioDevices() -> std::vector<AudioDeviceInfo>
    {
        return platform::platformEnumerateAudioDevices();
    }

    void MediaPlayer::setAudioDevice(AudioDeviceInfo const& audioDeviceInfo)
    {
        _audioDevice = addChild(std::make_unique<AudioDevice>(audioDeviceInfo, this));
    }
} // namespace aw::media
