//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#pragma once

#include <multi-media/audio.hxx>

#include <optional>
#include <alsa/asoundlib.h>

namespace aw::media::platform
{
    class PlatformAudioBackend
    {
    public:
        /// Constructor.
        /// \param deviceInfo The audio device information.
        explicit PlatformAudioBackend(AudioDeviceInfo const& deviceInfo);

        /// Closes the audio backend and releases any associated resources.
        void close();

        /// Opens the audio backend with the specified WAV format.
        /// \param format The WAV format to use for audio playback.
        /// \return true if the audio backend was successfully opened, false otherwise.
        [[nodiscard]] auto open(WavFormat const& format) -> bool;

        /// Writes the specified audio data to the audio backend for playback.
        /// \param data A pointer to the audio data to write.
        /// \param bytes The number of bytes of audio data to write.
        void write(uint8_t const* data, size_t const bytes);

    private:
        AudioDeviceInfo const _info;         ///< Audio device information.
        snd_pcm_t*            _pcm{};        ///< The ALSA PCM handle for the audio device.
        WavFormat             _format;       ///< The WAV format used for audio playback.
        size_t                _frameBytes{}; ///< The frame size, in bytes.
    };

    /// Gets the default output audio device for the platform.
    /// \return An optional AudioDeviceInfo structure containing information about the default output audio device, or std::nullopt if no default device is available.
    [[nodiscard]] auto platformDefaultAudioDevice() -> std::optional<AudioDeviceInfo>;

    /// Enumerates the available output audio devices for the platform.
    /// \return A vector of AudioDeviceInfo structures containing information about the available output audio devices
    [[nodiscard]] auto platformEnumerateAudioDevices() -> std::vector<AudioDeviceInfo>;
} // namespace aw::media::platform
