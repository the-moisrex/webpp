#include "../core/include/webpp/http/routes/dynamic_router.hpp"

#include "common_pch.hpp"


using namespace webpp;
using namespace webpp::http;
using namespace std;

TEST(DynamicRouter, RouteRegistration) {
    dynamic_router router;
    router += router / "page" >> [](context ctx) -> response {
        auto res = ctx.create_response();
        res      = "Hello world";
        return res;
    };
}
