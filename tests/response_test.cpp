// Created by moisrex on 2/4/20.


#include "../core/include/webpp/http/response.hpp"

#include "../core/include/webpp/http/bodies/string.hpp"
#include "../core/include/webpp/http/response_body.hpp"
#include "common_pch.hpp"


using namespace webpp;
using namespace webpp::http;


using res_t = simple_response<default_traits, string_body>;

TEST(HTTPResponseTest, Type) {
    constexpr auto return_callback = [] {
        return res_t::with_body("Hello");
    };
    using ret_type       = stl::remove_cvref_t<::std::invoke_result_t<decltype(return_callback)>>;
    constexpr bool one   = ::std::is_same_v<ret_type, res_t>;
    constexpr bool two   = ::std::is_convertible_v<ret_type, res_t>;
    constexpr bool three = ::std::is_convertible_v<res_t, res_t>;
    //    constexpr bool four  = ::std::is_convertible_v<std::string, res_t>;
    //    constexpr bool five = std::is_convertible_v<std::string_view, response>;
    EXPECT_TRUE(one);
    EXPECT_TRUE(two);
    EXPECT_TRUE(three);
    //    EXPECT_TRUE(four);
    //    EXPECT_TRUE(five);
    EXPECT_STREQ("Hello", return_callback().body.string().c_str());
}

TEST(Response, Init) {
    auto res  = res_t::create();
    auto res2 = res_t::create();

    EXPECT_EQ(res, res2);

    EXPECT_EQ(std::string(res.body.string("")), "");
    res2 << "Code";
    EXPECT_EQ(std::string(res2.body.string()), "Code");
    res = res2;
    EXPECT_EQ(std::string(res.body.string()), "Code");

    EXPECT_EQ(res, res2);
}

TEST(Response, File) {
    std::filesystem::path file = std::filesystem::temp_directory_path();
    file.append("webpp_test_file");
    std::ofstream handle{file};
    handle << "Hello World";
    handle.close();
    EXPECT_EQ(res_t::with_body(file).body.string(), "Hello World");
    std::filesystem::remove(file);
}
