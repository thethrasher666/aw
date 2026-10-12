//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#include "locale.hxx"
#include "string.hxx"

#include <windows.h>

namespace aw::core::platform
{
    auto platformLocale() -> std::string
    {
        wchar_t localeName[LOCALE_NAME_MAX_LENGTH] = { 0 };
        if (GetUserDefaultLocaleName(localeName, ARRAYSIZE(localeName)) > 0)
        {
            return platformWideToNarrow(localeName);
        }

        return "en-US"; // Default fallback
    }
} // namespace aw::core::platform
