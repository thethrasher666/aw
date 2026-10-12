//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#include "aw/core/resource.hxx"

#include <algorithm>
#include <iterator>
#include <mutex>
#include <vector>

namespace aw::core
{
    namespace
    {
        auto registry() -> std::vector<std::span<Resource const>>&
        {
            static std::vector<std::span<Resource const>> tables;
            return tables;
        }

        auto registryMutex() -> std::mutex&
        {
            static std::mutex mutex;
            return mutex;
        }

        auto normalizedPath(std::string_view path) -> std::string_view
        {
            if (path.starts_with(':'))
            {
                path.remove_prefix(1);
            }
            return path;
        }

        auto findInTable(std::span<Resource const> resources, std::string_view path) -> std::optional<Resource>
        {
            auto const entry = std::lower_bound(resources.begin(), resources.end(), path, [](Resource const& lhs, std::string_view rhs) { return lhs.path < rhs; });
            if (entry == resources.end() || entry->path != path)
            {
                return std::nullopt;
            }
            return *entry;
        }
    } // namespace

    auto findResource(std::string_view path) -> std::optional<Resource>
    {
        path = normalizedPath(path);

        std::lock_guard lock(registryMutex());
        auto const&     tables = registry();
        for (auto entry = tables.rbegin(); entry != tables.rend(); ++entry)
        {
            if (auto resource = findInTable(*entry, path))
            {
                return resource;
            }
        }
        return std::nullopt;
    }

    auto registerResourceData(int version, std::span<Resource const> resources) -> bool
    {
        if (version != 1)
        {
            return false;
        }

        std::lock_guard lock(registryMutex());
        registry().push_back(resources);
        return true;
    }

    auto unregisterResourceData(int version, std::span<Resource const> resources) -> bool
    {
        if (version != 1)
        {
            return false;
        }

        std::lock_guard lock(registryMutex());
        auto&           tables = registry();
        auto const      entry =
        std::find_if(tables.rbegin(), tables.rend(), [resources](auto const table) { return table.data() == resources.data() && table.size() == resources.size(); });
        if (entry == tables.rend())
        {
            return false;
        }

        tables.erase(std::next(entry).base());
        return true;
    }
} // namespace aw::core

extern "C" void awRegisterResourceData(char const*, void const* data, uint64_t length)
{
    if (data == nullptr || length % sizeof(aw::core::Resource) != 0)
    {
        return;
    }

    auto const resources = std::span(reinterpret_cast<aw::core::Resource const*>(data), length / sizeof(aw::core::Resource));
    (void) aw::core::registerResourceData(1, resources);
}

extern "C" void awUnregisterResourceData(char const*, void const* data, uint64_t length)
{
    if (data == nullptr || length % sizeof(aw::core::Resource) != 0)
    {
        return;
    }

    auto const resources = std::span(reinterpret_cast<aw::core::Resource const*>(data), length / sizeof(aw::core::Resource));
    (void) aw::core::unregisterResourceData(1, resources);
}
