//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#include <catch2/catch_test_macros.hpp>

#include <filesystem>
#include <aw/base/qoi.hxx>

namespace aw::test
{
    TEST_CASE("base.QoiImage.Can load")
    {
        const std::filesystem::path path = TEST_ASSET_PATH;
        aw::base::QoiImage         qoiImage;

        REQUIRE(qoiImage.read(path));

        REQUIRE(qoiImage.bitDepth() == 8);
        REQUIRE(qoiImage.data());
        REQUIRE(qoiImage.height() == 100);
        REQUIRE(qoiImage.length() == 40000);
        REQUIRE(qoiImage.width() == 100);
    }
} // namespace aw::test
