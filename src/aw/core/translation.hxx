//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#pragma once

#include <aw/core/locale.hxx>

#include <cstdint>
#include <optional>
#include <span>
#include <string>
#include <string_view>

namespace aw::core
{
    /// A single translated string, addressed by key and locale.
    struct Translation
    {
        std::string_view key;    ///< The key of the string, e.g. "home.title"
        Locale           locale; ///< The locale the text is translated for, e.g. "en-US"
        std::string_view text;   ///< The translated text.
    };

    /// Look up a registered translation by locale and key.
    /// \param locale The locale to translate for, e.g. "en-US"
    /// \param key The key of the string to translate.
    /// \return An optional containing the translated text if found, or std::nullopt if not found.
    [[nodiscard]] AW_CORE_API auto findTranslation(Locale const& locale, std::string_view key) -> std::optional<std::string_view>;

    /// Register a generated translation table with the Core translation system.
    /// \param version The version of the translation table.
    /// \param translations The translations to register.
    /// \return true if the translations were registered successfully, false otherwise.
    [[nodiscard]] AW_CORE_API auto registerTranslationData(uint32_t version, std::span<Translation const> translations) -> bool;

    /// Remove a generated translation table from the Core translation system.
    /// \param version The version of the translation table.
    /// \param translations The translations to unregister.
    /// \return true if the translations were unregistered successfully, false otherwise.
    [[nodiscard]] AW_CORE_API auto unregisterTranslationData(uint32_t version, std::span<Translation const> translations) -> bool;

} // namespace aw::core

/// Register a generated translation table with the Core translation system.
/// \param name The name of the translation table.
/// \param data The raw bytes of the translation table.
/// \param length The length of the translation table in bytes.
extern "C" AW_CORE_API void awRegisterTranslationData(char const* name, void const* data, uint64_t length);

/// Unregister a generated translation table with the Core translation system.
/// \param name The name of the translation table.
/// \param data The raw bytes of the translation table.
/// \param length The length of the translation table in bytes.
extern "C" AW_CORE_API void awUnregisterTranslationData(char const* name, void const* data, uint64_t length);
