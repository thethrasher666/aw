//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#include "aw/core/string.hxx"
#include "platform/string.hxx"

namespace aw::core
{
    auto narrowToWide(std::string_view narrow) -> std::wstring
    {
        return platform::platformNarrowToWide(narrow);
    }

    auto wideToNarrow(std::wstring_view wide) -> std::string
    {
        return platform::platformWideToNarrow(wide);
    }
} // namespace aw::core
