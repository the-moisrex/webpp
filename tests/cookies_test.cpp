#include "../webpp/http/cookies/request_cookie_jar.hpp"
#include "../webpp/http/cookies/response_cookie_jar.hpp"
#include "../webpp/traits/std_traits.hpp"
#include "common/tests_common_pch.hpp"

using namespace webpp;
using namespace webpp::http;

using res_cookie_t     = response_cookie<>;
using res_cookie_jar_t = response_cookie_jar<>;
using req_cookie_jar_t = request_cookie_jar<>;

TEST(Cookie, ResponseCookiesCreation) {
    res_cookie_t c;
    c.name(" test ");
    c.value("  value ");
    c.trim_name();
    c.trim_value();
    EXPECT_EQ(c.name(), "test");
    EXPECT_EQ(c.value(), "value");
    //    EXPECT_EQ(res_cookie_t{.name = "  test  ", .value = "  value "}.trimname().trimvalue().name,
    //              res_cookie_t{.name = "test", .value = "value"}.name)
    //      << "cookies should be trimmed";
    //    EXPECT_EQ(res_cookie_t("  test  ", "  value  ").name(), res_cookie_t().name("  test  ").name())
    //      << "name should trim it too.";
}

TEST(Cookie, SetCookieHeaderParsing) {
    res_cookie_t const c{
      "_HFID=hfid:86715526; httponly; expires=Fri, 21 Jan 2022 16:08:20 GMT; secure; path=/;"};
    EXPECT_EQ(c.name(), "_HFID");
    EXPECT_EQ(c.value(), "hfid:86715526");
    EXPECT_EQ(c.path(), "/");
    EXPECT_TRUE(c.secure());
    EXPECT_TRUE(c.http_only());
    EXPECT_EQ(c.expires_string(), "Fri, 21 Jan 2022 16:08:20 GMT");

    res_cookie_t const c1{"name = value ;   HttpOnly; path=/about  "};
    EXPECT_EQ(c1.name(), "name");
    EXPECT_EQ(c1.value(), "value");
    EXPECT_TRUE(c1.http_only());
    EXPECT_FALSE(c1.secure());
    EXPECT_EQ(c1.path(), "/about");
}

TEST(Cookie, RequestCookieJarParser) {
    req_cookie_jar_t jar;
    EXPECT_TRUE(jar.parse("one=1; two= \"2\"; three=\"3\""));
    ASSERT_EQ(jar.size(), 3);
    EXPECT_EQ(jar[0].name(), "one");
    EXPECT_EQ(jar[0].value(), "1");
    EXPECT_EQ(jar[1].name(), "two");
    EXPECT_EQ(jar[1].value(), "2");
    EXPECT_EQ(jar[2].name(), "three");
    EXPECT_EQ(jar[2].value(), "3");
}

// TODO: fill here
TEST(Cookie, CookieExpirationDate) {
    res_cookie_t c;
    c.name("name");
    c.value("value");
    c.expires_in(std::chrono::minutes(1));
    EXPECT_TRUE(c.expires()->time_since_epoch().count() > 0);
}

// TEST(ResponseCookies, ResponseCookiesHash) {
//    cookie_hash_t hash;
//    auto          a = hash(res_cookie_t("yes", "value"));
//    auto          b = hash(res_cookie_t("  yes  ", "  value  "));
//
//    EXPECT_FALSE(hash(res_cookie_t("one", "value")) ==
//                 hash(res_cookie_t("two", "value")));
//    EXPECT_TRUE(a == b) << "Same cookies should be the same.";
//}
//
TEST(ResponseCookies, CookieJar) {

    res_cookie_jar_t jar;
    jar.emplace_back("one", "value");
    jar.emplace_back(" one ", "value 2"); // this should replace the other one
    jar.emplace_back("two", "value");
    jar.remove_duplicates();

    EXPECT_EQ(jar.size(), 2) << "cookies with the same name should be replaced with the older ones";

    res_cookie_jar_t jar2;
    jar2.emplace_back("one", "value 1");
    jar2.emplace_back("two", "value 2");
    jar2.emplace_back(" one ",
                      "value 1-2"); // this should replace the other one
    jar2.remove_duplicates();

    EXPECT_EQ(jar2.size(), 2) << "The order that cookies get added to cookie jar does not matter";

    jar2.emplace_back("two", "value 2-2");

    // These all should be replaced with the first one; so the size should not
    // be increased here
    jar2.emplace_back("one", "value 1-3");
    jar2.push_back(res_cookie_t("one", "value 1-4"));
    res_cookie_t const c("one", "value 1-5");
    jar2.push_back(c);
    jar2.push_back(c);
    jar2.push_back(res_cookie_t("one", "value 1-6"));

    res_cookie_jar_t jar3;
    jar3.emplace_back("one", "value 1-7");
    jar3.emplace_back("two", "value 2-3");
    jar2.push_back(res_cookie_t("one", "value 1-8"));
    jar2.push_back(res_cookie_t("two", "value 2-4"));

    jar2.remove_duplicates();
    EXPECT_EQ(jar2.size(), 2) << "Cookie jar should have the same size when we're emplacing a cookie "
                                 "with the same name";

    auto found = jar2.find("two");
    EXPECT_EQ(found->name(), "two");
    EXPECT_EQ(found->value(), "value 2-4")
      << "The value we found is not the same as the value it should have "
         "(found: "
      << found->value() << "; expected: "
      << "value 2-4"
      << ").";

    for (auto const& a : jar2) {
        auto found_it = jar2.find(a.name());
        EXPECT_TRUE(found_it->value() == a.value())
          << "Checking all the values in the cookie jar (cookie name: " << a.name() << "=" << a.value()
          << "; found: " << found_it->name() << "=" << found_it->value() << ")";
    }
}

TEST(ResponseCookies, CookieJarUniqeness) {
    res_cookie_jar_t cs;
    cs.push_back(res_cookie_t().name("one").value("test").domain("google.com"));
    cs.push_back(res_cookie_t().name("one").value("test").domain("bing.com"));
    cs.remove_duplicates();

    EXPECT_TRUE(cs.size() == 2) << "Different domains should not be considered the same";

    cs.push_back(res_cookie_t().name("one").value("test").domain("google.com"));
    cs.push_back(res_cookie_t().name("one").value("test").domain("bing.com"));
    cs.remove_duplicates();

    EXPECT_EQ(cs.size(), 2) << "Inserting already push_backed cookies that are 'same_as' the other "
                               "one should be ignored";

    // now we check if changing the name, path, or domain to a new value that
    // already exists will remove the value or not
    cs.push_back(res_cookie_t().name("two").value("test").domain("bing.com").comment("hello"));
    EXPECT_EQ(cs.size(), 3);
    EXPECT_EQ(cs.find("two")->comment(), "hello");
    cs.name("two", "one");
    cs.remove_duplicates();

    EXPECT_EQ(cs.size(), 2) << "One of the cookies should now be removed so the whole cookie jar "
                               "have unique cookies";
    EXPECT_EQ(cs.find("one")->comment(), "hello")
      << "The old cookie should be removed instead of the new one. The new "
         "cookie should be replace the old one while renaming.";

    cs.push_back(res_cookie_t().name("one").value("test").domain("duckduckgo.com").comment("hello"));
    cs.remove_duplicates();
    auto p = std::find_if(cs.rbegin(), cs.rend(), [](auto const& cookie) {
        return cookie.name() == "one";
    });
    EXPECT_EQ(cs.size(), 3);
    EXPECT_EQ(p->domain(), "duckduckgo.com");
    cs.domain(p, "google.com");
    EXPECT_EQ(p->domain(), "google.com");

    EXPECT_EQ(cs.size(), 2) << "One of the cookies should now be removed so the whole cookie jar "
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
    EXPECT_NE(std::find_if(cs.begin(),
                           cs.end(),
                           [](auto const& cookie) {
                               return cookie.domain() == "something-else.com";
                           }),
              std::end(cs));
}

TEST(ResponseCookies, Date) {}

TEST(ResponseCookies, ResponseCookiesEncryption) {
    // TODO
}
