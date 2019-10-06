#include <gtest/gtest.h>
#include <webpp/utils/uri.h>

using namespace webpp;

TEST(URITests, Creation) {
    uri u("http://example.com/");
    EXPECT_TRUE(u.has_scheme());
    EXPECT_TRUE(u.has_host());
    EXPECT_EQ(u.str(), "http://example.com/");
    EXPECT_EQ(u.host_string().value_or(""), "example.com");
    EXPECT_TRUE(is::host(u.str()));
    EXPECT_TRUE(u.has_authority());
    EXPECT_TRUE(u.has_path()) << "the path is '/'";
    EXPECT_FALSE(u.has_port());
    EXPECT_FALSE(u.has_user_info());
    EXPECT_FALSE(u.has_fragment());
    EXPECT_FALSE(u.has_query());
    EXPECT_EQ(u.path_structured().size(), 1);
    EXPECT_EQ(u.scheme().value_or(""), "http");
    u.clear_scheme();
    EXPECT_FALSE(u.has_scheme());
    EXPECT_EQ(u.scheme().value_or(""), "");
    EXPECT_TRUE(u.has_authority());
    EXPECT_TRUE(u.has_host());
    EXPECT_TRUE(u.has_path());
    EXPECT_FALSE(u.has_port());
    EXPECT_EQ(u.str(), "//example.com/");

    uri ipv4_host("https://192.168.1.1");
    EXPECT_TRUE(is::ipv4(ipv4_host.host_string().value_or("")));
    EXPECT_EQ(ipv4_host.scheme().value_or(""), "https");
    EXPECT_FALSE(ipv4_host.has_path());
    EXPECT_FALSE(ipv4_host.has_query());
    EXPECT_FALSE(ipv4_host.has_port());
    EXPECT_TRUE(ipv4_host.has_authority());
    EXPECT_TRUE(ipv4_host.has_host());
    EXPECT_EQ(ipv4_host.host_string().value_or(""), "192.168.1.1");

    uri local_file("file:///home/test/folder/file.txt");
    EXPECT_EQ(local_file.path().value_or(""), "/home/test/folder/file.txt");
    EXPECT_TRUE(local_file.has_path());
    EXPECT_TRUE(local_file.has_scheme());
    EXPECT_FALSE(local_file.has_authority());
    EXPECT_FALSE(local_file.has_host());
    EXPECT_EQ(local_file.scheme().value_or(""), "file");
    EXPECT_EQ(local_file.host_decoded_str(), "");
    auto path = local_file.path_structured_decoded();
    EXPECT_EQ(local_file.path_structured().size(), 5);
    EXPECT_EQ(path.size(), 5);
    EXPECT_EQ(path.at(0), "");
    EXPECT_EQ(path.at(1), "home");
    EXPECT_EQ(path.at(2), "test");
    EXPECT_EQ(path.at(3), "folder");
    EXPECT_EQ(path.at(4), "file.txt");
}
