//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#include <catch2/catch_test_macros.hpp>

#include <aw/core/translation.hxx>

#include <array>

namespace test
{
    TEST_CASE("core.CoreApplication.Register/find/unregister")
    {
        aw::core::Locale                            localeEnglish("en-US");
        aw::core::Locale                            localeFrench("fr-FR");
        aw::core::Locale                            localeSpanish("es-ES");
        static std::array<aw::core::Translation, 3> table{ {
        { "button.save", aw::core::Locale("en-US"), "Save" },
        { "button.save", aw::core::Locale("fr-FR"), "Enregistrer" },
        { "home.title", aw::core::Locale("en-US"), "Home" },
        } };

        CHECK_FALSE(aw::core::findTranslation(localeEnglish, "button.save").has_value());

        REQUIRE(aw::core::registerTranslationData(1, table));

        auto const save = aw::core::findTranslation(localeEnglish, "button.save");
        REQUIRE(save.has_value());
        CHECK(std::string(*save) == "Save");

        auto const saveFr = aw::core::findTranslation(localeFrench, "button.save");
        REQUIRE(saveFr.has_value());
        CHECK(std::string(*saveFr) == "Enregistrer");

        CHECK_FALSE(aw::core::findTranslation(localeSpanish, "button.save").has_value());
        CHECK_FALSE(aw::core::findTranslation(localeEnglish, "missing.key").has_value());

        REQUIRE(aw::core::unregisterTranslationData(1, table));
        CHECK_FALSE(aw::core::findTranslation(localeEnglish, "button.save").has_value());
    }

    TEST_CASE("core.CoreApplication.Register/unregister reject unsupported versions")
    {
        static std::array<aw::core::Translation, 0> table{};

        CHECK_FALSE(aw::core::registerTranslationData(2, table));
        CHECK_FALSE(aw::core::unregisterTranslationData(2, table));
    }
} // namespace test
