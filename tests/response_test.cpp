// Created by moisrex on 2/4/20.


#include "../webpp/http/response.hpp"

#include "../webpp/http/bodies/file.hpp"
#include "../webpp/http/bodies/string.hpp"
#include "../webpp/http/response_body.hpp"
#include "../webpp/traits/enable_traits.hpp"
#include "common_pch.hpp"


using namespace webpp;
using namespace webpp::http;


using res_t = simple_response<default_traits, extension_pack<string_body>>;

TEST(HTTPResponseTest, Type) {
    enable_owner_traits<default_traits> et;
    auto                                return_callback = [&] {
        return res_t::with_body(et, "Hello");
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
    EXPECT_STREQ("Hello", return_callback().body.as<const char*>());
}

TEST(Response, Init) {
    enable_owner_traits<default_traits> et;
    HTTPResponse auto                   res  = res_t::create(et);
    HTTPResponse auto                   res2 = res_t::create(et);

    EXPECT_TRUE(res == res2);

    EXPECT_EQ(std::string(res.body.as()), "");
    res2 << "Code";
    EXPECT_EQ(std::string(res2.body.as<std::string>()), "Code");
    res = res2.body;
    EXPECT_EQ(std::string(res.body.as<std::string>()), "Code");

    EXPECT_EQ(res, res2);
}

TEST(Response, ResHeaders) {
    enable_owner_traits<default_traits> et;
    HTTPResponse auto                   res = res_t::create(et);
    res.headers.emplace_back("one", "1");
    EXPECT_EQ(res.headers["one"], "1");
    res.headers.emplace_back("two", "2");
    EXPECT_EQ(res.headers.has("one", "two"), stl::make_tuple(true, true));
    EXPECT_EQ(res.headers.has("one", "two", "three"), stl::make_tuple(true, true, false));
}

TEST(Response, ResHeadersReference) {
    enable_owner_traits<default_traits> et;
    HTTPResponse auto                   res = res_t::create(et);
    res.headers["Content-Length"]           = "10";
    EXPECT_EQ(res.headers["Content-Length"], "10");
}

TEST(Response, File) {
    enable_owner_traits<default_traits> et;
    std::filesystem::path               file = std::filesystem::temp_directory_path();
    file.append("webpp_test_file");
    std::ofstream handle{file};
    handle << "Hello World";
    handle.close();
    EXPECT_EQ(res_t::with_body(et, file).body.as<stl::string_view>(), "Hello World");
    std::filesystem::remove(file);
}


TEST(Response, DynamicResponse) {
    enable_owner_traits<default_dynamic_traits> et;

    response       res{et};
    response const res2{res};
    response       res3{et};
    res  = "string";
    res3 = "string";
    EXPECT_EQ(as<stl::string>(res), "string");
    EXPECT_EQ(res, res2) << as<std::string>(res) << as<std::string>(res2);
    EXPECT_EQ(res, res3) << as<std::string>(res) << as<std::string>(res3);
    EXPECT_EQ(res2, res3) << as<std::string>(res2) << as<std::string>(res3);
}