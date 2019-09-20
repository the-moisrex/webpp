#include <gtest/gtest.h>
#include <webpp/utils/uri.h>

using namespace webpp;

TEST(URITests, Creation) {
    uri u("http://example.com/");
    EXPECT_TRUE(u.has_scheme());
}
