//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#include <catch2/catch_test_macros.hpp>

#include <aw/widgets/application.hxx>

namespace test
{
    TEST_CASE("widgets.WidgetsApplication.Arguments")
    {
        std::vector<std::string> arguments = { "./test", "--version" };

        aw::widgets::WidgetsApplication app(arguments);

        REQUIRE(app.arguments().size() == arguments.size());
    }
} // namespace test
