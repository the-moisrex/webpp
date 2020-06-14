#include "../core/include/webpp/http/cookies/cookie.h"
#include "../core/include/webpp/http/cookies/cookie_jar.h"
#include "../core/include/webpp/traits/std_traits.h"

#include <gtest/gtest.h>
#include <iostream>


using res_cookie_t     = webpp::response_cookie<webpp::std_traits>;
using res_cookie_jar_t = webpp::response_cookie_jar<webpp::std_traits>;
using cookie_hash_t    = webpp::cookie_hash<res_cookie_t>;

TEST(Cookie, CookiesCreation) {
    res_cookie_t c;
    c.name("   test   ").value("  value  ");
    EXPECT_EQ(c.name(), "test");
    EXPECT_EQ(c.value(), "value");
    EXPECT_EQ(res_cookie_t("  test  ", "  value ").name(),
              res_cookie_t("test", "value").name())
      << "cookies should be trimmed";
    EXPECT_EQ(res_cookie_t("  test  ", "  value  ").name(),
              res_cookie_t().name("  test  ").name())
      << "name should trim it too.";
}

// TODO: fill here
TEST(Cookie, CookieExpirationDate) {
    res_cookie_t c;
    c.name("name").value("value");
    c.expires_in(std::chrono::minutes(1));
    EXPECT_TRUE(c.expires().time_since_epoch().count() > 0);
}

TEST(Cookies, CookiesHash) {
    cookie_hash_t hash;
    auto          a = hash(res_cookie_t("yes", "value"));
    auto          b = hash(res_cookie_t("  yes  ", "  value  "));

    EXPECT_FALSE(hash(res_cookie_t("one", "value")) ==
                 hash(res_cookie_t("two", "value")));
    EXPECT_TRUE(a == b) << "Same cookies should be the same.";
}

TEST(Cookies, CookieJar) {

    res_cookie_jar_t jar;
    jar.emplace_back("one", "value");
    jar.emplace_back(" one ", "value 2"); // this should replace the other one
    jar.emplace_back("two", "value");

    EXPECT_TRUE(jar.size() == 2)
      << "cookies with the same name should be replaced with the older ones";

    res_cookie_jar_t jar2;
    jar2.emplace_back("one", "value 1");
    jar2.emplace_back("two", "value 2");
    jar2.emplace_back(" one ",
                      "value 1-2"); // this should replace the other one

    EXPECT_TRUE(jar2.size() == 2)
      << "The order that cookies get added to cookie jar does not matter";

    jar2.emplace_back("two", "value 2-2");

    // These all should be replaced with the first one; so the size should not
    // be increased here
    jar2.emplace_back("one", "value 1-3");
    jar2.push_back(res_cookie_t("one", "value 1-4"));
    res_cookie_t c("one", "value 1-5");
    jar2.push_back(c);
    jar2.push_back(c);
    jar2.push_back(res_cookie_t("one", "value 1-6"));

    res_cookie_jar_t jar3;
    jar3.emplace_back("one", "value 1-7");
    jar3.emplace_back("two", "value 2-3");
    jar2.push_back(res_cookie_t("one", "value 1-8"));
    jar2.push_back(res_cookie_t("two", "value 2-4"));

    EXPECT_TRUE(jar2.size() == 2)
      << "Cookie jar should have the same size when we're emplacing a cookie "
         "with the same name";

    auto found = jar2.find("two");
    EXPECT_TRUE(found->name() == "two");
    EXPECT_TRUE(found->value() == "value 2-4")
      << "The value we found is not the same as the value it should have "
         "(found: "
      << found->value() << "; expected: "
      << "value 2-4"
      << ").";

    for (auto const& a : jar2) {
        auto found = jar2.find(a.name());
        EXPECT_TRUE(found->value() == a.value())
          << "Checking all the values in the cookie jar (cookie name: "
          << a.name() << "=" << a.value() << "; found: " << found->name() << "="
          << found->value() << ")";
    }
}

TEST(Cookies, CookieJarUniqeness) {
    res_cookie_jar_t cs;
    cs.push_back(res_cookie_t().name("one").value("test").domain("google.com"));
    cs.push_back(res_cookie_t().name("one").value("test").domain("bing.com"));

    EXPECT_TRUE(cs.size() == 2)
      << "Different domains should not be considered the same";

    cs.push_back(res_cookie_t().name("one").value("test").domain("google.com"));
    cs.push_back(res_cookie_t().name("one").value("test").domain("bing.com"));

    EXPECT_TRUE(cs.size() == 2)
      << "Inserting already push_backed cookies that are 'same_as' the other "
         "one should be ignored";

    // now we check if changing the name, path, or domain to a new value that
    // already exists will remove the value or not
    cs.push_back(res_cookie_t()
                   .name("two")
                   .value("test")
                   .domain("bing.com")
                   .comment("hello"));
    EXPECT_EQ(cs.size(), 3);
    EXPECT_EQ(cs.find("two")->comment(), "hello");
    cs.name("two", "one");

    EXPECT_EQ(cs.size(), 2)
      << "One of the cookies should now be removed so the whole cookie jar "
         "have unique cookies";
    EXPECT_EQ(cs.find("one")->comment(), "hello")
      << "The old cookie should be removed instead of the new one. The new "
         "cookie should be replace the old one while renaming.";

    cs.push_back(res_cookie_t()
                   .name("one")
                   .value("test")
                   .domain("duckduckgo.com")
                   .comment("hello"));
    auto p = std::find_if(cs.begin(), cs.end(), [](auto const& cookie) {
        return cookie.name() == "one";
    });
    EXPECT_EQ(cs.size(), 3);
    EXPECT_EQ(p->domain(), "duckduckgo.com");
    cs.domain(p, "google.com");
    EXPECT_EQ(p->domain(), "google.com");

    EXPECT_EQ(cs.size(), 2)
      << "One of the cookies should now be removed so the whole cookie jar "
         "have unique cookies";
    EXPECT_EQ(cs.find("one")->comment(), "hello")
      << "The old cookie should be removed instead of the new one. The new "
         "cookie should be replace the old one in the changing the domain "
         "process.";

    cs.domain(
      [](auto const& cookie) {
          return cookie.domain() == "google.com";
      },
      "something-else.com");
    EXPECT_NE(std::find_if(cs.begin(), cs.end(),
                           [](auto const& cookie) {
                               return cookie.domain() == "something-else.com";
                           }),
              std::end(cs));
}

TEST(Cookies, Date) {
}

TEST(Cookies, StringParsing) {

    res_cookie_t c("name=value");
    EXPECT_TRUE(c.is_valid());
    EXPECT_EQ("name", c.name());
    EXPECT_EQ("value", c.value());
}

TEST(Cookies, CookiesEncryption) {
    // TODO
}
