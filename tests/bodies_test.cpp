// Created by moisrex on 2/4/20.

#include "../core/include/webpp/http/bodies/string.hpp"
#include "common_pch.hpp"


using namespace webpp;
using namespace webpp::http;
using namespace webpp::details;
using namespace webpp::http::details;


using text_body_type = simple_response_body<default_traits, as_extension<string_body_extension>>;

TEST(Body, Text) {
    text_body_type b = "Testing";
    EXPECT_EQ(b.string(), "Testing");
    EXPECT_TRUE(b == "Testing");

    std::string str = "hello";
    b               = str;

    EXPECT_EQ(b, "hello");

    std::string_view const sth = "nice";
    b                          = sth;
    EXPECT_EQ(b, "nice");

    b = std::string("cool");
    EXPECT_EQ(b, "cool");

    text_body_type bt;
    {
        std::string            _str = "testing";
        std::string_view const test = _str;
        bt                          = test;
        EXPECT_EQ(bt.string(), test);
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
    std::string const file_out = buf.str();

    ASSERT_EQ(file_out, "Hello World");

    // so the file is okay

    text_body_type the_body;
    the_body = "data";
    EXPECT_EQ(the_body.string(), "data");
    ASSERT_TRUE(the_body.load(file));
    EXPECT_EQ(the_body.string(), "Hello World");
    std::filesystem::remove(file);
}
