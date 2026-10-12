//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#include "aw/core/translation.hxx"

#include <algorithm>
#include <iterator>
#include <mutex>
#include <vector>

namespace aw::core
{
    namespace
    {
        auto registry() -> std::vector<std::span<Translation const>>&
        {
            static std::vector<std::span<Translation const>> tables;
            return tables;
        }

        auto registryMutex() -> std::mutex&
        {
            static std::mutex mutex;
            return mutex;
        }

        auto findInTable(std::span<Translation const> translations, Locale const& locale, std::string_view key) -> std::optional<std::string_view>
        {
            auto const first = std::lower_bound(translations.begin(), translations.end(), key, [](Translation const& lhs, std::string_view rhs) { return lhs.key < rhs; });
            auto const last = std::upper_bound(translations.begin(), translations.end(), key, [](std::string_view lhs, Translation const& rhs) { return lhs < rhs.key; });

            for (auto entry = first; entry != last; ++entry)
            {
                if (entry->locale == locale)
                {
                    return entry->text;
                }
            }
            return std::nullopt;
        }
    } // namespace

    auto findTranslation(Locale const& locale, std::string_view key) -> std::optional<std::string_view>
    {
        std::lock_guard lock(registryMutex());
        auto const&     tables = registry();
        for (auto entry = tables.rbegin(); entry != tables.rend(); ++entry)
        {
            if (auto text = findInTable(*entry, locale, key))
            {
                return text;
            }
        }
        return std::nullopt;
    }

    auto registerTranslationData(uint32_t version, std::span<Translation const> translations) -> bool
    {
        if (version != 1)
        {
            return false;
        }

        std::lock_guard lock(registryMutex());
        registry().push_back(translations);
        return true;
    }

    auto unregisterTranslationData(uint32_t version, std::span<Translation const> translations) -> bool
    {
        if (version != 1)
        {
            return false;
        }

        std::lock_guard lock(registryMutex());
        auto&           tables = registry();
        auto const      entry =
        std::find_if(tables.rbegin(), tables.rend(), [translations](auto const table) { return table.data() == translations.data() && table.size() == translations.size(); });
        if (entry == tables.rend())
        {
            return false;
        }

        tables.erase(std::next(entry).base());
        return true;
    }
} // namespace aw::core

extern "C" void awRegisterTranslationData(char const*, void const* data, uint64_t length)
{
    if (data == nullptr || length % sizeof(aw::core::Translation) != 0)
    {
        return;
    }

    auto const translations = std::span(reinterpret_cast<aw::core::Translation const*>(data), length / sizeof(aw::core::Translation));
    std::ignore = aw::core::registerTranslationData(1, translations);
}

extern "C" void awUnregisterTranslationData(char const*, void const* data, uint64_t length)
{
    if (data == nullptr || length % sizeof(aw::core::Translation) != 0)
    {
        return;
    }

    auto const translations = std::span(reinterpret_cast<aw::core::Translation const*>(data), length / sizeof(aw::core::Translation));
    std::ignore = aw::core::unregisterTranslationData(1, translations);
}
