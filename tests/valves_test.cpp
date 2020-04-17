// Created by moisrex on 11/7/19.
#include <gtest/gtest.h>
#include <string>
#include "../core/include/webpp/http/request.h"
#include "../core/include/webpp/valves/methods.h"

using namespace webpp;
using namespace webpp::valves;

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

TEST(ValvesTests, Creation) {
    constexpr auto v = method("GET") or method("POST");

    // I'm not gonna test the lowercase and uppercase stuff because it's
    // probably the request's job to fix that information not the valve.
    EXPECT_TRUE(v(request_t<fake_cgi>().set_method("GET")));
    EXPECT_TRUE(v(request_t<fake_cgi>().set_method("POST")));
}

TEST(ValvesTests, Operations) {
    constexpr auto v = empty and empty;

    EXPECT_TRUE(v(request_t<fake_cgi>()));
    EXPECT_TRUE(
        (empty and empty and empty or empty or empty)(request_t<fake_cgi>()));
}

TEST(ValveTests, DynamicValve) {
    auto dv = dynamic_valve<fake_cgi>() and method("GET") or method("POST");

    auto con1 = request_t<fake_cgi>().set_method("GET");
    auto con2 = request_t<fake_cgi>().set_method("POST");
    EXPECT_TRUE(dv(con1));
    EXPECT_TRUE(dv(con2));
}
