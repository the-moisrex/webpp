// #include "../webpp/cgi/cgi.hpp"
#include "../webpp/http/bodies/string.hpp"
#include "../webpp/http/routes/context.hpp"
#include "../webpp/http/routes/path.hpp"
#include "../webpp/http/routes/static_router.hpp"
#include "../webpp/traits/enable_traits.hpp"
#include "common_pch.hpp"
#include "fake_protocol.hpp"


using namespace webpp;
using namespace webpp::http;
using namespace std;

constexpr auto s_router = static_router{root / "page" >>
                                          [] {
                                              return "page 1";
                                          },
                                        root / "test" >>
                                          [] {
                                              return "test 2";
                                          }};

struct fake_app_struct {
    HTTPResponse auto operator()(HTTPRequest auto&& req) noexcept {
        return s_router(req);
    }
};

TEST(Router, RouterConcepts) {
    EXPECT_TRUE(static_cast<bool>(Application<decltype(s_router)>));
}

TEST(Router, RouteCreation) {
    using fake_protocol = fake_proto<fake_app_struct>;
    fake_protocol fp;
    using request_type = typename fake_protocol::request_type;

    constexpr auto about_page = []() noexcept {
        return "About page\n";
    };


    request_type      req{fp};
    static_router     router1{about_page};
    HTTPResponse auto res = router1(req);
    res.calculate_default_headers();
    EXPECT_EQ(router1.route_count(), 1);
    EXPECT_EQ(res.headers.status_code_integer(), 200);
    EXPECT_EQ(as<std::string>(res.body), "About page\n");

    static_router           router2{[]() noexcept {
        return "testing";
    }};
    HTTPResponse auto const res2 = router2(req);
    EXPECT_EQ(as<std::string>(res2.body), "testing");


    static_router           router3{[]() {
        return "testing 2";
    }};
    HTTPResponse auto const res3 = router3(req);
    EXPECT_EQ(as<std::string>(res3.body), "testing 2");
}


// namespace webpp {
//    class fake_cgi;
//
//    template <typename Traits>
//    class basic_request<Traits, fake_cgi> {
//        stl::string method;
//
//      public:
//        stl::string request_method() const noexcept {
//            return method;
//        }
//
//        auto& set_method(stl::string _method) noexcept {
//            method = _method;
//            return *this;
//        }
//    };
//} // namespace webpp
//
// TEST(Router, ValveTest) {
//    constexpr auto return_callback = [] {
//        return basic_response("Hello");
//    };
//    constexpr auto v = method("GET");
//    route<default_traits, fake_cgi, decltype(return_callback), decltype(v)>
//    one{v};
//
//    basic_request<default_traits, fake_cgi> req;
//    req.set_method("GET");
//
//    EXPECT_EQ(req.request_method(), "GET");
//}
//
// TEST(Router, RouterClass) {
//    //    constexpr auto return_callback = [i = 2]() mutable {
//    //        i++;
//    //        return response("Hello");
//    //    };
//    //    constexpr auto v = method("GET");
//    //    constexpr route<fake_cgi, decltype(return_callback), decltype(v)> one{
//    //        v, return_callback};
//    //
//    //    constexpr auto route_list = make_const_list(one);
//    //
//    //    router<fake_cgi, decltype(route_list)> rr{one};
//    //
//    //    request_t<fake_cgi> req;
//    //    req.set_method("GET");
//    //    response res;
//    //
//    //    EXPECT_TRUE(one.is_match(req));
//    //    res = rr(req);
//    //    EXPECT_EQ(std::string(res.body.string()), "Hello");
//    //    EXPECT_EQ(i, 3);
//}
//
// TEST(Router, VectorForRouteList) {
//    router_t<default_traits, fake_cgi,
//             istl::vector<default_traits, dynamic_route<default_traits, fake_cgi>>>
//      _route{};
//    _route.on(method("GET"), []() noexcept {
//        return "Hello world";
//    });
//
//    basic_request<default_traits, fake_cgi> req;
//    req.set_method("GET");
//    basic_response res         = _route(req);
//    auto           strview_res = res.body.string();
//    std::string    str_res     = std::string(strview_res);
//    EXPECT_EQ(strview_res, "Hello world") << "strview_res is: " <<
//    strview_res;
//    // EXPECT_EQ(str_res, "Hello world") << "str_res is: " <<
//    // std::string(*static_cast<std::string*>(res.body.data));
//}
//
// TEST(Router, TupleForRouteList) {
//    auto s_router =
//      router_t<default_traits, fake_cgi, std::tuple<>>{}.on(method("GET"), [] {
//          return "Hello world";
//      });
//
//    basic_request<default_traits, fake_cgi> req;
//    req.set_method("GET");
//    basic_response res = s_router(req);
//    EXPECT_EQ(std::string(res.body.string()), "Hello world");
//}
//
// TEST(Router, ConstListForRouteList) {
//    //    constexpr auto s_router = router<fake_cgi, const_list<>>{}.on(
//    //        method("GET"), [] { return "Hello world"; });
//    //
//    //    request_t<fake_cgi> req;
//    //    req.set_method("GET");
//    //    response res = s_router(req);
//    //    EXPECT_EQ(std::string(res.body.string()), "Hello world");
//}
//
// TEST(Router, DefaultRouteList) {
//    auto s_router = router_t<fake_cgi, std::tuple<>>{}.on(method("GET"), [] {
//        return "Hello world";
//    });
//
//    basic_request<default_traits, fake_cgi> req;
//    req.set_method("GET");
//    basic_response res = s_router(req);
//    EXPECT_EQ(std::string(res.body.string()), "Hello world");
//}
//
// TEST(Router, MergeEffect) {
//    //    router<fake_cgi, std::tuple<>> s_router1{};
//    //    s_router1.on(method("GET"), [] { return "Hello world"; });
//    //
//    //    constexpr auto s_router2 = router<fake_cgi, std::tuple<>>{}.on(
//    //        method("GET"), [] { return "Hello world"; });
//    //
//    //    router<fake_cgi> mergeds_router(s_router1, s_router2);
//    //
//    //    request_t<fake_cgi> req;
//    //    req.set_method("GET");
//    //    response res = mergeds_router(req);
//    //    EXPECT_EQ(std::string(res.body.string()), "Hello world");
//}
//
// TEST(Router, DynamicRoute) {
//    dynamic_route<default_traits, fake_cgi> droute;
//    droute = []() {
//        return basic_response("Hello world");
//    };
//
//    basic_request<default_traits, fake_cgi> req;
//    req.set_method("GET");
//
//    basic_response<default_traits> res;
//    droute(req, res);
//    EXPECT_EQ(std::string(res.body.string()), "Hello world");
//}
//
//
//
// TEST(Router, ParseVars) {
//    std::string a     = "/user/19";
//    std::string templ = "/user/{user_id}";
//    auto        vars  = parse_vars(templ, a);
//    EXPECT_EQ(vars.size(), 1);
//    EXPECT_EQ(vars["user_id"], "19") << "user_id: " << vars["user_id"];
//
//    vars = parse_vars("/usr/user-{user_id}/{page}", "/usr/user-21/profile");
//
//    EXPECT_EQ(vars.size(), 2);
//    EXPECT_EQ(vars["user_id"], "21");
//    EXPECT_EQ(vars["page"], "profile");
//}
