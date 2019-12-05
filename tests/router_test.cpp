#include <gtest/gtest.h>
#include <webpp/router.h>

// TEST(Router, RouterCreation) {
//    // in main we would have something like this:
//    webpp::router_t router_t;
//    auto callback = []() {};
//    router_t.on("GET", webpp::matcher::simple("/about"), callback);

//    // these routes will have /blog/ as their parents
//    router_t.on<webpp::matcher::simple>("/blog/")->get<webpp::matcher::regex>(
//        "^post/[0-9]+$", callback);

//    EXPECT_TRUE(router_t.match("GET", "/blog/post/23")->is_active());
//}
