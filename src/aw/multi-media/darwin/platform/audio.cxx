//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#include "platform/audio.hxx"

#include <aw/coreAudio/CoreAudio.h>

namespace aw::media::platform
{
    namespace
    {
        auto getDeviceUID(AudioObjectID devId, std::string& uidOut) -> bool
        {
            AudioObjectPropertyAddress addr = {
                kAudioDevicePropertyDeviceUID,
                kAudioObjectPropertyScopeGlobal,
                kAudioObjectPropertyElementMain
            };
            CFStringRef uidRef = nullptr;
            UInt32      size = sizeof(uidRef);
            if (AudioObjectGetPropertyData(devId, &addr, 0, nullptr, &size, &uidRef) != noErr || !uidRef)
            {
                return false;
            }

            char buf[256];
            bool ok = CFStringGetCString(uidRef, buf, sizeof(buf), kCFStringEncodingUTF8);
            if (ok)
            {
                uidOut = buf;
            }
            CFRelease(uidRef);
            return ok;
        }

        auto queryDeviceInfo(AudioObjectID devId, AudioDeviceInfo& out) -> bool
        {
            // Output capability + channel count, via the output-scoped stream configuration.
            AudioObjectPropertyAddress streamConfigAddr = { kAudioDevicePropertyStreamConfiguration, kAudioDevicePropertyScopeOutput, kAudioObjectPropertyElementMain };

            UInt32 configSize = 0;
            if (AudioObjectGetPropertyDataSize(devId, &streamConfigAddr, 0, nullptr, &configSize) != noErr)
            {
                return false;
            }

            if (configSize == 0)
            {
                return false; // no output streams on this device (e.g. input-only)
            }

            std::vector<uint8_t> raw(configSize);
            auto*                bufferList = reinterpret_cast<AudioBufferList*>(raw.data());
            if (AudioObjectGetPropertyData(devId, &streamConfigAddr, 0, nullptr, &configSize, bufferList) != noErr)
            {
                return false;
            }

            uint32_t channelCount = 0;
            for (UInt32 i = 0; i < bufferList->mNumberBuffers; ++i)
            {
                channelCount += bufferList->mBuffers[i].mNumberChannels;
            }

            if (channelCount == 0)
            {
                return false;
            }

            std::string uid;
            if (!getDeviceUID(devId, uid))
            {
                return false;
            }

            AudioObjectPropertyAddress nameAddr = { kAudioObjectPropertyName, kAudioObjectPropertyScopeGlobal, kAudioObjectPropertyElementMain };
            CFStringRef                nameRef = nullptr;
            UInt32                     nameSize = sizeof(nameRef);
            std::string                name = "Unknown Device";
            if (AudioObjectGetPropertyData(devId, &nameAddr, 0, nullptr, &nameSize, &nameRef) == noErr && nameRef)
            {
                char buf[256];
                if (CFStringGetCString(nameRef, buf, sizeof(buf), kCFStringEncodingUTF8))
                {
                    name = buf;
                }
                CFRelease(nameRef);
            }

            out.id = std::move(uid);
            out.name = std::move(name);
            out.maxChannels = channelCount;
            return true;
        }
    } // namespace

    PlatformAudioBackend::PlatformAudioBackend(AudioDeviceInfo const& deviceInfo) : _info(deviceInfo)
    {
    }

    void PlatformAudioBackend::close()
    {
        if (_queue)
        {
            AudioQueueStop(_queue, true); // true = stop immediately, discard queued buffers
            AudioQueueDispose(_queue, true);
            _queue = nullptr;
        }
        std::lock_guard<std::mutex> lock(_mutex);
        _pending.clear();
    }

    auto PlatformAudioBackend::open(WavFormat const& format) -> bool
    {
        AudioStreamBasicDescription asbd = {};
        asbd.mSampleRate = format.sampleRate;
        asbd.mFormatID = kAudioFormatLinearPCM;
        asbd.mFormatFlags = kLinearPCMFormatFlagIsSignedInteger | kLinearPCMFormatFlagIsPacked;
        asbd.mBitsPerChannel = format.bitsPerSample;
        asbd.mChannelsPerFrame = format.channels;
        asbd.mBytesPerFrame = (format.bitsPerSample / 8) * format.channels;
        asbd.mFramesPerPacket = 1;
        asbd.mBytesPerPacket = asbd.mBytesPerFrame * asbd.mFramesPerPacket;

        OSStatus status = AudioQueueNewOutput(&asbd, staticOutputCallback, this, nullptr, kCFRunLoopCommonModes, 0, &_queue);
        if (status != noErr)
        {
            return false;
        }

        if (_info.id.empty())
        {
            CFStringRef uid = CFStringCreateWithCString(kCFAllocatorDefault, _info.id.c_str(), kCFStringEncodingUTF8);
            AudioQueueSetProperty(_queue, kAudioQueueProperty_CurrentDevice, &uid, sizeof(uid));
            CFRelease(uid);
            // Not checked as fatal: on failure the queue just falls back to the system default output.
        }

        for (size_t i = 0; i < kNumBuffers; ++i)
        {
            if (AudioQueueAllocateBuffer(_queue, kBufferByteSize, &_buffers[i]) != noErr)
            {
                close();
                return false;
            }
            _buffers[i]->mAudioDataByteSize = kBufferByteSize;
            std::memset(_buffers[i]->mAudioData, 0, kBufferByteSize);
            AudioQueueEnqueueBuffer(_queue, _buffers[i], 0, nullptr);
        }

        return AudioQueueStart(_queue, nullptr) == noErr;
    }

    void PlatformAudioBackend::write(uint8_t const* data, size_t const bytes)
    {
        std::lock_guard<std::mutex> lock(_mutex);
        _pending.insert(_pending.end(), data, data + bytes);
    }

    void PlatformAudioBackend::handleOutputCallback(AudioQueueRef queue, AudioQueueBufferRef buffer)
    {
        refillBuffer(buffer);
        AudioQueueEnqueueBuffer(queue, buffer, 0, nullptr);
    }

    void PlatformAudioBackend::refillBuffer(AudioQueueBufferRef buffer)
    {
        std::lock_guard<std::mutex> lock(_mutex);

        UInt32 capacity = buffer->mAudioDataBytesCapacity;
        size_t toCopy = std::min<size_t>(capacity, _pending.size());

        uint8_t* dst = reinterpret_cast<uint8_t*>(buffer->mAudioData);
        std::copy(_pending.begin(), _pending.begin() + toCopy, dst);
        _pending.erase(_pending.begin(), _pending.begin() + toCopy);

        if (toCopy < capacity)
        {
            std::memset(dst + toCopy, 0, capacity - toCopy); // pad with silence if the source underruns
        }

        buffer->mAudioDataByteSize = capacity;
    }

    void PlatformAudioBackend::staticOutputCallback(void* userData, AudioQueueRef queue, AudioQueueBufferRef buffer)
    {
        reinterpret_cast<PlatformAudioBackend*>(userData)->handleOutputCallback(queue, buffer);
    }

    auto platformDefaultAudioDevice() -> std::optional<AudioDeviceInfo>
    {
        AudioObjectPropertyAddress addr = { kAudioHardwarePropertyDefaultOutputDevice, kAudioObjectPropertyScopeGlobal, kAudioObjectPropertyElementMain };
        AudioObjectID              devId = kAudioObjectUnknown;
        UInt32                     size = sizeof(devId);

        if (AudioObjectGetPropertyData(kAudioObjectSystemObject, &addr, 0, nullptr, &size, &devId) != noErr)
        {
            return {};
        }

        AudioDeviceInfo info;
        queryDeviceInfo(devId, info);
        return info;
    }

    auto platformEnumerateAudioDevices() -> std::vector<AudioDeviceInfo>
    {
        std::vector<AudioDeviceInfo> result;

        AudioObjectPropertyAddress devicesAddr = { kAudioHardwarePropertyDevices, kAudioObjectPropertyScopeGlobal, kAudioObjectPropertyElementMain };

        UInt32 dataSize = 0;
        if (AudioObjectGetPropertyDataSize(kAudioObjectSystemObject, &devicesAddr, 0, nullptr, &dataSize) != noErr)
        {
            return result;
        }

        size_t                     count = dataSize / sizeof(AudioObjectID);
        std::vector<AudioObjectID> ids(count);
        if (AudioObjectGetPropertyData(kAudioObjectSystemObject, &devicesAddr, 0, nullptr, &dataSize, ids.data()) != noErr)
        {
            return result;
        }

        for (AudioObjectID devId : ids)
        {
            AudioDeviceInfo info;
            if (queryDeviceInfo(devId, info))
            {
                result.push_back(std::move(info));
            }
        }
        return result;
    }
} // namespace aw::media::platform
