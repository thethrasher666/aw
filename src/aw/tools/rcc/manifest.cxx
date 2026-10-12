//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#include "manifest.hxx"
#include "aw/base/version.hxx"

#include <array>
#include <charconv>
#include <regex>
#include <toml++/toml.hpp>

namespace aw::rcc
{
    namespace
    {
        auto trim(std::string value) -> std::string
        {
            auto const first = value.find_first_not_of(" \t\r\n");
            if (first == std::string::npos)
            {
                return {};
            }

            auto const last = value.find_last_not_of(" \t\r\n");
            return value.substr(first, last - first + 1);
        }

        auto parseVersionString(std::string const& text) -> std::optional<std::array<uint32_t, 4>>
        {
            auto trimmed = trim(text);
            if (trimmed.empty())
            {
                return std::nullopt;
            }

            std::smatch match;
            if (!std::regex_match(trimmed, match, std::regex(R"(^([0-9]+)([.]([0-9]+))?([.]([0-9]+))?([.]([0-9]+))?$)")))
            {
                return std::nullopt;
            }

            std::array<uint32_t, 4> version{ 0, 0, 0, 0 };
            for (std::size_t index = 0; index < version.size(); ++index)
            {
                auto const part = match[index * 2 + 1].str();
                if (part.empty())
                {
                    continue;
                }

                uint32_t value = 0;
                auto const [ptr, ec] = std::from_chars(part.data(), part.data() + part.size(), value);
                if (ec != std::errc{} || ptr != part.data() + part.size())
                {
                    return std::nullopt;
                }

                version[index] = value;
            }

            return version;
        }

        auto toOrdinal(std::array<uint32_t, 4> const& version) -> uint32_t
        {
            return (version[0] << 24) | (version[1] << 16) | (version[2] << 8) | (version[3] << 0);
        }

        // Derive the namespace/module name of the generated code from the manifest file name.
        auto toName(std::filesystem::path const& path) -> std::string
        {
            auto name = path.stem().string();
            for (auto& c : name)
            {
                if (!std::isalnum(static_cast<unsigned char>(c)))
                {
                    c = '_';
                }
            }

            if (name.empty() || std::isdigit(static_cast<unsigned char>(name.front())))
                name.insert(name.begin(), '_');

            return name;
        }

        auto stripSlashes(std::string_view value) -> std::string_view
        {
            while (!value.empty() && value.front() == '/')
            {
                value.remove_prefix(1);
            }

            while (!value.empty() && value.back() == '/')
            {
                value.remove_suffix(1);
            }

            return value;
        }

        // Build the canonical resource path, addressed at runtime as ":<path>".
        auto joinResourcePath(std::string_view prefix, std::string_view file) -> std::string
        {
            auto const head = stripSlashes(prefix);
            auto const tail = stripSlashes(file);

            std::string path = "/";
            if (!head.empty())
            {
                path += head;
                path += '/';
            }
            path += tail;

            return path;
        }

        void parseTomlVersion(toml::table const& table, std::filesystem::path const& path)
        {
            auto const* versionNode = table.get("version");
            if (!versionNode)
            {
                return;
            }

            auto const value = versionNode->value<std::string>();
            if (!value)
            {
                throw std::runtime_error("manifest '" + path.string() + "' has non-string version; expected dotted numeric string");
            }

            auto const manifestVersion = parseVersionString(*value);
            if (!manifestVersion)
            {
                throw std::runtime_error("manifest '" + path.string() + "' has invalid version '" + *value + "'");
            }

            auto const toolVersion = std::array<uint32_t, 4>{ aw::base::version::major(), aw::base::version::minor(), aw::base::version::patch(), aw::base::version::tweak() };
            if (toOrdinal(*manifestVersion) > toOrdinal(toolVersion))
            {
                auto const selfVersion = std::to_string(aw::base::version::major()) + "."   //
                                         + std::to_string(aw::base::version::minor()) + "." //
                                         + std::to_string(aw::base::version::patch()) + "." //
                                         + std::to_string(aw::base::version::tweak());
                throw std::runtime_error("manifest '" + path.string() + "' requires newer rcc version '" + *value + "' than this tool ('" + selfVersion + "')");
            }
        }
    } // namespace

    auto parseManifest(std::filesystem::path const& path) -> Manifest
    {
        auto const table = toml::parse_file(path.string());
        parseTomlVersion(table, path);

        Manifest manifest;
        manifest.name = table["name"].value_or(toName(path));

        auto const prefix = table["prefix"].value_or(std::string_view{});

        if (auto const* files = table["files"].as_array())
        {
            for (auto const& elem : *files)
            {
                if (auto const* str = elem.as_string())
                {
                    manifest.resources.push_back(Resource{ joinResourcePath(prefix, str->get()), str->get() });
                }
                else if (auto const* file = elem.as_table())
                {
                    auto const source = (*file)["file"].value<std::string>();
                    if (!source)
                    {
                        throw std::runtime_error("manifest '" + path.string() + "' has file entry without a string file");
                    }

                    auto const alias = (*file)["alias"].value_or(*source);
                    manifest.resources.push_back(Resource{ joinResourcePath(prefix, alias), *source });
                }
            }
        }

        return manifest;
    }
} // namespace aw::rcc
