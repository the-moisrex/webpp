// Created by moisrex on 11/7/19.
#include "./fake_protocol.hpp"

// clang-format off
#include webpp_include(http/routes/methods)
#include webpp_include(http/routes/path)
// clang-format on

using namespace webpp;

struct fake_app {

    Response auto operator()(Request auto&& req) {
        router _router{[]() {}};
        return _router(req);
    }
};

using request_type = fake_request_type;
using context_type = simple_context<request_type>;

TEST(Routes, Path) {
    request_type req;
    context_type ctx{};


    //    EXPECT_TRUE((root / number{"integer name"})(ctx));
    //    EXPECT_TRUE((root / number{"integer name"} / number{"2th num"})(ctx));
    //    EXPECT_TRUE((root / number{"integer name"} / number{"another number"} / number{"3th num"})(ctx));
}



// namespace webpp {
//    class fake_cgi;
//
//    template <typename Traits>
//    class basic_request<Traits, fake_cgi> {
//        stl::string method = "GET";
//        stl::string _uri   = "/home";
//
//      public:
//        [[nodiscard]] stl::string request_method() const noexcept {
//            return method;
//        }
//
//        auto& set_method(stl::string _method) noexcept {
//            method = stl::move(_method);
//            return *this;
//        }
//
//        auto& set_uri(stl::string __uri) noexcept {
//            this->_uri = stl::move(__uri);
//            return *this;
//        }
//
//        [[nodiscard]] auto request_uri() const noexcept {
//            return _uri;
//        }
//    };
//} // namespace webpp
//
// TEST(Valves, Creation) {
//    constexpr auto v = method("GET") or method("POST");
//
//    // I'm not gonna test the lowercase and uppercase stuff because it's
//    // probably the request's job to fix that information not the valve.
//    EXPECT_TRUE(v(basic_request<std_traits, fake_cgi>().set_method("GET")));
//    EXPECT_TRUE(v(basic_request<std_traits, fake_cgi>().set_method("POST")));
//}
//
// TEST(Valves, Operations) {
//    constexpr auto v = empty and empty;
//
//    EXPECT_TRUE(v(basic_request<std_traits, fake_cgi>()));
//    EXPECT_TRUE((empty and empty and empty or empty or
//                 empty)(basic_request<std_traits, fake_cgi>()));
//}
//
// TEST(Valves, DynamicValve) {
//    auto dv =
//      dynamic_valve<std_traits, fake_cgi>() and method("GET") or
//      method("POST");
//
//    auto con1 = basic_request<std_traits, fake_cgi>().set_method("GET");
//    auto con2 = basic_request<std_traits, fake_cgi>().set_method("POST");
//    EXPECT_TRUE(dv(con1));
//    EXPECT_TRUE(dv(con2));
//}
//
// TEST(Valves, EmptyValve) {
//    constexpr auto or_one   = empty;
//    constexpr auto or_two   = get or empty;
//    constexpr auto or_three = empty or get;
//
//    auto req = basic_request<std_traits, fake_cgi>().set_method("POST");
//
//    EXPECT_TRUE(or_one(req));
//    EXPECT_TRUE(or_two(req));
//    EXPECT_TRUE(or_three(req));
//
//    constexpr auto and_one   = post;
//    constexpr auto and_two   = post and empty;
//    constexpr auto and_three = empty and post;
//
//    EXPECT_TRUE(and_one(req));
//    EXPECT_TRUE(and_two(req));
//    EXPECT_TRUE(and_three(req));
//}
//
// TEST(Valves, TPath) {
//    using namespace webpp::valves;
//    auto req = basic_request<std_traits, fake_cgi>().set_method("POST");
//    req.set_uri("/home/one");
//    EXPECT_TRUE("/home/{page}"_tpath(req));
//}
