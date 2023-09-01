// Created by moisrex on 8/31/23.

#include "../webpp/utils/version.hpp"
#include "common/tests_common_pch.hpp"

using namespace webpp;
// NOLINTBEGIN(*-avoid-magic-numbers)

TEST(BasicVersion, Concepts) {
    EXPECT_TRUE(stl::regular<basic_version<>>);
}

TEST(BasicVersion, SimpleUsage) {
    basic_version ver{1, 2, 3};
    EXPECT_EQ(ver[0], 1);
    EXPECT_EQ(ver[1], 2);
    EXPECT_EQ(ver[2], 3);
    ver[2] = 5;
    EXPECT_EQ(ver[2], 5);

    ver = stl::uint16_t{7};
    EXPECT_EQ(ver[2], 7);
}


TEST(BasicVersion, FromString) {
    basic_version<3, stl::uint16_t> const ver123{stl::uint16_t{1}, 2, 3};

    basic_version ver{};
    EXPECT_TRUE(ver.from_string("1.2.3"));
    EXPECT_EQ(ver, ver123);
}

// NOLINTEND(*-avoid-magic-numbers)
