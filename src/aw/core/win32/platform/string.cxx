//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#include "platform/string.hxx"

#include <Windows.h>

namespace aw::core::platform
{
    auto platformNarrowToWide(std::string_view narrow) -> std::wstring
    {
        int size = ::MultiByteToWideChar(CP_UTF8, 0, narrow.data(), -1, nullptr, 0);

        if (size <= 0)
            return {};

        std::wstring result(size - 1, L'\0');

        ::MultiByteToWideChar(CP_UTF8, 0, narrow.data(), -1, result.data(), size);

        return result;
    }

    auto platformWideToNarrow(std::wstring_view wide) -> std::string
    {
        int size = ::WideCharToMultiByte(CP_UTF8, 0, wide.data(), -1, nullptr, 0, nullptr, nullptr);

        if (size <= 0)
            return {};

        std::string result(size - 1, '\0');

        ::WideCharToMultiByte(CP_UTF8, 0, wide.data(), -1, result.data(), size, nullptr, nullptr);

        return result;
    }
} // namespace aw::core::platform
