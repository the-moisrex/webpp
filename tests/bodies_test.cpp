// Created by moisrex on 2/4/20.

#include "../webpp/http/bodies/string.hpp"
#include "../webpp/http/response_body.hpp"
#include "../webpp/std/string.hpp"
#include "common_pch.hpp"

#include <filesystem>

using namespace webpp;
using namespace webpp::http;
using namespace webpp::details;
using namespace webpp::http::details;


using string_type = traits::general_string<default_traits>;
using body_type   = response_body<default_traits>;

TEST(Body, Concepts) {
    EXPECT_TRUE(bool(BodyReader<body_reader<default_traits>>));
    EXPECT_TRUE(bool(BodyReader<body_writer<default_traits>>));
    EXPECT_TRUE(bool(BodyWriter<body_writer<default_traits>>));
}

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
    EXPECT_EQ(as<std::string>(body), "Testing");
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

void serialize_body(custom_body_type, HTTPBody auto& body) {
    body = "custom body type";
}

TEST(Body, CustomBodyTypeSerializerTest) {
    enable_owner_traits<default_traits> et;
    body_type                           body{et};
    body = custom_body_type{};
    EXPECT_EQ(body.as<stl::string_view>(), "custom body type");
}


// NOLINTBEGIN(cppcoreguidelines-pro-type-reinterpret-cast)
////////////////////////////////////////////////// Read & Write //////////////////////////////////////////////////

TEST(Body, BodyStreamToStream) {
    enable_owner_traits<default_traits> et;
    body_type                           body{et};
    stl::string const                   str = "one two three";
    body << str;
    stl::string one, two, three;
    body >> one >> two >> three;
    EXPECT_EQ("one", one);
    EXPECT_EQ("two", two);
    EXPECT_EQ("three", three);

    stl::stringstream whole_str;
    body.seekg(0);
    whole_str << body.rdbuf();
    EXPECT_EQ(str, whole_str.str());
}

TEST(Body, BodyCStreamToCStream) {
    enable_owner_traits<default_traits> et;
    body_type                           body{et};
    stl::string const                   str = "this is a test";
    body.write(reinterpret_cast<stl::byte const*>(str.data()), static_cast<stl::streamsize>(str.size()));
    stl::string                 str2;
    static constexpr auto       buff_size = 10;
    stl::array<char, buff_size> buf{};
    while (auto res =
             body.read(reinterpret_cast<stl::byte*>(buf.data()), static_cast<stl::streamsize>(buf.size()))) {
        str2.append(buf.data(), static_cast<stl::size_t>(res));
    }
    EXPECT_EQ(str, str2);
}


TEST(Body, BodyTextToText) {
    enable_owner_traits<default_traits> et;
    body_type                           body{et};
    stl::string const                   str = "this is a test";
    body.append(str.data(), str.size());
    stl::string const str2{body.data(), body.size()};
    EXPECT_EQ(str, str2);
}

////////////////////////////////////////////////// Cross Talk //////////////////////////////////////////////////

TEST(Body, BodyCrossTalkCStreamToText) {
    enable_owner_traits<default_traits> et;
    body_type                           body{et};
    stl::string const                   str = "this is a test";
    body.write(reinterpret_cast<stl::byte const*>(str.data()), static_cast<stl::streamsize>(str.size()));
    ASSERT_NE(body.data(), nullptr);
    stl::string const str2{body.data(), body.size()};
    EXPECT_EQ(str, str2);
}

TEST(Body, BodyCrossTalkCStreamToStream) {
    enable_owner_traits<default_traits> et;
    body_type                           body{et};
    stl::string const                   str = "this is a test";
    body.write(reinterpret_cast<stl::byte const*>(str.data()), static_cast<stl::streamsize>(str.size()));
    stl::string str2;
    body >> str2;
    EXPECT_EQ(str, str2);
}

TEST(Body, BodyCrossTalkTextToStream) {
    enable_owner_traits<default_traits> et;
    body_type                           body{et};
    stl::string const                   str = "this is a test";
    body.append(str.data(), str.size());
    stl::string str2;
    body >> str2;
    EXPECT_EQ(str, str2);
}

TEST(Body, BodyCrossTalkTextToCStream) {
    enable_owner_traits<default_traits> et;
    body_type                           body{et};
    stl::string const                   str = "this is a test";
    body.append(str.data(), str.size());
    stl::string                 str2;
    static constexpr auto       buff_size = 10;
    stl::array<char, buff_size> buf{};
    while (auto res =
             body.read(reinterpret_cast<stl::byte*>(buf.data()), static_cast<stl::streamsize>(buf.size()))) {
        str2.append(buf.data(), static_cast<stl::size_t>(res)); // res is always zero to not create a loop!!!
    }
    EXPECT_EQ(str, str2);
}

TEST(Body, BodyCrossTalkStreamToCStream) {
    enable_owner_traits<default_traits> et;
    body_type                           body{et};
    stl::string const                   str = "this is a test";
    body << str;
    stl::string                 str2;
    static constexpr auto       buff_size = 10;
    stl::array<char, buff_size> buf{};
    while (auto res =
             body.read(reinterpret_cast<stl::byte*>(buf.data()), static_cast<stl::streamsize>(buf.size()))) {
        str2.append(buf.data(), static_cast<stl::size_t>(res));
    }
    EXPECT_EQ(str, str2);
}

// NOLINTEND(cppcoreguidelines-pro-type-reinterpret-cast)
