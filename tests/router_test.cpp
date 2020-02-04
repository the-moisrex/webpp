#include <gtest/gtest.h>
#include <webpp/router.h>
#include <webpp/interfaces/cgi.h>

using namespace webpp::valves;
using namespace webpp;

TEST(Router, RouteCreation) {
    using request = request_t<cgi>;

    // this will happen with a help of a little "user-defined template deduction guide"
    constexpr auto about_page_callback = [](response &res) noexcept {
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

}

