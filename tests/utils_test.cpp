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
    EXPECT_EQ(ver, "1.2.3");
    EXPECT_GE(ver, "0.2.3");
    EXPECT_LE(ver, "1.2.3.1");
    EXPECT_GT(ver, "1.2");

    EXPECT_TRUE(ver.from_string("1.2"));
    EXPECT_EQ(ver, "1.2");


    ver.from_safe_string("1.2");
    EXPECT_EQ(ver, "1.2");
}

TEST(BasicVersion, OperatorsTest) {
    basic_version<3, stl::uint16_t> const ver123{stl::uint16_t{1}, 2, 3};

    basic_version ver{};
    ver = ver123;
    EXPECT_EQ(ver, "1.2.3");
    ver += ver123;
    EXPECT_EQ(ver, "2.4.6");

    ver = ver123 + basic_version{2, 1, 1};
    EXPECT_EQ(ver, (basic_version{3, 3, 4}));

    EXPECT_GT(ver, (basic_version{3}));
    EXPECT_LT(ver, (basic_version{4}));
    EXPECT_GT(ver, (basic_version{3, 1}));
    EXPECT_LT(ver, (basic_version{3, 3, 4, 1}));
    EXPECT_EQ(ver, (basic_version{3, 3, 4, 0, 0}));
    EXPECT_TRUE(stl::is_eq(ver <=> basic_version{3, 3, 4, 0, 0}));
}

// NOLINTEND(*-avoid-magic-numbers)
