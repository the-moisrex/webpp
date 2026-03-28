// Created by moisrex on 2/4/20.

#include "../webpp/http/bodies/string.hpp"
#include "../webpp/http/response_body.hpp"
#include "../webpp/std/string.hpp"
#include "./common/test.hpp"

#include <filesystem>

using namespace webpp;
using namespace webpp::http;
using namespace webpp::details;
using namespace webpp::http::details;


using string_type = stl::string;
using body_type   = response_body<char>;

TEST(Body, Concepts) {
    EXPECT_TRUE(bool(BodyReader<body_reader<char>>));
    EXPECT_TRUE(bool(BodyReader<body_writer<char>>));
    EXPECT_TRUE(bool(BodyWriter<body_writer<char>>));
}

struct custom_body_type {
    friend void tag_invoke(serialize_body_tag, custom_body_type, HTTPBody auto& body) {
        body = "custom body type";
    }
};

static_assert(SerializableBody<custom_body_type, body_type>, "custom body is not serializable but it should be.");
static_assert(DeserializableBody<stl::string_view, body_type>, "string view is not deserializable but it should be.");
static_assert(DeserializableBody<char const*, body_type>, "c-string is not deserializable but it should be.");

TEST(Body, CustomBodyTypeSerializerTest) {
    body_type body;
    body = custom_body_type{};
    EXPECT_EQ(body.as<stl::string_view>(), "custom body type");
}

TEST(Body, Text) {
    body_type b{"Testing"};
    EXPECT_EQ(b.template as<std::string_view>(), "Testing");
    EXPECT_TRUE(std::strcmp(b.template as<char const*>(), "Testing") == 0) << stl::string_view{b.as<char const*>()};

    // todo
    // EXPECT_TRUE(b == "Testing");

    string_type const str = "hello";
    b                     = str;

    EXPECT_TRUE(std::strcmp(b.as<char const*>(), "hello") == 0);

    constexpr std::string_view sth = "nice";
    b                              = sth;
    EXPECT_TRUE(std::strcmp(b.as<char const*>(), "nice") == 0);

    b = string_type("cool");
    EXPECT_EQ(b.as<stl::string_view>(), "cool");

    body_type bt;
    {
        string_type            _str = "testing";
        std::string_view const test = _str;
        bt                          = test;
        EXPECT_TRUE(std::strcmp(bt.as(), test.data()) == 0);
        _str = "";
    }
    // EXPECT_NE(bt.string(), "testing") << "The test should be empty since it was a string_view and not a
    // string";
}

TEST(Body, File) {
    std::filesystem::path file = std::filesystem::temp_directory_path();
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

    body_type the_body;
    the_body = "data";
    EXPECT_TRUE(std::strcmp(the_body.as<char const*>(), "data") == 0);
    // ASSERT_TRUE(the_body.load(file));
    // EXPECT_EQ(the_body.as(), "Hello World");
    std::filesystem::remove(file);
}

TEST(Body, StringCustomBody) {
    static_assert(istl::String<stl::string> && stl::is_default_constructible_v<stl::string>,
                  "We need string to be default constructible for this test to work.");
    body_type body{"Testing"};
    EXPECT_EQ(as<std::string>(body), "Testing");
    body                       = "Hello World";
    stl::string const body_str = body.as();
    EXPECT_EQ(body_str, "Hello World");
    body_type body2;
    body2          = "Hello World";
    auto body_str2 = body2.template as<stl::string>();
    EXPECT_EQ(body_str, body_str2);
    body_str2 = "nice";
    body2     = body_str2;
    body_str2 = body2.template as<stl::string>();
    EXPECT_EQ("nice", body_str2);
}

// NOLINTBEGIN(cppcoreguidelines-pro-type-reinterpret-cast)
////////////////////////////////////////////////// Read & Write //////////////////////////////////////////////////

TEST(Body, BodyStreamToStream) {
    body_type         body;
    stl::string const str = "one two three";
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
    body_type         body;
    stl::string const str = "this is a test";
    body.write(reinterpret_cast<stl::byte const*>(str.data()), static_cast<stl::streamsize>(str.size()));
    stl::string                 str2;
    static constexpr auto       buff_size = 10;
    stl::array<char, buff_size> buf{};
    while (auto res = body.read(reinterpret_cast<stl::byte*>(buf.data()), static_cast<stl::streamsize>(buf.size()))) {
        str2.append(buf.data(), static_cast<stl::size_t>(res));
    }
    EXPECT_EQ(str, str2);
}

TEST(Body, BodyTextToText) {
    body_type         body;
    stl::string const str = "this is a test";
    body.append(str.data(), str.size());
    stl::string const str2{body.data(), body.size()};
    EXPECT_EQ(str, str2);
}

////////////////////////////////////////////////// Cross Talk //////////////////////////////////////////////////

TEST(Body, BodyCrossTalkCStreamToText) {
    body_type         body;
    stl::string const str = "this is a test";
    body.write(reinterpret_cast<stl::byte const*>(str.data()), static_cast<stl::streamsize>(str.size()));
    ASSERT_NE(body.data(), nullptr);
    stl::string const str2{body.data(), body.size()};
    EXPECT_EQ(str, str2);
}

TEST(Body, BodyCrossTalkCStreamToStream) {
    body_type         body;
    stl::string const str = "this is a test";
    body.write(reinterpret_cast<stl::byte const*>(str.data()), static_cast<stl::streamsize>(str.size()));
    stl::string str2;
    body >> str2;
    EXPECT_EQ(str, str2);
}

TEST(Body, BodyCrossTalkTextToStream) {
    body_type         body;
    stl::string const str = "this is a test";
    body.append(str.data(), str.size());
    stl::string str2;
    body >> str2;
    EXPECT_EQ(str, str2);
}

TEST(Body, BodyCrossTalkTextToCStream) {
    body_type         body;
    stl::string const str = "this is a test";
    body.append(str.data(), str.size());
    stl::string                 str2;
    static constexpr auto       buff_size = 10;
    stl::array<char, buff_size> buf{};
    while (auto res = body.read(reinterpret_cast<stl::byte*>(buf.data()), static_cast<stl::streamsize>(buf.size()))) {
        str2.append(buf.data(), static_cast<stl::size_t>(res)); // res is always zero to not create a loop!!!
    }
    EXPECT_EQ(str, str2);
}

TEST(Body, BodyCrossTalkStreamToCStream) {
    body_type         body;
    stl::string const str = "this is a test";
    body << str;
    stl::string                 str2;
    static constexpr auto       buff_size = 10;
    stl::array<char, buff_size> buf{};
    while (auto res = body.read(reinterpret_cast<stl::byte*>(buf.data()), static_cast<stl::streamsize>(buf.size()))) {
        str2.append(buf.data(), static_cast<stl::size_t>(res));
    }
    EXPECT_EQ(str, str2);
}

// NOLINTEND(cppcoreguidelines-pro-type-reinterpret-cast)
