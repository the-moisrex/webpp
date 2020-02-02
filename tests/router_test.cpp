#include <gtest/gtest.h>
#include <webpp/router.h>
#include <webpp/interfaces/cgi.h>

using namespace webpp::valves;
using namespace webpp;

TEST(Router, RouteCreation) {
    constexpr route<cgi> about_page([](auto &res) noexcept {
        res << "About page\n";
    });

}
