// Created by moisrex on 5/12/23.
#include "../webpp/uri/domain.hpp"

#include "common_pch.hpp"

using namespace webpp;

std::array<std::string_view, 90> valid_domains = {
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
  "example.com.",
  "example.com:8080",
  "example.com:80",
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
  "example.tel",
  "example.travel",
  "example.xxx"
  "example.abcdefghijklmnopqrstuvwxyz0123456789",
  "example.a.b.c.d.e.f.g.h.i.j.k.l.m.n.o.p.q.r.s.t.u.v.w.x.y.z",
};

std::array<std::string_view, 48> invalid_domains = {
  "-example.com",    "example-.com",
  "example.com-",    "example..com",
  "example.com.",    "example_.com",
  "example.com_",    "example_com",
  "example..com",    "example#com",
  "example&com",     "example!com",
  "example+com",     "example=com",
  "example~com",     "example(com",
  "example)com",     "example{com",
  "example}com",     "example[com",
  "example]com",     "example<com",
  "example>com",     "example\"com",
  "example'com",     "example\\com",
  "example|com",     "example/abc.com",
  "example@com",     "example.com:80",
  "example.com:xyz", "example.com:",
  "example..com",    "example.com..",
  "example..com..",  "example.com-abc",
  "example.1234",    "example.-com",
  "example-.com",    "example-.com.",
  "example-.com-",   "example.coma",
  "example.com.",    "example.com..",
  "example..com",    "example.com...",
  "example.com1",    "example.com123456789012345678901234567890123456789012345678901234567890"};



TEST(DomainsTest, Validity) {
    for (auto domain_str : valid_domains) {
        auto ptr    = domain_str.data();
        auto status = parse_domain_name(ptr, ptr + domain_str.size());
        EXPECT_EQ(status, domain_name_status::valid) << domain_str;
    }
}


TEST(DomainsTest, InValidity) {
    for (auto domain_str : invalid_domains) {
        auto ptr    = domain_str.data();
        auto status = parse_domain_name(ptr, ptr + domain_str.size());
        EXPECT_NE(status, domain_name_status::valid) << domain_str;
    }
}
