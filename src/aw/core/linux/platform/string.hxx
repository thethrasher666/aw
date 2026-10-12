//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#pragma once

#include <string>

namespace aw::core::platform
{
    /// Converts a narrow string to a wide string.
    /// \param narrow The narrow string to convert.
    /// \return The converted wide string.
    [[nodiscard]] auto platformNarrowToWide(std::string_view narrow) -> std::wstring;

    /// Converts a wide string to a narrow string.
    /// \param wide The wide string to convert.
    /// \return The converted narrow string.
    [[nodiscard]] auto platformWideToNarrow(std::wstring_view wide) -> std::string;
} // namespace aw::core::platform
