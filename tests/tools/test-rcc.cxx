//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#include <catch2/catch_test_macros.hpp>

#include <aw/tools/rcc/generator.hxx>
#include <aw/tools/rcc/manifest.hxx>

#include <filesystem>
#include <fstream>

namespace aw::test
{
    namespace
    {
        // The manifest lives alongside the sample "data" directory used as the resource base directory.
        auto dataBaseDir() -> std::filesystem::path
        {
            return std::filesystem::path(TEST_MANIFEST_FILE).parent_path();
        }
    } // namespace

    TEST_CASE("tools.rcc.Parse language")
    {
        CHECK(rcc::parseLanguage("cpp") == rcc::Language::Cpp);
        CHECK(rcc::parseLanguage("python") == rcc::Language::Python);
        CHECK_FALSE(rcc::parseLanguage("ruby").has_value());
    }

    TEST_CASE("tools.rcc.Parses file aliases")
    {
        auto const path = std::filesystem::temp_directory_path() / "aw-rcc-alias-test.toml";
        {
            std::ofstream manifest(path);
            manifest << "prefix = \"images\"\n"
                        "files = [{ file = \"data/file1.bin\", alias = \"application.bin\" }]\n";
        }

        auto const manifest = rcc::parseManifest(path);
        std::filesystem::remove(path);

        REQUIRE(manifest.resources.size() == 1);
        CHECK(manifest.resources[0].path == "/images/application.bin");
        CHECK(manifest.resources[0].source == "data/file1.bin");
    }

    TEST_CASE("tools.rcc.Generate cpp embeds resources sorted by path")
    {
        rcc::Manifest manifest;
        manifest.resources = {
            { "/data/file2.bin", "data/file2.bin" },
            { "/data/file1.bin", "data/file1.bin" },
        };

        auto const code = rcc::generate(manifest, dataBaseDir(), rcc::Language::Cpp);
        REQUIRE(code);

        CHECK(code->find("namespace resources") != std::string::npos);
        CHECK(code->find("std::array<Resource, 2> all") != std::string::npos);
        CHECK(code->find("std::array<std::uint8_t, 394> resource0") != std::string::npos); // file1.bin
        CHECK(code->find("std::array<std::uint8_t, 385> resource1") != std::string::npos); // file2.bin

        // Sorted by path regardless of manifest order, so file1 is resource0 and file2 is resource1.
        CHECK(code->find("{ \"/data/file1.bin\", detail::resource0 }") != std::string::npos);
        CHECK(code->find("{ \"/data/file2.bin\", detail::resource1 }") != std::string::npos);
    }

    TEST_CASE("tools.rcc.Generate python embeds resources")
    {
        rcc::Manifest manifest;
        manifest.resources = { { "/data/file1.bin", "data/file1.bin" } };

        auto const code = rcc::generate(manifest, dataBaseDir(), rcc::Language::Python);
        REQUIRE(code);

        CHECK(code->find("RESOURCES: dict[str, bytes]") != std::string::npos);
        CHECK(code->find("\"/data/file1.bin\": bytes.fromhex(") != std::string::npos);
        CHECK(code->find("def find(path: str) -> bytes | None:") != std::string::npos);
    }

    TEST_CASE("tools.rcc.Generate handles an empty manifest")
    {
        rcc::Manifest const manifest;

        auto const cppCode = rcc::generate(manifest, dataBaseDir(), rcc::Language::Cpp);
        REQUIRE(cppCode);
        CHECK(cppCode->find("std::array<Resource, 0> all{};") != std::string::npos);

        auto const pythonCode = rcc::generate(manifest, dataBaseDir(), rcc::Language::Python);
        REQUIRE(pythonCode);
        CHECK(pythonCode->find("RESOURCES: dict[str, bytes] = {\n}\n") != std::string::npos);
    }

    TEST_CASE("tools.rcc.Generate collapses a resource claimed twice by the same source")
    {
        rcc::Manifest manifest;
        manifest.resources = {
            { "/data/file1.bin", "data/file1.bin" },
            { "/data/file1.bin", "data/file1.bin" },
        };

        auto const code = rcc::generate(manifest, dataBaseDir(), rcc::Language::Cpp);
        REQUIRE(code);
        CHECK(code->find("std::array<Resource, 1> all") != std::string::npos);
    }

    TEST_CASE("tools.rcc.Generate fails when a path is claimed by two different sources")
    {
        rcc::Manifest manifest;
        manifest.resources = {
            { "/data/shared.bin", "data/file1.bin" },
            { "/data/shared.bin", "data/file2.bin" },
        };

        CHECK_FALSE(rcc::generate(manifest, dataBaseDir(), rcc::Language::Cpp));
    }

    TEST_CASE("tools.rcc.Generate fails when a source file does not exist")
    {
        rcc::Manifest manifest;
        manifest.resources = { { "/data/missing.bin", "data/does-not-exist.bin" } };

        CHECK_FALSE(rcc::generate(manifest, dataBaseDir(), rcc::Language::Cpp));
    }
} // namespace aw::test
