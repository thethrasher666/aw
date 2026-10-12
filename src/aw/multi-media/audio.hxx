//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#pragma once

#include <aw/core/object.hxx>
#include <aw/multi-media/api.hxx>

namespace aw::media
{
    namespace platform
    {
        class PlatformAudioBackend; ///< Forward declaration.
    }

    /// Information about an audio device.
    struct AudioDeviceInfo
    {
        std::string id;              ///< ID.
        std::string name;            ///< Name.
        uint32_t    maxChannels = 0; ///< Maximum number of channels supported by the device.
    };

    /// Represents the format of a WAV audio file.
    struct WavFormat
    {
        uint32_t sampleRate = 44100; ///< Sample rate in Hz.
        uint16_t channels = 2;       ///< Number of channels (1 = mono, 2 = stereo, etc.).
        uint16_t bitsPerSample = 16; ///< Bits per sample (8, 16, 24, or 32).
    };

    /// Represents an audio device.
    class AudioDevice : public core::Object
    {
    public:
        /// Constructor, use the specified audio device.
        /// \param info The audio device information.
        /// \param parent The parent object of this object.
        AW_MULTIMEDIA_API explicit AudioDevice(AudioDeviceInfo const& info, core::Object* parent);

        /// Destructor.
        AW_MULTIMEDIA_API virtual ~AudioDevice();

        /// Get the ID of the audio device.
        /// \return The ID of the audio device.
        [[nodiscard]] AW_MULTIMEDIA_API auto id() const -> std::string;

        /// Get the name of the audio device.
        /// \return The name of the audio device.
        [[nodiscard]] AW_MULTIMEDIA_API auto name() const -> std::string;

        /// Get the maximum number of channels supported by the audio device.
        /// \return The maximum number of channels supported by the audio device.
        [[nodiscard]] AW_MULTIMEDIA_API auto maxChannels() const -> uint32_t;

    private:
        AudioDeviceInfo const                           _info;            ///< Audio device information.
        std::unique_ptr<platform::PlatformAudioBackend> _platformBackend; ///< Platform-specific audio backend
    };
} // namespace aw::media
