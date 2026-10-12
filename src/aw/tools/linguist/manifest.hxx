//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#pragma once

#include <filesystem>
#include <string>
#include <vector>

namespace aw::linguist
{
    /// A single translated string, addressed by key and locale.
    struct Entry
    {
        std::string key;    ///< The key of the string, e.g. "home.title".
        std::string locale; ///< The locale the text is translated for, e.g. "en-US".
        std::string text;   ///< The translated text.
    };

    /// Represents a manifest file that lists the strings to be embedded.
    struct Manifest
    {
        std::vector<Entry> entries; ///< The translated strings to embed.
    };

    /// Parse a `.toml` manifest file.
    /// \param path The path to the manifest file.
    /// \return The parsed manifest.
    /// \throws std::runtime_error If the manifest is malformed or requires a newer version of the tool.
    auto parseManifest(std::filesystem::path const& path) -> Manifest;
} // namespace aw::linguist
