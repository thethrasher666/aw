//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#pragma once

#include <aw/core/api.hxx>

#include <cstdint>
#include <optional>
#include <span>
#include <string_view>

namespace aw::core
{
    /// An embedded file and the path it is addressed by.
    struct Resource
    {
        std::string_view              path; ///< The path to the resource, e.g. ":textures/brick.qoi"
        std::span<std::uint8_t const> data; ///< The raw bytes of the resource file.
    };

    /// Look up a registered resource by path. A leading ':' is accepted.
    /// \param path The path to the resource, e.g. ":textures/brick.qoi"
    /// \return An optional containing the resource if found, or std::nullopt if not found.
    [[nodiscard]] AW_CORE_API auto findResource(std::string_view path) -> std::optional<Resource>;

    /// Register a generated resource table with the Core resource system.
    /// \param version The version of the resource table.
    /// \param resources The resources to register.
    /// \return true if the resources were registered successfully, false otherwise.
    [[nodiscard]] AW_CORE_API auto registerResourceData(uint32_t version, std::span<Resource const> resources) -> bool;

    /// Remove a generated resource table from the Core resource system.
    /// \param version The version of the resource table.
        /// \param resources The resources to unregister.
    /// \return true if the resources were unregistered successfully, false otherwise.
    [[nodiscard]] AW_CORE_API auto unregisterResourceData(uint32_t version, std::span<Resource const> resources) -> bool;
} // namespace aw::core

/// Register a generated resource table with the Core resource system.
/// \param name The name of the resource table.
/// \param data The raw bytes of the resource table.
/// \param length The length of the resource table in bytes.
extern "C" AW_CORE_API void awRegisterResourceData(char const* name, void const* data, uint64_t length);

/// Unregister a generated resource table with the Core resource system.
/// \param name The name of the resource table.
/// \param data The raw bytes of the resource table.
/// \param length The length of the resource table in bytes.
extern "C" AW_CORE_API void awUnregisterResourceData(char const* name, void const* data, uint64_t length);
