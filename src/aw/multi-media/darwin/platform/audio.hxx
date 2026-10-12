//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#pragma once

#include <multi-media/audio.hxx>

#include <deque>
#include <mutex>
#include <AudioToolbox/AudioToolbox.h>
#include <aw/coreAudio/CoreAudio.h>

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
        void        handleOutputCallback(AudioQueueRef queue, AudioQueueBufferRef buffer);
        void        refillBuffer(AudioQueueBufferRef buffer);
        static void staticOutputCallback(void* userData, AudioQueueRef queue, AudioQueueBufferRef buffer);

    private:
        static constexpr int    kNumBuffers = 3;         ///< Number of audio buffers to use for the audio queue.
        static constexpr UInt32 kBufferByteSize = 16384; ///< Size of each audio buffer in bytes.

        AudioDeviceInfo const _info;                      ///< Audio device information.
        AudioQueueRef         _queue = nullptr;           ///< The audio queue used for playback.
        AudioQueueBufferRef   _buffers[kNumBuffers] = {}; ///< The audio buffers used for playback.
        std::mutex            _mutex;                     ///< Mutex to protect access to the pending audio data.
        std::deque<uint8_t>   _pending;                   ///< PCM bytes waiting to be consumed by the queue callback
    };

    /// Gets the default output audio device for the platform.
    /// \return An optional AudioDeviceInfo structure containing information about the default output audio device, or std::nullopt if no default device is available.
    [[nodiscard]] auto platformDefaultAudioDevice() -> std::optional<AudioDeviceInfo>;

    /// Enumerates the available output audio devices for the platform.
    /// \return A vector of AudioDeviceInfo structures containing information about the available output audio devices
    [[nodiscard]] auto platformEnumerateAudioDevices() -> std::vector<AudioDeviceInfo>;
} // namespace aw::media::platform
