//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#include "string.hxx"

#include <cstdlib>
#include <vector>

namespace aw::core::platform
{
    auto platformNarrowToWide(std::string_view narrow) -> std::wstring
    {
        if (narrow.empty())
        {
            return {};
        }

        std::string const input(narrow);

        size_t const length = std::mbstowcs(nullptr, input.c_str(), 0);
        if (length == static_cast<size_t>(-1))
        {
            return {};
        }

        std::vector<wchar_t> buffer(length + 1, L'\0');
        if (std::mbstowcs(buffer.data(), input.c_str(), buffer.size()) == static_cast<size_t>(-1))
        {
            return {};
        }

        return std::wstring(buffer.data(), length);
    }

    auto platformWideToNarrow(std::wstring_view wide) -> std::string
    {
        if (wide.empty())
        {
            return {};
        }

        std::wstring const input(wide);

        size_t const length = std::wcstombs(nullptr, input.c_str(), 0);
        if (length == static_cast<size_t>(-1))
        {
            return {};
        }

        std::vector<char> buffer(length + 1, '\0');
        if (std::wcstombs(buffer.data(), input.c_str(), buffer.size()) == static_cast<size_t>(-1))
        {
            return {};
        }

        return std::string(buffer.data(), length);
    }
} // namespace aw::core::platform
