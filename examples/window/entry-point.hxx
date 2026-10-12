//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#pragma once

#include <string>
#include <vector>

namespace aw::examples
{
    /// Entry point for the application.
    /// \return true if the application started successfully; otherwise, false.
    [[nodiscard]] auto entry_point(std::vector<std::string> const& arguments) -> bool;
} // namespace aw::examples
