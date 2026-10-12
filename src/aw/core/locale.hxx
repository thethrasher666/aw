//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#pragma once

#include <aw/core/api.hxx>

#include <string>

namespace aw::core
{
    /// Represents a locale for internationalization and localization purposes.
    class Locale
    {
    public:
        /// Constructor - uses the system's default locale.
        AW_CORE_API Locale();

        /// Constructor.
        /// \param locale The locale string (e.g., "en-US", "fr-FR").
        AW_CORE_API explicit Locale(std::string_view locale)
        {
            _locale = locale;
        }

        /// Get the locale string.
        /// \return The locale string (e.g., "en-US", "fr-FR").
        [[nodiscard]] AW_CORE_API auto name() const -> std::string const&;

        /// Equality operator to compare two Locale objects.
        /// \return true if the locales are equal, false otherwise.
        [[nodiscard]] bool operator==(Locale const& rhs) const
        {
            return _locale == rhs._locale;
        }

        /// Equality operator to compare two locales.
        /// \return true if the locales are equal, false otherwise.
        [[nodiscard]] bool operator==(std::string_view const& rhs) const
        {
            return _locale == rhs;
        }

    private:
        std::string _locale; ///< The locale string (e.g., "en-US", "fr-FR").
    };
} // namespace aw::core
