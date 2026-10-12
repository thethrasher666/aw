//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#pragma once

#include <aw/multi-media/audio.hxx>

#include <optional>

namespace aw::media
{
    /// Enables the playback of media files.
    class MediaPlayer : public core::Object
    {
    public:
        /// Constructor.
        /// \param parent The parent object of this object.
        AW_MULTIMEDIA_API explicit MediaPlayer(core::Object* parent);

        /// Destructor.
        AW_MULTIMEDIA_API virtual ~MediaPlayer();

        /// Get the default audio device.
        /// \return A valid device if one is available, otherwise std::nullopt.
        [[nodiscard]] AW_MULTIMEDIA_API auto defaultAudioDevice() -> std::optional<AudioDeviceInfo>;

        /// Get the available audio devices.
        /// \return A list of available audio devices.
        [[nodiscard]] AW_MULTIMEDIA_API auto enumerateAudioDevices() -> std::vector<AudioDeviceInfo>;

        /// Set the audio device to use for playback.
        /// \param audioDeviceInfo The audio device information to set for playback.
        AW_MULTIMEDIA_API void setAudioDevice(AudioDeviceInfo const& audioDeviceInfo);

    private:
        AudioDevice* _audioDevice{}; ///< The audio device to use.
    };
} // namespace aw::media
