//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#pragma once

#include <aw/core/api.hxx>

#include <string>

namespace aw::core
{
    /// Converts a narrow string to a wide string.
    /// \param narrow The narrow string to convert.
    /// \return The converted wide string.
    [[nodiscard]] AW_CORE_API auto narrowToWide(std::string_view narrow) -> std::wstring;

    /// Converts a wide string to a narrow string.
    /// \param wide The wide string to convert.
    /// \return The converted narrow string.
    [[nodiscard]] AW_CORE_API auto wideToNarrow(std::wstring_view wide) -> std::string;

} // namespace aw::core
