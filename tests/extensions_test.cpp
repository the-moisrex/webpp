#include "../core/include/webpp/extensions/extension.h"

#include <gtest/gtest.h>

using namespace webpp;

struct fake_extension {};

TEST(ExtensionsTests, ExtensionConcepts) {
    EXPECT_FALSE(Extension<int>);
    EXPECT_FALSE(Extension<::std::tuple<int, int>>);
    EXPECT_FALSE(ExtensionList<::std::tuple<int, int>>);

    EXPECT_TRUE(Extension<fake_extension>);
    EXPECT_FALSE(Extension<::std::tuple<fake_extension, fake_extension>>);
    EXPECT_TRUE(ExtensionList<::std::tuple<fake_extension, fake_extension>>);
}
