#include "../core/include/webpp/http/routes/dynamic_router.hpp"

#include "common_pch.hpp"


using namespace webpp;
using namespace webpp::http;
using namespace std;

TEST(DynamicRouter, Init) {
    dynamic_router router;
    router.set_route();
}
