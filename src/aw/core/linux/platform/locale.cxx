//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#include "locale.hxx"
#include "string.hxx"

#include <cstdlib>

namespace aw::core::platform
{
    auto platformLocale() -> std::string
    {
        // Linux/Unix
        if (const char* lang = std::getenv("LANG"); lang)
        {
            std::string locale(lang);

            // Trim whitespace
            const size_t first_non_ws = locale.find_first_not_of(" \t\n\r");
            if (first_non_ws == std::string::npos)
            {
                return "en-US";
            }

            locale.erase(0, first_non_ws);
            locale.erase(locale.find_last_not_of(" \t\n\r") + 1);

            // Extract locale from formats like "en_US.UTF-8"
            const size_t dot_position = locale.find('.');
            if (dot_position != std::string::npos)
            {
                locale = locale.substr(0, dot_position);
            }

            // Handle "C" or "POSIX" locales which are minimal/default
            if (locale.empty() || locale == "C" || locale == "POSIX")
            {
                return "en-US";
            }

            // Convert from "en_US" to "en-US"
            const size_t underscore_position = locale.find('_');
            if (underscore_position != std::string::npos)
            {
                locale[underscore_position] = '-';
            }

            return locale;
        }

        return "en-US"; // Default fallback
    }
} // namespace aw::core::platform
