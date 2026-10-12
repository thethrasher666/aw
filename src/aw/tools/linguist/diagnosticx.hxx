//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#pragma once

#include <print>
#include <string_view>

namespace aw::linguist
{
    /// Print an error message to stderr with a standard prefix.
    /// \tparam ...Args The types of the format arguments.
    /// \param fmt The format string.
    /// \param ...args The format arguments.
    template <typename... Args>
    void logError(std::string_view fmt, Args&&... args)
    {
        std::print(stderr, "[error] {}", fmt);
        (std::print(stderr, " {}", std::forward<Args>(args)), ...);
        std::print(stderr, "\n");
    }
} // namespace aw::linguist
