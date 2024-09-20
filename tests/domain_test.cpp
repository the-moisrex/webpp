// Created by moisrex on 5/12/23.
#include "../webpp/uri/domain.hpp"

#include "common/tests_common_pch.hpp"

#include <cstring>
using namespace webpp;
using namespace webpp::uri;

static constexpr auto valid_domains = stl::array{
  "example.com",
  "google.com",
  "github.com",
  "wikipedia.org",
  "stackoverflow.com",
  "yahoo.com",
  "amazon.com",
  "microsoft.com",
  "linkedin.com",
  "reddit.com",
  "instagram.com",
  "twitter.com",
  "facebook.com",
  "netflix.com",
  "ebay.com",
  "craigslist.org",
  "spotify.com",
  "apple.com",
  "pinterest.com",
  "tumblr.com",
  "wordpress.com",
  "bbc.co.uk",
  "nytimes.com",
  "cnn.com",
  "foxnews.com",
  "forbes.com",
  "walmart.com",
  "bestbuy.com",
  "homedepot.com",
  "target.com",
  "ikea.com",
  "nasa.gov",
  "usda.gov",
  "whitehouse.gov",
  "irs.gov",
  "cdc.gov",
  "nih.gov",
  "who.int",
  "un.org",
  "europa.eu",
  "nato.int",
  "icann.org",
  "ietf.org",
  "iso.org",
  "ieee.org",
  "acm.org",
  "ietf.org",
  "dnssec.net",
  "localhost",
  "example.com",
  "example.co.uk",
  "example.org",
  "www.example.com",
  "a.example.com",
  "a.b.example.com",
  "xn--h1aegh.com",
  "example.com-foo.com",
  "foo.example.com",
  "example.00",
  "example.1",
  "example.123",
  "example.1234",
  "example.12345",
  "example.123456",
  "example.1234567",
  "example.com",
  "example.net",
  "example.org",
  "example.edu",
  "example.gov",
  "example.mil",
  "example.int",
  "example.arpa",
  "example.info",
  "example.biz",
  "example.name",
  "example.pro",
  "example.coop",
  "example.mobi",
  "example.asia",
  "example.cat",
  "example.jobs",
  "example.com-abc",
  "example.tel",
  "example.coma",
  "example.travel",
  "example.1234",
  "example.xxx",
  "14159265358979323846264338327950288419716939937510582097494459.com",
  "example.abcdefghijklmnopqrstuvwxyz0123456789",
  "example.a.b.c.d.e.f.g.h.i.j.k.l.m.n.o.p.q.r.s.t.u.v.w.x.y.z",
  "example.com1",
  "example.com123456789012345678901234567890123456789012345678901234567890"};

static constexpr auto invalid_domains = stl::array{
  "-example.com",
  "example-.com",
  "example.com-",
  "example..com",
  "example.com.",
  "example_.com",
  "example.com_",
  "example_com",
  "example..com",
  "example#com",
  "example&com",
  "example!com",
  "example+com",
  "example=com",
  "example~com",
  "example(com",
  "example)com",
  "example{com",
  "example}com",
  "example[com",
  "example]com",
  "example<com",
  "example>com",
  "example\"com",
  "example'com",
  "example\\com",
  "example|com",
  "example/abc.com",
  "example@com",
  "example.com:80",
  "example.com.",
  "example.com:8080",
  "example.com:xyz",
  "example.com:",
  "example..com",
  "example.com..",
  "example..com..",
  "example.-com",
  "example-.com",
  "example-.com.",
  "example-.com-",
  "example.com.",
  "example.com..",
  "example..com",
  "example.com...",
  "0123456789"      // 10
  "0123456789"      // 20
  "0123456789"      // 30
  "0123456789"      // 40
  "0123456789"      // 50
  "0123456789"      // 60
  "0123456789.com", // 70 (only 63 chars are allowed in one subdomain)
  "dev.0123456789"  // 10
  "0123456789"      // 20
  "0123456789"      // 30
  "0123456789"      // 40
  "0123456789"      // 50
  "0123456789"      // 60
  "0123456789"      // 70 (only 63 chars are allowed in one subdomain)
};

// fixme: since these are AI generated, they have the wrong values.
std::array<std::pair<std::string_view, std::string_view>, 60> puny_domains = {
  {{"xn--4gbrim.xn----rmckbbajlc6dj7bxne2c.xn--wgbh1c", "موقع.وزارة-الأتصالات.مصر"},
   {"xn--80ahbprimd0a.xn--p1ai", "мдпанитпд.рф"},
   {"xn--fzys8d69uvgm.xn--estv75g", "電訊盈科.工行"},
   {"xn--3ds443g.xn--fiqz9s", "ไทย.คอม"},
   {"xn--mgba3a4f16a.xn--mgbaam7a8h", "السعودية.موبايلي"},
   {"xn--kpry57d.xn--zckzah", "இந்தியா.நிறுவனம்"},
   {"xn--mgbaakc7dvf.xn--j1amh", "الإمارات.شبكة"},
   {"xn--80adxhks.xn--p1ai", "сайт.рф"},
   {"xn--80aqecdr1a.xn--p1ai", "пример.рф"},
   {"xn--80aizm.xn--p1ai", "сайт.рф"},
   {"xn--80aaigiyb.xn--p1ai", "сайт.рф"},
   {"xn--80aacf4bwnk.xn--p1ai", "сайт.рф"},
   {"xn--80ahc0b.xn--p1ai", "москва.рф"},
   {"xn--j1aef.xn--h2brj9c", "укр"},
   {"xn--h2brj9c.xn--mgba3aso", "ایران.ایر"},
   {"xn--mgbpl2fh.xn--mgbaam7a8h", "البحرين.كوم"},
   {"xn--kprw13d.xn--kpry57d", "இந்தியா.இலவசம்"},
   {"xn--mgba3a4fra.xn--mgbaam7a8h", "المغرب.موبايلي"},
   {"xn--80ajg.xn--j1amh", "сайт.рус"},
   {"xn--90ais.xn--p1ai", "дети.рф"},
   {"xn--80affb7d.xn--p1ai", "комсомольск-на-амуре.рф"},
   {"xn--80abucjiibhv9a.xn--p1ai", "магазин.рф"},
   {"xn--o1ach.xn--p1ai", "бухгалтер.рф"},
   {"xn--80aa3adxha4f.xn--p1ai", "коломна.рф"},
   {"xn--90aiifbl0c.xn--p1ai", "екатеринбург.рф"},
   {"xn--j1amh.xn--kpry57d", "рус"},
   {"xn--80ahbq2aqb.xn--p1ai", "москва.рф"},
   {"xn--80aaafb5bakeeh.xn--p1ai", "медведково.рф"},
   {"xn--80aaahwbee3a.xn--p1ai", "подольск.рф"},
   {"xn--80acgfbsl1azdqr.xn--p1ai", "юрлицо.рф"},
   {"xn--h1akwe.xn--p1ai", "сайт.рф"},
   {"xn--80aqecdr1a.xn--p1ai", "пример.рф"},
   {"xn--d1abbgf6aiiy.xn--p1ai", "сайт.рф"},
   {"xn--80ahbq2aqb.xn--p1ai", "москва.рф"},
   {"xn--80acgfbsl1azdqr.xn--p1ai", "юрлицо.рф"},
   {"xn--h1alffa9f.xn--p1ai", "сайт.рф"},
   {"xn--80aaafb5bakeeh.xn--p1ai", "медведково.рф"},
   {"xn--80affb7d.xn--p1ai", "комсомольск-на-амуре.рф"},
   {"xn--b1agd0aean.xn--p1ai", "компания.рф"},
   {"xn--j1aeef4c.xn--p1ai", "сайт.рф"},
   {"xn--80ahdheogk5l.xn--p1ai", "юрист.рф"},
   {"xn--e1agmkjdg.xn--p1ai", "бизнес.рф"},
   {"xn--90ais.xn--p1ai", "дети.рф"},
   {"xn--c1avg.xn--p1ai", "авто.рф"},
   {"xn--90a3ac.xn--p1ai", "москва.рф"},
   {"xn--o1ach.xn--p1ai", "бухгалтер.рф"},
   {"xn--80a1bd.xn--p1ai", "сайт.рф"},
   {"xn--90aiifbl0c.xn--p1ai", "екатеринбург.рф"},
   {"xn--80aaigiyb.xn--p1ai", "сайт.рф"},
   {"xn--80aizm.xn--p1ai", "сайт.рф"},
   {"xn--h1ahd.xn--p1ai", "сайт.рф"},
   {"xn--80abucjiibhv9a.xn--p1ai", "магазин.рф"},
   {"xn--80aa3adxha4f.xn--p1ai", "коломна.рф"},
   {"xn--c1aqndc.xn--p1ai", "доставка.рф"},
   {"xn--h1akdx.xn--p1ai", "сайт.рф"},
   {"xn--80aacf4bwnk.xn--p1ai", "сайт.рф"},
   {"xn--h1akwe.xn--p1ai", "сайт.рф"},
   {"xn--80aaahwbee3a.xn--p1ai", "подольск.рф"},
   {"xn--80adxhks.xn--p1ai", "сайт.рф"},
   {"xn--80ahc0b.xn--p1ai", "москва.рф"}}
};

TEST(DomainsTest, Validity) {
    for (auto domain_str : valid_domains) {
        auto const ptr    = domain_str;
        auto const status = parse_domain_name(ptr, ptr + std::strlen(ptr));
        EXPECT_TRUE(status == domain_name_status::valid || status == domain_name_status::valid_punycode)
          << domain_str << "\n"
          << to_string(status);
    }
}

TEST(DomainsTest, ValidityPunycode) {
    for (auto const& [domain_str, domain_unicode] : puny_domains) {
        auto const ptr    = domain_str.data();
        auto const status = parse_domain_name(ptr, ptr + domain_str.size());
        EXPECT_TRUE(status == domain_name_status::valid_punycode) << domain_str << "\n" << to_string(status);
    }
}

TEST(DomainsTest, InValidity) {
    for (auto domain_str : invalid_domains) {
        auto const ptr    = domain_str;
        auto const status = parse_domain_name(ptr, ptr + std::strlen(ptr));
        EXPECT_NE(status, domain_name_status::valid) << domain_str << "\n" << to_string(status);
    }
}

TEST(DomainsTest, SubDomainTooLongError) {
    std::string str = "";
    for (int i = 0; i <= 64; ++i) {
        str += 'd';
    }
    str                += "example.com";
    auto const str_beg   = static_cast<char const*>(str.data());
    auto const str_end  = str_beg + str.size();
    auto const status    = parse_domain_name(str_beg, str_end);
    EXPECT_EQ(status, domain_name_status::subdomain_too_long) << str << "\n" << to_string(status);
}

TEST(DomainsTest, DomainTooLongError) {
    std::string str = "";
    for (int i = 0; i <= 255 / 2; ++i) {
        str += 'd';
        str += '.';
    }
    str                += "com";
    auto const str_beg   = static_cast<char const*>(str.data());
    auto const str_end  = str_beg + str.size();
    auto const status    = parse_domain_name(str_beg, str_end);
    EXPECT_EQ(status, domain_name_status::too_long) << str << "\n" << to_string(status);
}

TEST(DomainsTest, TLDTest) {
    basic_domain const domain{"example.com"};
    EXPECT_EQ(domain.tld(), "com");
}
