//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#include "locale.hxx"

#include <aw/coreFoundation/CoreFoundation.h>

namespace aw::core::platform
{
    auto platformLocale() -> std::string
    {
        CFLocaleRef const locale = CFLocaleCopyCurrent();
        if (!locale)
        {
            return {};
        }

        auto const localeID = static_cast<CFStringRef>(CFLocaleGetValue(locale, kCFLocaleIdentifier));

        char          buffer[128]{};
        Boolean const converted = localeID && CFStringGetCString(localeID, buffer, sizeof(buffer), kCFStringEncodingUTF8);

        CFRelease(locale);

        if (!converted)
        {
            return {};
        }

        std::string result(buffer);
        if (auto const keywords = result.find('@'); keywords != std::string::npos)
        {
            result.erase(keywords);
        }

        // Convert from "en_US" to "en-US"
        if (auto const separator = result.find('_'); separator != std::string::npos)
        {
            result[separator] = '-';
        }

        return result;
    }
} // namespace aw::core::platform
