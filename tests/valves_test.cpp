// Created by moisrex on 11/7/19.
#include <gtest/gtest.h>
#include <webpp/valves/valve.h>
#include <webpp/http/request.h>
#include <string>

using namespace webpp;
using namespace webpp::valves;

namespace webpp {
    class fake_cgi_GET;

    template<>
    class request_t<fake_cgi_GET> {
    public:
        std::string request_method() const noexcept {
            return "GET";
        }
    };
}

TEST(ValvesTests, Creation) {
    auto v = method("GET") or method("POST");
    EXPECT_TRUE(v(request_t<fake_cgi_GET>()));

}
