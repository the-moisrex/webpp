// Created by moisrex on 11/7/19.
#include "../core/include/webpp/http/request.h"
#include "../core/include/webpp/routes/literals.h"
#include "../core/include/webpp/routes/methods.h"
#include "../core/include/webpp/routes/path.h"
#include "../core/include/webpp/traits/std_traits.h"

#include <gtest/gtest.h>
#include <string>
#include <utility>

using namespace webpp;
using namespace webpp::routes;

namespace webpp {
    class fake_cgi;

    template <typename Traits>
    class request_t<Traits, fake_cgi> {
        std::string method = "GET";
        std::string _uri   = "/home";

      public:
        [[nodiscard]] std::string request_method() const noexcept {
            return method;
        }

        auto& set_method(std::string _method) noexcept {
            method = std::move(_method);
            return *this;
        }

        auto& set_uri(std::string __uri) noexcept {
            this->_uri = std::move(__uri);
            return *this;
        }

        [[nodiscard]] auto request_uri() const noexcept {
            return _uri;
        }
    };
} // namespace webpp

TEST(Valves, Creation) {
    constexpr auto v = method("GET") or method("POST");

    // I'm not gonna test the lowercase and uppercase stuff because it's
    // probably the request's job to fix that information not the valve.
    EXPECT_TRUE(v(request_t<std_traits, fake_cgi>().set_method("GET")));
    EXPECT_TRUE(v(request_t<std_traits, fake_cgi>().set_method("POST")));
}

TEST(Valves, Operations) {
    constexpr auto v = empty and empty;

    EXPECT_TRUE(v(request_t<std_traits, fake_cgi>()));
    EXPECT_TRUE((empty and empty and empty or empty or
                 empty)(request_t<std_traits, fake_cgi>()));
}

TEST(Valves, DynamicValve) {
    auto dv =
      dynamic_valve<std_traits, fake_cgi>() and method("GET") or method("POST");

    auto con1 = request_t<std_traits, fake_cgi>().set_method("GET");
    auto con2 = request_t<std_traits, fake_cgi>().set_method("POST");
    EXPECT_TRUE(dv(con1));
    EXPECT_TRUE(dv(con2));
}

TEST(Valves, EmptyValve) {
    constexpr auto or_one   = empty;
    constexpr auto or_two   = get or empty;
    constexpr auto or_three = empty or get;

    auto req = request_t<std_traits, fake_cgi>().set_method("POST");

    EXPECT_TRUE(or_one(req));
    EXPECT_TRUE(or_two(req));
    EXPECT_TRUE(or_three(req));

    constexpr auto and_one   = post;
    constexpr auto and_two   = post and empty;
    constexpr auto and_three = empty and post;

    EXPECT_TRUE(and_one(req));
    EXPECT_TRUE(and_two(req));
    EXPECT_TRUE(and_three(req));
}

TEST(Valves, TPath) {
    using namespace webpp::valves;
    auto req = request_t<std_traits, fake_cgi>().set_method("POST");
    req.set_uri("/home/one");
    EXPECT_TRUE("/home/{page}"_tpath(req));
}
