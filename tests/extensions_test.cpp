#include "../core/include/webpp/extensions/extension.h"
#include "../core/include/webpp/std/concepts.h"

#include <gtest/gtest.h>

using namespace webpp;

TEST(ExtensionsTests, ExtensionConcepts) {
    EXPECT_FALSE(static_cast<bool>(Extension<int>));

    EXPECT_TRUE(static_cast<bool>(Extension<fake_extension>));
    EXPECT_TRUE(static_cast<bool>(
      ExtensionList<::std::tuple<fake_extension, fake_extension>>));
}
