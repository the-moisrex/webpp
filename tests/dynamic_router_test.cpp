#include "../webpp/http/routes/dynamic_router.hpp"

#include "../webpp/http/routes/router.hpp"
#include "common_pch.hpp"


using namespace webpp;
using namespace webpp::http;
using namespace std;


template <typename Iter, char Num = 13>
void rot13(Iter begin, const Iter& end) noexcept {
    while (begin != end) {
        char& c = *begin;

        if (c >= 'a' && c <= 'm') {
            c += Num;
        } else if (c >= 'n' && c <= 'z') {
            c -= Num;
        } else if (c >= 'A' && c <= 'M') {
            c += Num;
        } else if (c >= 'N' && c <= 'Z') {
            c -= Num;
        }
        ++begin;
    }
}

void rot13(auto& str) noexcept {
    rot13(str.begin(), str.end());
}

struct pages {
    // NOLINTBEGIN(readability-convert-member-functions-to-static)
    [[nodiscard]] response about(context const& ctx) const {
        response res{ctx};
        res = "about page";
        return res;
    }

    // I know it's not efficient
    [[nodiscard]] response add_body(response res) const {
        res.body = "<body>" + as<stl::string>(res.body) + "</body>";
        return res;
    }

    void rot13_path(context& ctx) const {
        auto uri = ctx.request.uri();
        rot13(uri);
        ctx.request.uri(uri); // set the uri again
    }
    // NOLINTEND(readability-convert-member-functions-to-static)
};

TEST(DynamicRouter, RouteRegistration) {
    EXPECT_TRUE(bool(HTTPRequest<request>));

    enable_traits_for<dynamic_router> router;

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

    enable_traits_for<dynamic_router> router;
    router.objects.emplace_back(pages{});

    router += router / "about" >> &pages::about;

    request req{router.get_traits()};
    req.method("GET");
    req.uri("/about");

    EXPECT_EQ(as<std::string>(router(req).body), "about page");
}

TEST(DynamicRouter, NotNotTest) {

    enable_traits_for<dynamic_router> router;
    router.objects.emplace_back(pages{});

    router += !!(router / "about") >> &pages::about;

    request req{router.get_traits()};
    req.method("GET");
    req.uri("/about");

    EXPECT_EQ(as<std::string>(router(req).body), "about page") << router.to_string();
}

TEST(DynamicRouter, DynamicString) {

    enable_traits_for<dynamic_router> router;
    router.objects.emplace_back(pages{});
    std::string const about_route = "about";

    router += router / about_route >> &pages::about;

    request req{router.get_traits()};
    req.method("GET");
    req.uri("/about");

    EXPECT_EQ(as<std::string>(router(req).body), "about page");
}


// TEST(DynamicRouter, ManglerTest) {
//
//     enable_traits_for<dynamic_router> router;
//     router.objects.emplace_back(pages{});
//
//     auto const body_mangler = [](context& ctx, auto next) {
//         if (next(ctx)) {
//             ctx.response.body = "<body>" + as<stl::string>(ctx.response.body) + "</body>";
//         }
//     };
//
//     router += (router / "about" >> &pages::about) % body_mangler;
//
//     request req{router.get_traits()};
//     req.method("GET");
//     req.uri("/about");
//
//     EXPECT_EQ(as<std::string>(router(req).body), "<body>about page</body>") << router.to_string();
// }

TEST(DynamicRouter, CacheDeceptionTest) {
    // I got the idea from: https://twitter.com/naglinagli/status/1639351113571868673?s=20

    enable_traits_for<dynamic_router> router;
    router.objects.emplace_back(pages{});

    router += router % "about" >> &pages::about;

    request req{router.get_traits()};
    req.method("GET");
    req.uri("/about/style.css");

    auto const res = router(req);
    EXPECT_EQ(res.headers.status_code(), status_code::not_found) << router.to_string();
    EXPECT_NE(as<std::string>(res.body), "about page");
}

TEST(DynamicRouter, CommonBypassTests) {
    enable_traits_for<dynamic_router> router;
    router.objects.emplace_back(pages{});

    router += router % "admin" >> &pages::about;

    request req{router.get_traits()};
    req.method("GET");

    for (auto const u : {"/%2e/admin",
                         "/admin/.",
                         "//admin//",
                         "/./admin/..",
                         "/;/admin",
                         "/.;/admin",
                         "//;//admin",
                         "/admin..;/",
                         "/aDmIN"}) {
        req.uri(u);
        auto const res = router(req);
        EXPECT_EQ(res.headers.status_code(), status_code::not_found) << router.to_string();
        EXPECT_NE(as<std::string>(res.body), "about page");
    }
}

TEST(DynamicRouter, PostRoutingTest) {
    enable_traits_for<dynamic_router> router;
    router.objects.emplace_back(pages{});

    auto const main_page = router / "page" + &pages::add_body;
    router += main_page % "about" >> &pages::about;

    request req{router.get_traits()};
    req.method("GET");
    req.uri("/page/about");

    auto const res = router(req);
    EXPECT_EQ(res.headers.status_code(), status_code::ok) << router.to_string();
    EXPECT_EQ(as<std::string>(res.body), "<body>about page</body>") << router.to_string();
}

TEST(DynamicRouter, PreRoutingTest) {
    enable_traits_for<dynamic_router> router;
    router.objects.emplace_back(pages{});

    auto const main_page = router / "page" - &pages::rot13_path;
    router += main_page % "about" >> &pages::about;

    std::string uri = "/page/about";
    rot13(uri);
    request req{router.get_traits()};
    req.method("GET");
    req.uri(uri);

    auto const res = router(req);
    EXPECT_EQ(res.headers.status_code(), status_code::ok) << router.to_string();
    EXPECT_EQ(as<std::string>(res.body), "about page");
}

TEST(DynamicRouter, SameOrderPreRoutingTest) {
    enable_traits_for<dynamic_router> router;
    router.objects.emplace_back(pages{});
    int num = 0;

    auto const add_num = [&](context& ctx) {
        EXPECT_FALSE(ctx.path_traverser().at_end()) << num;
        // rot13 should already be run, so we should get a clear "page" as the first segment
        EXPECT_EQ("page", *ctx.path_traverser())
          << "Segment: " << *ctx.path_traverser() << "\n"
          << "Should be called before checking the paths, it's a pre-routing.\nNum: " << num
          << "\nRouter: " << router.to_string();
        ++num;
    };

    auto const main_page = router / "page" - &pages::rot13_path - add_num - add_num;
    router += ((main_page % "about" - add_num) >> &pages::about) - add_num;

    std::string uri = "/page/about";
    rot13(uri);
    request req{router.get_traits()};
    req.method("GET");
    req.uri(uri);

    auto const res = router(req);
    EXPECT_EQ(res.headers.status_code(), status_code::ok) << router.to_string();
    EXPECT_EQ(as<std::string>(res.body), "about page") << router.to_string();
    EXPECT_EQ(num, 4);
}

TEST(DynamicRouter, SameOrderPostRoutingTest) {
    enable_traits_for<dynamic_router> router;
    router.objects.emplace_back(pages{});
    int num = 0;

    auto const set_num = [&] {
        num = 10; // NOLINT(cppcoreguidelines-avoid-magic-numbers)
    };

    auto const add_num = [&](context& ctx) {
        EXPECT_TRUE(ctx.path_traverser().at_end()) << num << "\nRouter: " << router.to_string();
        EXPECT_GE(num, 10);
        ++num;
    };

    auto const main_page = router / "page" + add_num + add_num;
    router += (((main_page % "about" + add_num) >> &pages::about) >> set_num) + add_num;

    request req{router.get_traits()};
    req.method("GET");
    req.uri("/page/about");

    auto const res = router(req);
    EXPECT_EQ(res.headers.status_code(), status_code::ok) << router.to_string();
    EXPECT_EQ(as<std::string>(res.body), "about page") << router.to_string();
    EXPECT_EQ(num, 14);
}

TEST(DynamicRouter, PrePostRoutingTest) {
    enable_traits_for<dynamic_router> router;
    router.objects.emplace_back(pages{});

    auto const main_page = router / "page" - &pages::rot13_path + &pages::add_body;
    router += main_page % "about" >> &pages::about;

    std::string uri = "/page/about";
    rot13(uri);
    request req{router.get_traits()};
    req.method("GET");
    req.uri(uri);

    auto const res = router(req);
    EXPECT_EQ(res.headers.status_code(), status_code::ok) << router.to_string();
    EXPECT_EQ(as<std::string>(res.body), "<body>about page</body>") << router.to_string();
}


TEST(DynamicRouter, ValvesInStaticRouter) {
    router const _router{empty_extension_pack{},
                         valve{} / "home" >>
                           [] {
                               return "Home Page";
                           },
                         valve{} / "about" >>
                           []() {
                               return "About Page";
                           }};

    enable_owner_traits<default_dynamic_traits> et;

    request req{et};
    req.method("GET");
    req.uri("/about/style.css");

    // HTTPResponse auto const res = _router(req);
    // EXPECT_EQ(res.headers.status_code(), status_code::ok);
    // EXPECT_NE(as<std::string>(res.body), "about page");
}
