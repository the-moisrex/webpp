#include "../core/include/webpp/http/routes/dynamic_router.hpp"

#include "common_pch.hpp"


using namespace webpp;
using namespace webpp::http;
using namespace std;

TEST(DynamicRouter, RouteRegistration) {
    EXPECT_TRUE(bool(HTTPRequest<request>));

    dynamic_router router;

    auto const page = router / "page";
    router += page / "about" >> [] {
        return "About";
    };
    router += page / "index" >> [] {
        return "Index";
    };

    request req{router.get_traits()};
    EXPECT_EQ(router(req).headers.status_code(), status_code::not_found);

    req.method("GET");
    req.uri("/page/about");
    EXPECT_EQ(req.uri(), "/page/about");
    auto it = req.path_iterator();
    EXPECT_TRUE(it.check_segment("page")) << *it;
    EXPECT_TRUE(it.check_segment("about")) << *it;

    auto res = router(req);
    EXPECT_EQ(res.headers.status_code(), status_code::ok) << router.to_string();
    EXPECT_EQ(as<std::string>(res.body), "About");
}
