#include <gtest/gtest.h>
#include <webpp/utils/uri.h>

using namespace webpp;

TEST(URITests, Creation) {
    uri u("http://example.com/");
    EXPECT_TRUE(u.has_scheme());
    EXPECT_TRUE(u.has_host());
    EXPECT_TRUE(is::host(u.str()));
    EXPECT_TRUE(u.has_authority());
    EXPECT_TRUE(u.has_path()) << "the path is '/'";
    EXPECT_FALSE(u.has_port());
    EXPECT_FALSE(u.has_user_info());
    EXPECT_FALSE(u.has_fragment());
    EXPECT_FALSE(u.has_query());
    EXPECT_EQ(u.path_structured().size(), 1);
    EXPECT_EQ(u.scheme().value_or(""), "http");

    uri ipv4_host("https://192.168.1.1");
    EXPECT_TRUE(is::ipv4(ipv4_host.host_string().value_or("")));
    EXPECT_EQ(ipv4_host.scheme().value_or(""), "https");
}
