//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#include "platform/string.hxx"

#include <aw/coreFoundation/CoreFoundation.h>

#include <cstring>

namespace aw::core::platform
{
    auto platformNarrowToWide(std::string_view narrow) -> std::wstring
    {
        static_assert(sizeof(wchar_t) == 4, "Darwin expects 32-bit wchar_t for UTF-32 conversion.");

        CFIndex numBytes = static_cast<CFIndex>(narrow.size());

        CFStringRef cfStr = CFStringCreateWithBytes(
            kCFAllocatorDefault,
            reinterpret_cast<const UInt8 *>(narrow.data()),
            numBytes,
            kCFStringEncodingUTF8,
            false
        );

        std::wstring result;
        if (cfStr)
        {
            CFIndex length = CFStringGetLength(cfStr);

            CFIndex maxBytes = CFStringGetMaximumSizeForEncoding(length, kCFStringEncodingUTF32LE);
            std::string utf32Bytes;
            utf32Bytes.resize(static_cast<size_t>(maxBytes));

            CFIndex usedBytes = 0;
            Boolean success = CFStringGetBytes(
                cfStr,
                CFRangeMake(0, length),
                kCFStringEncodingUTF32LE,
                0,
                false,
                reinterpret_cast<UInt8 *>(utf32Bytes.data()),
                maxBytes,
                &usedBytes
            );

            if (success && (usedBytes % static_cast<CFIndex>(sizeof(wchar_t)) == 0))
            {
                result.resize(static_cast<size_t>(usedBytes / static_cast<CFIndex>(sizeof(wchar_t))));
                std::memcpy(result.data(), utf32Bytes.data(), static_cast<size_t>(usedBytes));
            }

            CFRelease(cfStr);
        }

        return result;
    }

    auto platformWideToNarrow(std::wstring_view wide) -> std::string
    {
        static_assert(sizeof(wchar_t) == 4, "Darwin expects 32-bit wchar_t for UTF-32 conversion.");

        CFIndex numBytes = static_cast<CFIndex>(wide.size() * sizeof(wchar_t));

        CFStringRef cfStr = CFStringCreateWithBytes(
            kCFAllocatorDefault,
            reinterpret_cast<const UInt8 *>(wide.data()),
            numBytes,
            kCFStringEncodingUTF32LE,
            false
        );

        std::string result;
        if (cfStr)
        {
            CFIndex length = CFStringGetLength(cfStr);
            CFIndex maxBytes = CFStringGetMaximumSizeForEncoding(length, kCFStringEncodingUTF8);
            result.resize(maxBytes);

            CFIndex usedBytes = 0;
            Boolean success = CFStringGetBytes(
                cfStr,
                CFRangeMake(0, length),
                kCFStringEncodingUTF8,
                0,
                false,
                reinterpret_cast<UInt8 *>(result.data()),
                maxBytes,
                &usedBytes
            );

            if (success)
            {
                result.resize(usedBytes);
            }
            else
            {
                result.clear();
            }

            CFRelease(cfStr);
        }

        return result;
    }
} // namespace aw::core::platform
