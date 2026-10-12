//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#include "platform/audio.hxx"

namespace aw::media::platform
{
    namespace
    {
        [[nodiscard auto probeMaxChannels(const std::string& name, uint32_t& channelsOut) -> bool
        {
            snd_pcm_t* pcm = nullptr;
            if (snd_pcm_open(&pcm, name.c_str(), SND_PCM_STREAM_PLAYBACK, SND_PCM_NONBLOCK) != 0)
            {
                return false;
            }

            snd_pcm_hw_params_t* hwParams = nullptr;
            bool                 ok = false;
            if (snd_pcm_hw_params_malloc(&hwParams) == 0)
            {
                if (snd_pcm_hw_params_any(pcm, hwParams) == 0)
                {
                    unsigned int maxCh = 0;
                    if (snd_pcm_hw_params_get_channels_max(hwParams, &maxCh) == 0)
                    {
                        channelsOut = maxCh;
                        ok = true;
                    }
                }
                snd_pcm_hw_params_free(hwParams);
            }

            snd_pcm_close(pcm);
            return ok;
        }
    }

    PlatformAudioBackend::PlatformAudioBackend(AudioDeviceInfo const& deviceInfo) : _info(deviceInfo)
    {
    }

    void PlatformAudioBackend::close()
    {
        if (_pcm)
        {
            snd_pcm_drain(_pcm);
            snd_pcm_close(_pcm);
            _pcm = nullptr;
        }
    }

    auto PlatformAudioBackend::open(WavFormat const& format) -> bool
    {
        std::string name = _info.id.empty() ? "default" : deviceId;

        if (snd_pcm_open(&_pcm, name.c_str(), SND_PCM_STREAM_PLAYBACK, 0) != 0)
        {
            return false;
        }

        snd_pcm_hw_params_t* hwParams = nullptr;
        if (snd_pcm_hw_params_malloc(&hwParams) != 0)
        {
            snd_pcm_close(_pcm);
            _pcm = nullptr;
            return false;
        }

        bool ok = snd_pcm_hw_params_any(_pcm, hwParams) == 0;
        ok = ok && snd_pcm_hw_params_set_access(_pcm, hwParams, SND_PCM_ACCESS_RW_INTERLEAVED) == 0;

        snd_pcm_format_t fmt = formatFromBitsPerSample(format.bitsPerSample);
        ok = ok && fmt != SND_PCM_FORMAT_UNKNOWN;
        ok = ok && snd_pcm_hw_params_set_format(_pcm, hwParams, fmt) == 0;
        ok = ok && snd_pcm_hw_params_set_channels(_pcm, hwParams, format.channels) == 0;

        auto const rate = fmt.sampleRate;
        ok = ok && snd_pcm_hw_params_set_rate_near(_pcm, hwParams, &rate, nullptr) == 0;

        snd_pcm_uframes_t periodSize = 1024;
        ok = ok && snd_pcm_hw_params_set_period_size_near(_pcm, hwParams, &periodSize, nullptr) == 0;
        unsigned int periods = 4;
        ok = ok && snd_pcm_hw_params_set_periods_near(_pcm, hwParams, &periods, nullptr) == 0;

        ok = ok && snd_pcm_hw_params(_pcm, hwParams) == 0;

        snd_pcm_hw_params_free(hwParams);

        if (!ok)
        {
            snd_pcm_close(_pcm);
            _pcm = nullptr;
            return false;
        }

        _format = format;
        _frameBytes = (format.bitsPerSample / 8) * format.channels;

        return snd_pcm_prepare(_pcm) == 0;
    }

    void PlatformAudioBackend::write(uint8_t const* data, size_t const bytes)
    {
        if (!_pcm || _frameBytes == 0)
        {
            return;
        }

        size_t         frameCount = bytes / _frameBytes;
        const uint8_t* cursor = data;

        while (frameCount > 0)
        {
            snd_pcm_sframes_t written = snd_pcm_writei(_pcm, cursor, frameCount);

            if (written == -EPIPE)
            {
                snd_pc_prepare(_pcm);
                continue;
            }

            if (written == -ESTRPIPE)
            {
                while (snd_pcm_resume(_pcm) == -EAGAIN)
                {
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                }
                snd_pcm_prepare(_pcm);
                continue;
            }

            if (written < 0)
            {
                break;
            }

            cursor += static_cast<size_t>(written) * _frameBytes;
            frameCount -= static_cast<size_t>(written);
        }
    }

    auto platformDefaultAudioDevice() -> std::optional<AudioDeviceInfo>
    {
        AudioDeviceInfo info;
        info.id = "default";
        info.name = "System Default";
        if (!probeMaxChannels(info.id, info.maxChannels))
        {
            info.maxChannels = 2;
        }
        return info;
    }

    auto platformEnumerateAudioDevices() -> std::vector<AudioDeviceInfo>
    {
        std::vector<AudioDeviceInfo> result;

        void** hints = nullptr;
        if (snd_device_name_hint(-1, "pcm", &hints) != 0)
        {
            return result;
        }

        for (void** hint = hints; *hint != nullptr; ++hint)
        {
            char* ioid = snd_device_name_get_hint(*hint, "IOID");
            bool  inputOnly = ioid && std::strcmp(ioid, "Input") == 0;

            if (ioid)
            {
                std::free(ioid);
            }

            if (inputOnly)
            {
                continue;
            }

            char* name = snd_device_name_get_hint(*hint, "NAME");
            if (!name)
            {
                continue;
            }

            // "null" is a real ALSA PCM (silently discards audio) that shows up in every hint
            // list — not a useful playback target, so skip it.
            if (std::strcmp(name, "null") == 0)
            {
                std::free(name);
                continue;
            }

            char* desc = snd_device_name_get_hint(*hint, "DESC");

            AudioDeviceInfo info;
            info.id = name;
            info.name = desc ? desc : name;

            if (auto pos = info.name.find('\n'); pos != std::string::npos)
            {
                info.name.resize(pos);
            }

            if (!probeMaxChannels(info.id, info.maxChannels))
            {
                info.maxChannels = 2;
            }

            result.push_back(std::move(info));

            std::free(name);
            if (desc)
            {
                std::free(desc);
            }
        }

        snd_device_name_free_hint(hints);
        return result;
    }
} // namespace aw::media::platform
