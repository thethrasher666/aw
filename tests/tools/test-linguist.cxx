//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#include <catch2/catch_test_macros.hpp>

#include <aw/tools/linguist/generator.hxx>

namespace aw::test
{
    TEST_CASE("tools.linguist.Generate embeds entries sorted by key then locale")
    {
        linguist::Manifest manifest;
        manifest.entries = {
            { "home.title", "fr-FR", "Accueil" },
            { "button.save", "en-US", "Save" },
            { "home.title", "en-US", "Home" },
        };

        auto const code = linguist::generate(manifest);
        REQUIRE(code);

        CHECK(code->find("namespace translations") != std::string::npos);
        CHECK(code->find("std::array<Translation, 3> all") != std::string::npos);

        // Sorted by (key, locale) regardless of manifest order.
        auto const buttonPos = code->find("{ \"button.save\", aw::core::Locale(\"en-US\"), \"Save\" }");
        auto const homeEnPos = code->find("{ \"home.title\", aw::core::Locale(\"en-US\"), \"Home\" }");
        auto const homeFrPos = code->find("{ \"home.title\", aw::core::Locale(\"fr-FR\"), \"Accueil\" }");
        REQUIRE(buttonPos != std::string::npos);
        REQUIRE(homeEnPos != std::string::npos);
        REQUIRE(homeFrPos != std::string::npos);
        CHECK(buttonPos < homeEnPos);
        CHECK(homeEnPos < homeFrPos);
    }

    TEST_CASE("tools.linguist.Generate escapes quotes and backslashes in text")
    {
        linguist::Manifest manifest;
        manifest.entries = { { "quote.example", "en-US", R"(Say "hi" \ bye)" } };

        auto const code = linguist::generate(manifest);
        REQUIRE(code);

        std::string const expected = R"("Say \"hi\" \\ bye")";
        CHECK(code->find(expected) != std::string::npos);
    }

    TEST_CASE("tools.linguist.Generate handles an empty manifest")
    {
        linguist::Manifest const manifest;

        auto const code = linguist::generate(manifest);
        REQUIRE(code);
        CHECK(code->find("std::array<Translation, 0> all{};") != std::string::npos);
    }

    TEST_CASE("tools.linguist.Generate collapses an entry claimed twice with identical text")
    {
        linguist::Manifest manifest;
        manifest.entries = {
            { "home.title", "en-US", "Home" },
            { "home.title", "en-US", "Home" },
        };

        auto const code = linguist::generate(manifest);
        REQUIRE(code);
        CHECK(code->find("std::array<Translation, 1> all") != std::string::npos);
    }

    TEST_CASE("tools.linguist.Generate fails when a key/locale is claimed with different text")
    {
        linguist::Manifest manifest;
        manifest.entries = {
            { "home.title", "en-US", "Home" },
            { "home.title", "en-US", "Dashboard" },
        };

        CHECK_FALSE(linguist::generate(manifest));
    }
} // namespace aw::test
