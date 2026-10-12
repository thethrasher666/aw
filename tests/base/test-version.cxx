//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#include <catch2/catch_test_macros.hpp>

#include <aw/base/version.hxx>

namespace aw::test
{
    TEST_CASE("base.Version.Is expected")
    {
        REQUIRE(aw::base::version::major() == 0);
        REQUIRE(aw::base::version::minor() == 0);
        REQUIRE(aw::base::version::patch() == 1);
        REQUIRE(aw::base::version::tweak() == 0);
    }
} // namespace aw::test
