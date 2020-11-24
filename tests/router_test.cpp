#include "../core/include/webpp/http/routes/router.hpp"

#include "../core/include/webpp/application/application_concepts.hpp"
#include "../core/include/webpp/http/protocols/cgi.hpp"
#include "../core/include/webpp/http/routes/path.hpp"
#include "../core/include/webpp/utils/const_list.hpp"
#include "./common_pch.hpp"
#include "fake_protocol.hpp"

#include <tuple>
#include <vector>

using namespace webpp;
using namespace std;

constexpr auto _router = router{root / "page" >>=
                                [] {
                                    return "page 1";
                                },
                                relative / "test" >>=
                                [] {
                                    return "test 2";
                                }};

struct fake_app {
    Response auto operator()(Request auto&& req) noexcept {
        return _router(req);
    }
};

TEST(Router, RouterConcepts) {
    EXPECT_TRUE(static_cast<bool>(Application<decltype(_router)>));
}

TEST(Router, RouteCreation) {
    using request = typename fake_proto<std_traits, fake_app>::request_type;

    constexpr auto about_page = [](Context auto& ctx) noexcept {
        return "About page\n";
    };

    auto   req = request{};
    router router1{extension_pack<string_response>{}, about_page};
    auto   res = router1(req);
    res.calculate_default_headers();
    EXPECT_EQ(router1.route_count(), 1);
    EXPECT_EQ(res.body.str(), "About page\n");
    EXPECT_EQ(res.headers.status_code, 200);

    router router2{extension_pack<string_response>{}, [](Context auto&& ctx) noexcept {
                       return ctx.string("testing");
                   }};
    auto   res2 = router2(req);
    EXPECT_EQ(res2.body.str(), "testing");


    router router3{extension_pack<string_response>{}, [](Context auto&& ctx) noexcept(false) {
      return ctx.string("testing 2");
    }};
    auto   res3 = router3(req);
    EXPECT_EQ(res3.body.str(), "testing 2");
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
//    route<std_traits, fake_cgi, decltype(return_callback), decltype(v)>
//    one{v};
//
//    basic_request<std_traits, fake_cgi> req;
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
//    //    EXPECT_EQ(std::string(res.body.str()), "Hello");
//    //    EXPECT_EQ(i, 3);
//}
//
// TEST(Router, VectorForRouteList) {
//    router_t<std_traits, fake_cgi,
//             istl::vector<std_traits, dynamic_route<std_traits, fake_cgi>>>
//      _route{};
//    _route.on(method("GET"), []() noexcept {
//        return "Hello world";
//    });
//
//    basic_request<std_traits, fake_cgi> req;
//    req.set_method("GET");
//    basic_response res         = _route(req);
//    auto           strview_res = res.body.str();
//    std::string    str_res     = std::string(strview_res);
//    EXPECT_EQ(strview_res, "Hello world") << "strview_res is: " <<
//    strview_res;
//    // EXPECT_EQ(str_res, "Hello world") << "str_res is: " <<
//    // std::string(*static_cast<std::string*>(res.body.data));
//}
//
// TEST(Router, TupleForRouteList) {
//    auto _router =
//      router_t<std_traits, fake_cgi, std::tuple<>>{}.on(method("GET"), [] {
//          return "Hello world";
//      });
//
//    basic_request<std_traits, fake_cgi> req;
//    req.set_method("GET");
//    basic_response res = _router(req);
//    EXPECT_EQ(std::string(res.body.str()), "Hello world");
//}
//
// TEST(Router, ConstListForRouteList) {
//    //    constexpr auto _router = router<fake_cgi, const_list<>>{}.on(
//    //        method("GET"), [] { return "Hello world"; });
//    //
//    //    request_t<fake_cgi> req;
//    //    req.set_method("GET");
//    //    response res = _router(req);
//    //    EXPECT_EQ(std::string(res.body.str()), "Hello world");
//}
//
// TEST(Router, DefaultRouteList) {
//    auto _router = router_t<fake_cgi, std::tuple<>>{}.on(method("GET"), [] {
//        return "Hello world";
//    });
//
//    basic_request<std_traits, fake_cgi> req;
//    req.set_method("GET");
//    basic_response res = _router(req);
//    EXPECT_EQ(std::string(res.body.str()), "Hello world");
//}
//
// TEST(Router, MergeEffect) {
//    //    router<fake_cgi, std::tuple<>> _router1{};
//    //    _router1.on(method("GET"), [] { return "Hello world"; });
//    //
//    //    constexpr auto _router2 = router<fake_cgi, std::tuple<>>{}.on(
//    //        method("GET"), [] { return "Hello world"; });
//    //
//    //    router<fake_cgi> merged_router(_router1, _router2);
//    //
//    //    request_t<fake_cgi> req;
//    //    req.set_method("GET");
//    //    response res = merged_router(req);
//    //    EXPECT_EQ(std::string(res.body.str()), "Hello world");
//}
//
// TEST(Router, DynamicRoute) {
//    dynamic_route<std_traits, fake_cgi> droute;
//    droute = []() {
//        return basic_response("Hello world");
//    };
//
//    basic_request<std_traits, fake_cgi> req;
//    req.set_method("GET");
//
//    basic_response<std_traits> res;
//    droute(req, res);
//    EXPECT_EQ(std::string(res.body.str()), "Hello world");
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
