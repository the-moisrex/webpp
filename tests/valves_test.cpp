// Created by moisrex on 11/7/19.
#include <gtest/gtest.h>
#include <webpp/valves/valve.h>
#include <webpp/http/request.h>
#include <string>

using namespace webpp;
using namespace webpp::valves;

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

TEST(ValvesTests, Creation) {
    constexpr auto v = method("GET") or method("POST");

    // I'm not gonna test the lowercase and uppercase stuff because it's probably the request's job to fix that information not the valve.
    EXPECT_TRUE(v(request_t<fake_cgi>().set_method("GET")));
    EXPECT_TRUE(v(request_t<fake_cgi>().set_method("POST")));
}
