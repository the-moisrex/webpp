#include <any>
#include <gtest/gtest.h>
#include <vector>
#include <webpp/interfaces/cgi.h>
#include <webpp/router.h>

using namespace webpp::valves;
using namespace webpp;

TEST(Router, RouteCreation) {
    using request = request_t<cgi>;

    // this will happen with a help of a little "user-defined template deduction
    // guide"
    constexpr auto about_page_callback = [](response& res) noexcept {
        res << "About page\n";
    };
    route<cgi, decltype(about_page_callback)> about_page{};

    auto req = request_t<cgi>{};
    auto res = response();

    about_page(req, res);
    EXPECT_EQ(res.body.str(""), "About page\n");



    constexpr auto return_callback = [] {
        return response("Hello");
    };
    route<cgi, decltype(return_callback)> one{};
    one(req, res);
    EXPECT_EQ(std::string(res.body.str()), "Hello");

    constexpr auto return_callback_string = [] {
        return "Hello String";
    };
    route<cgi, decltype(return_callback_string)> two{};
    two(req, res);
    EXPECT_EQ(std::string(res.body.str()), "Hello String");
}


namespace webpp {
    class fake_cgi;

    template<>
    class request_t<fake_cgi> {
        std::string method;
    public:
        std::string request_method() const noexcept {
            return method;
        }

        auto &set_method(std::string _method) noexcept {
            method = _method;
            return *this;
        }
    };
}

TEST(Router, ValveTest) {
    using namespace webpp::valves;
    using namespace webpp;

    constexpr auto return_callback = [] {
        return response("Hello");
    };
    constexpr auto v = method("GET");
    route<fake_cgi, decltype(return_callback), decltype(v)> one{v};

    request_t<fake_cgi> req;
    req.set_method("GET");

    EXPECT_EQ(req.request_method(), "GET");
}


TEST(Router, RouterClass) {
    using namespace webpp::valves;
    using namespace webpp;

    constexpr auto return_callback = [i = 2]() mutable {
        i++;
        return response("Hello");
    };
    constexpr auto v = method("GET");
    constexpr route<fake_cgi, decltype(return_callback), decltype(v)> one{
        v, return_callback};

    constexpr auto route_list = make_const_list(one);

    router<fake_cgi, decltype(route_list)> rr{one, route_list};

    request_t<fake_cgi> req;
    req.set_method("GET");
    response res;

    EXPECT_TRUE(one.is_match(req));
    rr.run(req);
    EXPECT_EQ(std::string(res.body.str()), "Hello");
    //    EXPECT_EQ(i, 3);
}

TEST(Router, VectorForRouteList) {
    using namespace webpp::valves;
    using namespace webpp;
    using namespace std;

    router<fake_cgi, vector<any>> _route{};
    _route.on(method("GET"), [] { return "Hello world"; });

    request_t<fake_cgi> req;
    req.set_method("GET");
    response res;

    _route.run(req);
    EXPECT_EQ(std::string(res.body.str()), "Hello world");
}