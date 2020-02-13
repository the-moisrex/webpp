#include <gtest/gtest.h>
#include <tuple>
#include <vector>
#include <webpp/interfaces/cgi.h>
#include <webpp/router.h>
#include <webpp/utils/const_list.h>

using namespace webpp::valves;
using namespace webpp;
using namespace std;

TEST(Router, RouteCreation) {
    using request = request_t<cgi>;

    // this will happen with a help of a little "user-defined template deduction
    // guide"
    constexpr auto about_page_callback = [](response& res) noexcept {
        res << "About page\n";
    };
    route<cgi, decltype(about_page_callback)> about_page{about_page_callback};

    auto req = request_t<cgi>{};
    auto res = response();

    about_page(req, res);
    EXPECT_EQ(res.body.str(""), "About page\n");

    constexpr auto return_callback = [] { return response("Hello"); };
    route<cgi, decltype(return_callback)> one{};
    one(req, res);
    EXPECT_EQ(std::string(res.body.str()), "Hello");

    constexpr auto return_callback_string = [] { return "Hello String"; };
    route<cgi, decltype(return_callback_string)> two{};
    two(req, res);
    EXPECT_EQ(std::string(res.body.str()), "Hello String");
}

namespace webpp {
    class fake_cgi;

    template <>
    class request_t<fake_cgi> {
        std::string method;

      public:
        std::string request_method() const noexcept { return method; }

        auto& set_method(std::string _method) noexcept {
            method = _method;
            return *this;
        }
    };
} // namespace webpp

TEST(Router, ValveTest) {

    constexpr auto return_callback = [] { return response("Hello"); };
    constexpr auto v = method("GET");
    route<fake_cgi, decltype(return_callback), decltype(v)> one{v};

    request_t<fake_cgi> req;
    req.set_method("GET");

    EXPECT_EQ(req.request_method(), "GET");
}

TEST(Router, RouterClass) {

    //    constexpr auto return_callback = [i = 2]() mutable {
    //        i++;
    //        return response("Hello");
    //    };
    //    constexpr auto v = method("GET");
    //    constexpr route<fake_cgi, decltype(return_callback), decltype(v)> one{
    //        v, return_callback};
    //
    //    constexpr auto route_list = make_const_list(one);
    //
    //    router<fake_cgi, decltype(route_list)> rr{one};
    //
    //    request_t<fake_cgi> req;
    //    req.set_method("GET");
    //    response res;
    //
    //    EXPECT_TRUE(one.is_match(req));
    //    res = rr(req);
    //    EXPECT_EQ(std::string(res.body.str()), "Hello");
    //    EXPECT_EQ(i, 3);
}

TEST(Router, VectorForRouteList) {

    router<fake_cgi, vector<dynamic_route<fake_cgi>>> _route{};
    _route.on(method("GET"), [] () noexcept { return "Hello world"; });

    request_t<fake_cgi> req;
    req.set_method("GET");
    response res = _route(req);
    auto strview_res = res.body.str();
    std::string str_res = std::string(strview_res);
    EXPECT_EQ(strview_res, "Hello world") << "strview_res is: " << strview_res;
    //EXPECT_EQ(str_res, "Hello world") << "str_res is: " << std::string(*static_cast<std::string*>(res.body.data));
}

TEST(Router, TupleForRouteList) {

    auto _router = router<fake_cgi, std::tuple<>>{}.on(
        method("GET"), [] { return "Hello world"; });

    request_t<fake_cgi> req;
    req.set_method("GET");
    response res = _router(req);
    EXPECT_EQ(std::string(res.body.str()), "Hello world");
}

TEST(Router, ConstListForRouteList) {

    //    constexpr auto _router = router<fake_cgi, const_list<>>{}.on(
    //        method("GET"), [] { return "Hello world"; });
    //
    //    request_t<fake_cgi> req;
    //    req.set_method("GET");
    //    response res = _router(req);
    //    EXPECT_EQ(std::string(res.body.str()), "Hello world");
}

TEST(Router, DefaultRouteList) {

    auto _router = router<fake_cgi, std::tuple<>>{}.on(
        method("GET"), [] { return "Hello world"; });

    request_t<fake_cgi> req;
    req.set_method("GET");
    response res = _router(req);
    EXPECT_EQ(std::string(res.body.str()), "Hello world");
}

TEST(Router, MergeEffect) {
    //    router<fake_cgi, std::tuple<>> _router1{};
    //    _router1.on(method("GET"), [] { return "Hello world"; });
    //
    //    constexpr auto _router2 = router<fake_cgi, std::tuple<>>{}.on(
    //        method("GET"), [] { return "Hello world"; });
    //
    //    router<fake_cgi> merged_router(_router1, _router2);
    //
    //    request_t<fake_cgi> req;
    //    req.set_method("GET");
    //    response res = merged_router(req);
    //    EXPECT_EQ(std::string(res.body.str()), "Hello world");
}

TEST(Router, DynamicRoute) {
    dynamic_route<fake_cgi> droute;
    droute = []() { return response("Hello world"); };

    request_t<fake_cgi> req;
    req.set_method("GET");

    response res;
    droute(req, res);
    EXPECT_EQ(std::string(res.body.str()), "Hello world");
}
