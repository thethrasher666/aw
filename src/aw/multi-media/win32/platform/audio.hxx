//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#pragma once

#include <aw/multi-media/audio.hxx>

#include <deque>
#include <mutex>
#include <optional>
#include <audioclient.h>
#include <wrl/client.h>

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
        void renderThreadMain();

    private:
        AudioDeviceInfo const                      _info;             ///< Audio device information.
        std::mutex                                 _mutex;            ///< Mutex to protect access to the pending audio data.
        std::deque<uint8_t>                        _pending;          ///< PCM bytes waiting to be consumed by the queue callback
        WavFormat                                  _format;           ///< The format of the WAV file.
        Microsoft::WRL::ComPtr<IAudioClient>       _audioClient;      ///< Pointer to the IAudioClient interface for audio playback.
        uint32_t                                   _bufferFrames{};   ///< Number of frames in the audio buffer.
        Microsoft::WRL::ComPtr<IAudioRenderClient> _renderClient;     ///< Pointer to the IAudioRenderClient interface for audio playback.
        HANDLE                                     _renderEvent{};    ///< Handle to the render event used for audio playback.
        std::thread                                _renderThread;     ///< Thread for rendering audio data.
        std::atomic<bool>                          _running{ false }; ///< Flag indicating whether the audio backend is running.
    };

    /// Gets the default output audio device for the platform.
    /// \return An optional AudioDeviceInfo structure containing information about the default output audio device, or std::nullopt if no default device is available.
    [[nodiscard]] auto platformDefaultAudioDevice() -> std::optional<AudioDeviceInfo>;

    /// Enumerates the available output audio devices for the platform.
    /// \return A vector of AudioDeviceInfo structures containing information about the available output audio devices
    [[nodiscard]] auto platformEnumerateAudioDevices() -> std::vector<AudioDeviceInfo>;
} // namespace aw::media::platform
