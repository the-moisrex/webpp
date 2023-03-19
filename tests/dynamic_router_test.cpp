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

    request    req{router.get_traits()};
    auto const empty_res = router(req);
    EXPECT_TRUE(empty_res.body.empty()) << as<stl::string>(empty_res.body);
    EXPECT_EQ(empty_res.headers.status_code(), status_code::not_found) << router.to_string();

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

TEST(DynamicRouter, MemFuncPtr) {

    struct pages {
        // NOLINTBEGIN(readability-convert-member-functions-to-static)
        [[nodiscard]] response about(context const& ctx) const {
            response res{ctx};
            res = "about page";
            return res;
        }
        // NOLINTEND(readability-convert-member-functions-to-static)
    };

    dynamic_router router;
    router.objects.emplace_back(pages{});

    router += router / "about" >> &pages::about;

    request req{router.get_traits()};
    req.method("GET");
    req.uri("/about");

    EXPECT_EQ(as<std::string>(router(req).body), "about page");
}
