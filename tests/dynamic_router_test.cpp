#include "../core/include/webpp/http/routes/dynamic_router.hpp"

#include "common_pch.hpp"


using namespace webpp;
using namespace webpp::http;
using namespace std;

TEST(DynamicRouter, RouteRegistration) {
    dynamic_router router;

    auto const page = router / "page";
    router += page / "about" >> [] {
        return "About";
    };
    router += page / "index" >> [] {
        return "Index";
    };

    request req{router.get_traits()};
    router(req);
}
