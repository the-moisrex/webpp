#include "../core/include/webpp/http/cookies/cookie.h"
#include "../core/include/webpp/traits/std_traits.h"

#include <gtest/gtest.h>
#include <iostream>


using cookie_t = webpp::response_cookie<webpp::std_traits>;

TEST(Cookie, CookiesCreation) {
    cookie_t c;
    c.name("   test   ").value("  value  ");
    EXPECT_EQ(c.name(), "test");
    EXPECT_EQ(c.value(), "value");
    EXPECT_EQ(cookie_t("  test  ", "  value ").name(),
              cookie_t("test", "value").name())
      << "cookies should be trimmed";
    EXPECT_EQ(cookie_t("  test  ", "  value  ").name(),
              cookie_t().name("  test  ").name())
      << "name should trim it too.";
}

// TODO: fill here
TEST(Cookie, CookieExpirationDate) {
    cookie_t c;
    c.name("name").value("value");
    c.expires_in(std::chrono::minutes(1));
    EXPECT_TRUE(c.expires().time_since_epoch().count() > 0);
}

TEST(Cookies, CookiesHash) {
    webpp::cookie_hash<webpp::std_traits> hash;
    auto                                  a = hash(cookie_t("yes", "value"));
    auto b = hash(cookie_t("  yes  ", "  value  "));

    EXPECT_FALSE(hash(cookie_t("one", "value")) ==
                 hash(cookie_t("two", "value")));
    EXPECT_TRUE(a == b) << "Same cookies should be the same.";
}

TEST(Cookies, CookieJar) {

    webpp::basic_cookie_jar<webpp::std_traits> jar;
    jar.emplace("one", "value");
    jar.emplace(" one ", "value 2"); // this should replace the other one
    jar.emplace("two", "value");

    EXPECT_TRUE(jar.size() == 2)
      << "cookies with the same name should be replaced with the older ones";

    webpp::basic_cookie_jar<webpp::std_traits> jar2;
    jar2.emplace("one", "value 1");
    jar2.emplace("two", "value 2");
    jar2.emplace(" one ", "value 1-2"); // this should replace the other one

    EXPECT_TRUE(jar2.size() == 2)
      << "The order that cookies get added to cookie jar does not matter";

    jar2.emplace("two", "value 2-2");

    // These all should be replaced with the first one; so the size should not
    // be increased here
    jar2.emplace_hint(jar2.begin(), "one", "value 1-3");
    jar2.insert(cookie_t("one", "value 1-4"));
    cookie_t c("one", "value 1-5");
    jar2.insert(c);
    jar2.insert(jar2.begin(), c);
    jar2.insert(jar2.begin(), cookie_t("one", "value 1-6"));

    webpp::basic_cookie_jar<webpp::std_traits> jar3;
    jar3.emplace("one", "value 1-7");
    jar3.emplace("two", "value 2-3");
    jar2.insert(jar3.begin(), jar3.end());
    jar2.insert({cookie_t("one", "value 1-8"), cookie_t("two", "value 2-4")});

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
    webpp::basic_cookie_jar<webpp::std_traits> cs;
    cs.insert(cookie_t().name("one").value("test").domain("google.com"));
    cs.insert(cookie_t().name("one").value("test").domain("bing.com"));

    EXPECT_TRUE(cs.size() == 2)
      << "Different domains should not be considered the same";

    cs.insert(cookie_t().name("one").value("test").domain("google.com"));
    cs.insert(cookie_t().name("one").value("test").domain("bing.com"));

    EXPECT_TRUE(cs.size() == 2)
      << "Inserting already inserted cookies that are 'same_as' the other "
         "one should be ignored";

    // now we check if changing the name, path, or domain to a new value that
    // already exists will remove the value or not
    cs.insert(
      cookie_t().name("two").value("test").domain("bing.com").comment("hello"));
    EXPECT_EQ(cs.size(), 3);
    EXPECT_EQ(cs.find("two")->comment(), "hello");
    cs.name("two", "one");

    EXPECT_EQ(cs.size(), 2)
      << "One of the cookies should now be removed so the whole cookie jar "
         "have unique cookies";
    EXPECT_EQ(cs.find("one")->comment(), "hello")
      << "The old cookie should be removed instead of the new one. The new "
         "cookie should be replace the old one while renaming.";

    auto p = cs.insert(cookie_t()
                         .name("one")
                         .value("test")
                         .domain("duckduckgo.com")
                         .comment("hello"));
    EXPECT_EQ(cs.size(), 3);
    EXPECT_EQ(p.first->domain(), "duckduckgo.com");
    cs.domain(p.first, "google.com");
    EXPECT_EQ(p.first->domain(), "google.com");

    EXPECT_EQ(cs.size(), 2)
      << "One of the cookies should now be removed so the whole cookie jar "
         "have unique cookies";
    EXPECT_EQ(cs.find("one")->comment(), "hello")
      << "The old cookie should be removed instead of the new one. The new "
         "cookie should be replace the old one in the changing the domain "
         "process.";
}

TEST(Cookies, Date) {
}

TEST(Cookies, StringParsing) {

    cookie_t c("name=value");
    EXPECT_TRUE(c.is_valid());
    EXPECT_EQ("name", c.name());
    EXPECT_EQ("value", c.value());
}

TEST(Cookies, CookiesEncryption) {
    // TODO
}
