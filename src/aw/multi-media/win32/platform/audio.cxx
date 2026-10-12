//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#include "platform/audio.hxx"

#include <mmdeviceapi.h>

#include <functiondiscoverykeys_devpkey.h>

namespace aw::media::platform
{
    namespace
    {
        struct ComScope
        {
            bool initializedHere = false;

            ComScope()
            {
                auto hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
                initializedHere = SUCCEEDED(hr);
            }

            ~ComScope()
            {
                if (initializedHere)
                {
                    CoUninitialize();
                }
            }
        };

        [[nodiscard]] auto utf8ToWide(std::string const& utf8) -> std::wstring
        {
            if (utf8.empty())
            {
                return {};
            }

            auto const len = MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, nullptr, 0);
            if (len <= 0)
            {
                return {};
            }

            std::wstring result(static_cast<size_t>(len - 1), L'\0');
            MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, result.data(), len);
            return result;
        }

        [[nodiscard]] auto wideToUtf8(wchar_t const* wide) -> std::string
        {
            if (!wide)
            {
                return {};
            }

            auto const len = WideCharToMultiByte(CP_UTF8, 0, wide, -1, nullptr, 0, nullptr, nullptr);
            if (len <= 0)
            {
                return {};
            }
            std::string result(static_cast<size_t>(len - 1), '\0');
            WideCharToMultiByte(CP_UTF8, 0, wide, -1, result.data(), len, nullptr, nullptr);
            return result;
        }

        [[nodiscard]] auto findDevice(std::string const& deviceId) -> Microsoft::WRL::ComPtr<IMMDevice>
        {
            Microsoft::WRL::ComPtr<IMMDeviceEnumerator> enumerator;
            if (FAILED(CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), &enumerator)))
            {
                return nullptr;
            }

            Microsoft::WRL::ComPtr<IMMDevice> device;
            if (deviceId.empty())
            {
                enumerator->GetDefaultAudioEndpoint(eRender, eConsole, &device);
            }
            else
            {
                std::wstring wide = utf8ToWide(deviceId);
                enumerator->GetDevice(wide.c_str(), &device);
            }
            return device;
        }

        [[nodiscard]] auto queryDeviceInfo(IMMDevice* device, AudioDeviceInfo& out) -> bool
        {
            LPWSTR idStr = nullptr;
            if (FAILED(device->GetId(&idStr)))
            {
                return false;
            }
            out.id = wideToUtf8(idStr);
            CoTaskMemFree(idStr);

            Microsoft::WRL::ComPtr<IPropertyStore> props;
            if (SUCCEEDED(device->OpenPropertyStore(STGM_READ, &props)))
            {
                PROPVARIANT nameVar;
                PropVariantInit(&nameVar);
                if (SUCCEEDED(props->GetValue(PKEY_Device_FriendlyName, &nameVar)) && nameVar.vt == VT_LPWSTR)
                {
                    out.name = wideToUtf8(nameVar.pwszVal);
                }
                PropVariantClear(&nameVar);
            }

            if (out.name.empty())
            {
                out.name = "Unknown Device";
            }

            // Channel count: the endpoint's current mix format is the cheapest reliable source
            // (typically 2 for stereo, more for surround-configured outputs).
            out.maxChannels = 2; // sane fallback
            Microsoft::WRL::ComPtr<IAudioClient> tempClient;
            if (SUCCEEDED(device->Activate(__uuidof(IAudioClient), CLSCTX_ALL, nullptr, &tempClient)))
            {
                WAVEFORMATEX* mixFormat = nullptr;
                if (SUCCEEDED(tempClient->GetMixFormat(&mixFormat)) && mixFormat)
                {
                    out.maxChannels = mixFormat->nChannels;
                    CoTaskMemFree(mixFormat);
                }
            }

            return true;
        }
    } // namespace

    PlatformAudioBackend::PlatformAudioBackend(AudioDeviceInfo const& deviceInfo) : _info(deviceInfo)
    {
    }

    void PlatformAudioBackend::close()
    {
        _running = false;
        if (_renderEvent)
        {
            SetEvent(_renderEvent); // wake the render thread so it can observe m_running == false
        }

        if (_renderThread.joinable())
        {
            _renderThread.join();
        }

        if (_audioClient)
        {
            _audioClient->Stop();
        }

        _renderClient.Reset();
        _audioClient.Reset();

        if (_renderEvent)
        {
            CloseHandle(_renderEvent);
            _renderEvent = nullptr;
        }

        std::lock_guard<std::mutex> lock(_mutex);
        _pending.clear();
    }

    auto PlatformAudioBackend::open(WavFormat const& format) -> bool
    {
        ComScope com; // covers device lookup + client activation on this (caller's) thread

        Microsoft::WRL::ComPtr<IMMDevice> device = findDevice(_info.id);
        if (!device)
        {
            return false;
        }

        if (FAILED(device->Activate(__uuidof(IAudioClient), CLSCTX_ALL, nullptr, &_audioClient)))
        {
            return false;
        }

        WAVEFORMATEX wfx = {};
        wfx.wFormatTag = WAVE_FORMAT_PCM;
        wfx.nChannels = format.channels;
        wfx.nSamplesPerSec = format.sampleRate;
        wfx.wBitsPerSample = format.bitsPerSample;
        wfx.nBlockAlign = (wfx.nChannels * wfx.wBitsPerSample) / 8;
        wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;
        wfx.cbSize = 0;

        if (FAILED(_audioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, AUDCLNT_STREAMFLAGS_EVENTCALLBACK, 0, 0, &wfx, nullptr)))
        {
            return false;
        }

        if (FAILED(_audioClient->GetBufferSize(&_bufferFrames)))
        {
            _audioClient.Reset();
            return false;
        }

        _renderEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
        if (!_renderEvent || FAILED(_audioClient->SetEventHandle(_renderEvent)))
        {
            _audioClient.Reset();
            return false;
        }

        if (FAILED(_audioClient->GetService(__uuidof(IAudioRenderClient), &_renderClient)))
        {
            _audioClient.Reset();
            return false;
        }

        _format = format;

        // Pre-fill one full buffer of silence so playback starts glitch-free before the first
        // real write() lands — standard practice for event-driven shared-mode render.
        BYTE* data = nullptr;
        if (SUCCEEDED(_renderClient->GetBuffer(_bufferFrames, &data)))
        {
            _renderClient->ReleaseBuffer(_bufferFrames, AUDCLNT_BUFFERFLAGS_SILENT);
        }

        if (FAILED(_audioClient->Start()))
        {
            close();
            return false;
        }

        _running = true;
        _renderThread = std::thread(&PlatformAudioBackend::renderThreadMain, this);
        return true;
    }

    void PlatformAudioBackend::write(uint8_t const* data, size_t const bytes)
    {
        std::lock_guard<std::mutex> lock(_mutex);
        _pending.insert(_pending.end(), data, data + bytes);
    }

    void PlatformAudioBackend::renderThreadMain()
    {
        ComScope com;

        const UINT32 blockAlign = (static_cast<UINT32>(_format.channels) * _format.bitsPerSample) / 8;

        while (_running)
        {
            auto const waitResult = WaitForSingleObject(_renderEvent, 2000);
            if (waitResult != WAIT_OBJECT_0)
            {
                continue; // timeout or spurious wake — loop back and re-check m_running
            }

            uint32_t paddingFrames = 0;
            if (FAILED(_audioClient->GetCurrentPadding(&paddingFrames)))
            {
                continue;
            }

            uint32_t framesAvailable = _bufferFrames - paddingFrames;
            if (framesAvailable == 0)
            {
                continue;
            }

            BYTE* data = nullptr;
            if (FAILED(_renderClient->GetBuffer(framesAvailable, &data)))
            {
                continue;
            }

            size_t                       bytesAvailable = static_cast<size_t>(framesAvailable) * blockAlign;
            std::unique_lock<std::mutex> lock(_mutex);
            size_t                       toCopy = std::min(bytesAvailable, _pending.size());

            std::copy(_pending.begin(), _pending.begin() + toCopy, data);
            _pending.erase(_pending.begin(), _pending.begin() + toCopy);
            lock.unlock();

            DWORD flags = 0;
            if (toCopy < bytesAvailable)
            {
                std::memset(data + toCopy, 0, bytesAvailable - toCopy); // pad with silence on underrun
                if (toCopy == 0)
                {
                    flags = AUDCLNT_BUFFERFLAGS_SILENT;
                }
            }

            _renderClient->ReleaseBuffer(framesAvailable, flags);
        }
    }

    auto platformDefaultAudioDevice() -> std::optional<AudioDeviceInfo>
    {
        ComScope        com;
        AudioDeviceInfo info;

        Microsoft::WRL::ComPtr<IMMDeviceEnumerator> enumerator;
        if (FAILED(CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), &enumerator)))
        {
            return info;
        }

        Microsoft::WRL::ComPtr<IMMDevice> device;
        if (FAILED(enumerator->GetDefaultAudioEndpoint(eRender, eConsole, &device)))
        {
            return info;
        }

        if (queryDeviceInfo(device.Get(), info))
        {
            return info;
        }

        return std::nullopt;
    }

    auto platformEnumerateAudioDevices() -> std::vector<AudioDeviceInfo>
    {
        std::vector<AudioDeviceInfo> result;
        ComScope                     com;

        Microsoft::WRL::ComPtr<IMMDeviceEnumerator> enumerator;
        if (FAILED(CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), &enumerator)))
        {
            return result;
        }

        Microsoft::WRL::ComPtr<IMMDeviceCollection> collection;
        if (FAILED(enumerator->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, &collection)))
        {
            return result;
        }

        UINT count = 0;
        collection->GetCount(&count);

        for (UINT i = 0; i < count; ++i)
        {
            Microsoft::WRL::ComPtr<IMMDevice> device;
            if (FAILED(collection->Item(i, &device)))
            {
                continue;
            }

            AudioDeviceInfo info;
            if (queryDeviceInfo(device.Get(), info))
            {
                result.push_back(std::move(info));
            }
        }

        return result;
    }
} // namespace aw::media::platform
