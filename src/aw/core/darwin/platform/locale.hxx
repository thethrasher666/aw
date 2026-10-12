//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#pragma once

#include <string>

namespace aw::core::platform
{
    /// Get the platform locale as a string.
    /// \return The platform locale as a string.
    [[nodiscard]] auto platformLocale() -> std::string;
} // namespace aw::core::platform
