// Created by moisrex on 2/4/20.

#include "../core/include/webpp/http/bodies/string.hpp"
#include "../core/include/webpp/http/response_body.hpp"
#include "../core/include/webpp/std/string.hpp"
#include "common_pch.hpp"

#include <filesystem>

using namespace webpp;
using namespace webpp::http;
using namespace webpp::details;
using namespace webpp::http::details;


using string_type = traits::general_string<default_traits>;
using body_type   = simple_response_body<default_traits>;

TEST(Body, Text) {
    enable_owner_traits<default_traits> et;
    body_type                           b{et, "Testing"};
    EXPECT_EQ(b.template as<std::string_view>(), "Testing");
    EXPECT_STREQ(b.template as<char const*>(), "Testing") << stl::string_view{b.as<const char*>()};

    // todo
    // EXPECT_TRUE(b == "Testing");

    string_type const str = "hello";
    b                     = str;

    EXPECT_STREQ(b.as<const char*>(), "hello");

    std::string_view const sth = "nice";
    b                          = sth;
    EXPECT_STREQ(b.as<const char*>(), "nice");

    b = string_type("cool");
    EXPECT_EQ(b.as<stl::string_view>(), "cool");

    body_type bt{et};
    {
        string_type            _str = "testing";
        std::string_view const test = _str;
        bt                          = test;
        EXPECT_STREQ(bt.as(), test.data());
        _str = "";
    }
    // EXPECT_NE(bt.string(), "testing") << "The test should be empty since it was a string_view and not a
    // string";
}

TEST(Body, File) {
    enable_owner_traits<default_traits> et;
    std::filesystem::path               file = std::filesystem::temp_directory_path();
    file.append("webpp_test_file");
    std::ofstream handle{file};
    handle << "Hello World";
    handle.close();

    std::ifstream const in{file};
    std::stringstream   buf;
    buf << in.rdbuf();
    auto const file_out = buf.str();

    ASSERT_EQ(file_out, "Hello World");

    // so the file is okay

    body_type the_body{et};
    the_body = "data";
    EXPECT_STREQ(the_body.as<char const*>(), "data");
    // ASSERT_TRUE(the_body.load(file));
    // EXPECT_EQ(the_body.as(), "Hello World");
    std::filesystem::remove(file);
}


TEST(Body, StringCustomBody) {
    enable_owner_traits<default_traits> et;
    static_assert(istl::String<stl::string> && stl::is_default_constructible_v<stl::string>,
                  "We need string to be default constructible for this test to work.");
    body_type body{et, "Testing"};
    body                       = "Hello World";
    stl::string const body_str = body.as();
    EXPECT_EQ(body_str, "Hello World");
    body_type body2{et};
    body2          = "Hello World";
    auto body_str2 = body2.template as<stl::string>();
    EXPECT_EQ(body_str, body_str2);
    body_str2 = "nice";
    body2     = body_str2;
    body_str2 = body2.template as<stl::string>();
    EXPECT_EQ("nice", body_str2);
}


struct custom_body_type {};

template <typename T, HTTPBody BodyT>
    requires(stl::same_as<stl::remove_cvref_t<T>, custom_body_type>)
void serialize_body(T&&, BodyT& body) {
    body = "custom body type";
}

TEST(Body, CustomBodyTypeSerializerTest) {
    enable_owner_traits<default_traits> et;
    body_type                           body{et};
    body = custom_body_type{};
    EXPECT_EQ(body.as<stl::string_view>(), "custom body type");
}
