#include <gtest/gtest.h>
#include <webpp/router.h>
#include <webpp/interfaces/cgi.h>

using namespace webpp::valves;
using namespace webpp;

TEST(Router, RouteCreation) {
    using request = request_t<cgi>;
    constexpr route<cgi> about_page([](response &res) noexcept {
        res << "About page\n";
    });

}
