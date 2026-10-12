//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#pragma once

#include "manifest.hxx"

#include <filesystem>
#include <memory>
#include <optional>
#include <string>
#include <string_view>

namespace aw::rcc
{
    /// The output languages the generator can emit.
    enum class Language
    {
        Cpp,   ///< A C++ translation unit.
        Python ///< A Python module.
    };

    /// Parse a generator name as accepted on the command line.
    /// \param name The generator name, e.g. "cpp" or "python".
    /// \return The matching language, or nullopt if the name is not recognised.
    [[nodiscard]] auto parseLanguage(std::string_view name) -> std::optional<Language>;

    /// Generate the resource bundle for a manifest.
    /// \param manifest The manifest describing the resources to embed.
    /// \param baseDir The base directory used to resolve resource source paths.
    /// \param language The language to emit.
    /// \return The generated code, or nullopt if a resource could not be embedded.
    auto generate(Manifest const& manifest, std::filesystem::path const& baseDir, Language language) -> std::unique_ptr<std::string>;
} // namespace aw::rcc
