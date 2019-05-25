#include <gtest/gtest.h>
#include <webpp/router.hpp>

// TEST(Router, RouterCreation) {
//    // in main we would have something like this:
//    webpp::router router;
//    auto callback = []() {};
//    router.use("GET", webpp::matcher::simple("/about"), callback);

//    // these routes will have /blog/ as their parents
//    router.use<webpp::matcher::simple>("/blog/")->get<webpp::matcher::regex>(
//        "^post/[0-9]+$", callback);

//    EXPECT_TRUE(router.match("GET", "/blog/post/23")->is_active());
//}
