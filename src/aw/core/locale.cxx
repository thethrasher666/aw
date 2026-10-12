//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#include "aw/core/locale.hxx"
#include "platform/locale.hxx"

namespace aw::core
{
    Locale::Locale() : _locale(platform::platformLocale())
    {

    }

    auto Locale::name() const -> std::string const&
    {
        return _locale;
    }
} // namespace aw::core
