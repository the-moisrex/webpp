#include <gtest/gtest.h>
#include <iostream>
#include <webpp/http/cookies.h>

TEST(Cookies, CookiesCreation) {
    webpp::cookie cookie;
    cookie.name("   test   ").value("  value  ");
    EXPECT_TRUE(cookie.name() == "test");
    EXPECT_TRUE(cookie.value() == "value");
    EXPECT_TRUE(webpp::cookie("  test  ", "  value ").name() ==
                webpp::cookie("test", "value").name())
        << "cookies should be trimmed";
}

TEST(Cookies, CookiesHash) {
    using namespace webpp;
    cookie_hash hash;
    auto a = hash(webpp::cookie("yes", "value"));
    auto b = hash(webpp::cookie("  yes  ", "  value  "));

    EXPECT_FALSE(hash(webpp::cookie("one", "value")) ==
                 hash(webpp::cookie("two", "value")));
    EXPECT_TRUE(a == b) << "Same cookies should be the same.";
}

TEST(Cookies, CookieJar) {
    using namespace webpp;

    cookies jar;
    jar.emplace("one", "value");
    jar.emplace(" one ", "value 2"); // this should replace the other one
    jar.emplace("two", "value");

    EXPECT_TRUE(jar.size() == 2)
        << "cookies with the same name should be replaced with the older ones";

    cookies jar2;
    jar.emplace("one", "value");
    jar.emplace("two", "value");
    jar.emplace(" one ", "value 2"); // this should replace the other one

    EXPECT_TRUE(jar2.size() == 2)
        << "The order that cookies get added to cookie jar does not matter";
}
