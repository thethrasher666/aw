//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#include <catch2/catch_test_macros.hpp>

#include <aw/gui/application.hxx>

namespace test
{
    TEST_CASE("gui.GuiApplication.Arguments")
    {
        std::vector<std::string> arguments = { "./test", "--version" };

        aw::gui::GuiApplication app(arguments);

        REQUIRE(app.arguments().size() == arguments.size());
    }
} // namespace test
