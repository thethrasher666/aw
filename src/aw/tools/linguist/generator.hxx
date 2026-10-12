//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#pragma once

#include "manifest.hxx"

#include <memory>
#include <string>

namespace aw::linguist
{
    /// Generate the embedded translation table for a manifest.
    /// \param manifest The manifest describing the strings to embed.
    /// \return The generated C++ source.
    auto generate(Manifest const& manifest) -> std::unique_ptr<std::string>;
} // namespace aw::linguist
