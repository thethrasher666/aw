//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#include <catch2/catch_test_macros.hpp>

#include <aw/core/application.hxx>
#include <aw/core/resource.hxx>

namespace test
{
    TEST_CASE("core.CoreApplication.Receives arguments")
    {
        std::vector<std::string> arguments = { "./test", "--version" };

        aw::core::CoreApplication app(arguments);

        REQUIRE(app.arguments().size() == arguments.size());
    }
} // namespace test
