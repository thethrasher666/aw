//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#pragma once

#include <filesystem>
#include <string>
#include <vector>

namespace aw::rcc
{
    /// A single file to embed, together with the path it is addressed by at runtime.
    struct Resource
    {
        std::string           path;   ///< Canonical resource path, e.g. "/data/file1.bin".
        std::filesystem::path source; ///< Source file, relative to the manifest directory.
    };

    /// Represents a manifest file that lists the resources to be embedded.
    struct Manifest
    {
        std::string           name;      ///< Identifier used to namespace the generated code.
        std::vector<Resource> resources; ///< The resources to embed, in manifest order.
    };

    /// Parse a TOML manifest file. File entries may be strings or tables containing `file` and `alias` strings.
    /// \param path The path to the manifest file.
    /// \return The parsed manifest.
    /// \throws std::runtime_error If the manifest is malformed or requires a newer version of the tool.
    auto parseManifest(std::filesystem::path const& path) -> Manifest;
} // namespace aw::rcc
