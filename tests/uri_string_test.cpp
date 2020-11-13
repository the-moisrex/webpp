#include "../core/include/webpp/traits/std_traits.hpp"
#include "../core/include/webpp/uri/uri_string.hpp"

#include "./common_pch.hpp"
#include <string>

using namespace webpp;

TEST(URITests, Creation) {
    // using set and get methods twice in a row should not affect the outcome

    uri u("http://example.com/");
    EXPECT_TRUE(u.has_scheme());
    EXPECT_TRUE(!u.host.empty());
    EXPECT_EQ(u.str(), "http://example.com/");
    EXPECT_EQ(u.host.raw(), "example.com");
    EXPECT_TRUE(is::host(u.host.raw()));
    EXPECT_TRUE(u.has_authority());
    EXPECT_TRUE(!u.path.empty()) << "the path is '/'";
    EXPECT_FALSE(u.has_port());
    EXPECT_FALSE(u.has_user_info());
    EXPECT_FALSE(u.has_fragment());
    EXPECT_FALSE(!u.queries.empty());
    EXPECT_EQ(u.path.raw_slugs().size(), 1);
    EXPECT_EQ(u.scheme(), "http");
    u.clear_scheme();
    u.clear_scheme();
    EXPECT_FALSE(u.has_scheme());
    EXPECT_EQ(u.scheme(), "");
    EXPECT_TRUE(u.has_authority());
    EXPECT_TRUE(!u.host.empty());
    EXPECT_TRUE(!u.path.empty());
    EXPECT_FALSE(u.has_port());
    EXPECT_EQ(u.str(), "//example.com/");
    EXPECT_TRUE(u.path.is_normalized());
    u.host.clear();
    u.host.clear();
    EXPECT_TRUE(u.host.empty());
    EXPECT_EQ(u.host.raw(), "");
    EXPECT_EQ(u.str(), "/");
    u.path = "folder/file";
    EXPECT_EQ(u.str(), "/folder/file");
    u.path = "folder/file";
    EXPECT_TRUE(u.host.empty());
    u.host = "eg2.com";
    u.host = "eg2.com";
    EXPECT_FALSE(u.host.empty());
    EXPECT_TRUE(!u.path.empty());
    EXPECT_EQ(u.host.raw(), "eg2.com") << "host is: " << u.host.raw();
    EXPECT_EQ(u.str(), "//eg2.com/folder/file") << "str is: " << u.str();
    u.scheme("https:");
    u.scheme("https:");
    EXPECT_TRUE(u.has_scheme());
    u.path.clear();
    u.path.clear();
    EXPECT_EQ(u.str(), "https://eg2.com/");
    u.scheme("http");
    u.scheme("http");
    EXPECT_TRUE(u.has_scheme());
    EXPECT_EQ(u.str(), "http://eg2.com/");

    const_uri ipv4_host("https://192.168.1.1");
    EXPECT_TRUE(is::ipv4(ipv4_host.host.raw()));
    EXPECT_EQ(ipv4_host.scheme(), "https");
    EXPECT_FALSE(!ipv4_host.path.empty());
    EXPECT_FALSE(!ipv4_host.queries.empty());
    EXPECT_FALSE(ipv4_host.has_port());
    EXPECT_TRUE(ipv4_host.has_authority());
    EXPECT_FALSE(ipv4_host.host.empty());
    EXPECT_EQ(ipv4_host.host.raw(), "192.168.1.1");

    uri local_file("file:///home/test/folder/file.txt");
    EXPECT_EQ(local_file.path.raw(), "/home/test/folder/file.txt");
    EXPECT_TRUE(!local_file.path.empty());
    EXPECT_TRUE(local_file.has_scheme());
    EXPECT_FALSE(local_file.has_authority());
    EXPECT_TRUE(local_file.host.empty());
    EXPECT_EQ(local_file.scheme(), "file");
    EXPECT_EQ(local_file.host.decoded(), "");
    auto path = local_file.path.slugs();
    EXPECT_EQ(local_file.path.raw_slugs().size(), 5);
    EXPECT_EQ(path.size(), 5);
    EXPECT_EQ(path.at(0), "");
    EXPECT_EQ(path.at(1), "home");
    EXPECT_EQ(path.at(2), "test");
    EXPECT_EQ(path.at(3), "folder");
    EXPECT_EQ(path.at(4), "file.txt");
    EXPECT_TRUE(local_file.path.is_absolute());
    EXPECT_TRUE(local_file.path.is_normalized());
    EXPECT_FALSE(local_file.path.is_relative());
    local_file.path.clear();
    EXPECT_EQ(local_file.str(), "file:///");
}

TEST(URITests, IPv6HostName) {
    uri         u;
    std::string uri_str =
      "//[::1]:8080/folder/file.md?name=value&name2=value2#str";
    u = uri_str;
    EXPECT_EQ(u.str(), uri_str);
    EXPECT_FALSE(u.has_scheme()) << "scheme: " << u.scheme();
    EXPECT_FALSE(u.host.empty());
    EXPECT_TRUE(u.has_port());
    EXPECT_TRUE(u.has_authority());
    EXPECT_TRUE(!u.path.empty()) << "path: " << u.path.raw();
    EXPECT_TRUE(!u.queries.empty());
    EXPECT_TRUE(u.has_fragment());
    EXPECT_EQ(u.fragment(), "str");
    EXPECT_EQ(u.path.raw(), "/folder/file.md") << "path: " << u.path.raw();
    EXPECT_EQ(u.host.raw(), "[::1]") << "host: " << u.host.raw();
    EXPECT_EQ(u.port_uint16(), 8080);
    EXPECT_EQ(u.port(), "8080");
    EXPECT_TRUE(std::holds_alternative<ipv6<std_traits>>(u.host.host.structured()))
      << "index: " << u.host.structured().index();
    u.path.clear();
    EXPECT_EQ(u.str(), "//[::1]:8080/?name=value&name2=value2#str");
}

TEST(URITests, WieredURIs) {
    const_uri u1("ftp://ftp.is.co.za/rfc/rfc1808.txt");
    EXPECT_FALSE(u1.host.empty());
    EXPECT_TRUE(u1.has_scheme());
    EXPECT_EQ(u1.scheme(), "ftp");
    EXPECT_EQ(u1.host.raw(), "ftp.is.co.za") << "host: " << u1.host.raw();
    EXPECT_FALSE(u1.host.raw().empty());
    EXPECT_NE(u1.host.raw(), "");
    EXPECT_TRUE(!u1.path.empty());
    EXPECT_EQ(u1.path.raw(), "/rfc/rfc1808.txt") << "path: " << u1.path.raw();
    EXPECT_TRUE(u1.is_url());
    EXPECT_TRUE(u1.is_valid());
    EXPECT_FALSE(u1.is_urn());

    // some examples from https://rosettacode.org/wiki/URL_parser
    auto _uris = {
      "ftp://ftp.is.co.za/rfc/rfc1808.txt",
      "http://www.ietf.org/rfc/rfc2396.txt",
      "ldap://[2001:db8::7]/c=GB?objectClass?one",
      "mailto:John.Doe@example.com",
      "news:comp.infosystems.www.servers.unix",
      "tel:+1-816-555-1212",
      "telnet://192.0.2.16:80/",
      "urn:oasis:names:specification:docbook:dtd:xml:4.1.2",
      "foo://example.com:8042/over/there?name=ferret#nose",
      "urn:example:animal:ferret:nose",
      "jdbc:mysql://test_user:test@test.com:3306/sakila?profileSQL=true",
      "ftp://ftp.is.co.za/rfc/rfc1808.txt",
      "http://www.ietf.org/rfc/rfc2396.txt#header1",
      "ldap://[2001:db8::7]/c=GB?objectClass=one&objectClass=two",
      "mailto:example@email.com",
      "news:comp.infosystems.www.servers.unix",
      "tel:+1-816-555-1212",
      "telnet://192.0.2.16:80/",
      "urn:oasis:names:specification:docbook:dtd:xml:4.1.2",
      "ssh://test@test.com",
      "https://bob:pass@test.com/place",
      "http://example.com/?a=1&b=2+2&c=3&c=4&d=%65%6e%63%6F%64%65%64"};

    for (auto const& _uri : _uris) {
        EXPECT_TRUE(const_uri(_uri).is_valid()) << "uri: " << _uri;
    }

    uri not_port{"http://username:password@domain.tld/path/file.ext"};
    EXPECT_FALSE(not_port.has_port());
    EXPECT_TRUE(not_port.has_username());
    EXPECT_TRUE(not_port.has_password());
    EXPECT_EQ(not_port.username(), "username");
    EXPECT_EQ(not_port.password(), "password");
    EXPECT_EQ(not_port.port_uint16(), 80u);
}

TEST(URITests, URN) {
    auto valid_urns = {
      "urn:isbn:0451450523",
      "urn:uuid:6e8bc430-9c3a-11d9-9669-0800200c9a66",
      "urn:publishing:book",
      "urn:isbn:0451450523",
      "urn:isan:0000-0000-2CEA-0000-1-0000-0000-Y",
      "urn:ISSN:0167-6423",
      "urn:ietf:rfc:2648",
      "urn:mpeg:mpeg7:schema:2001",
      "urn:oid:2.16.840",
      "urn:uuid:6e8bc430-9c3a-11d9-9669-0800200c9a66",
      "urn:nbn:de:bvb:19-146642",
      "urn:lex:eu:council:directive:2010-03-09;2010-19-UE",
      "urn:lsid:zoobank.org:pub:CDC8D258-8F57-41DC-B560-247E17D3DC8C",
      "urn:mpeg:mpeg7:schema:2001urn:isbn:0451450523",
      "urn:sha1:YNCKHTQCWBTRNJIV4WNAE52SJUQCZO5C",
      "urn:uuid:6e8bc430-9c3a-11d9-9669-0800200c9a66"};

    for (auto const& _urn : valid_urns) {
        EXPECT_TRUE(uri(_urn).is_valid());
        EXPECT_TRUE(uri(_urn).is_urn());
        EXPECT_FALSE(uri(_urn).is_url());
    }

    const_uri a("urn:example:a123,z456");
    uri       b{"URN:example:a123,z456"};
    uri       c{"urn:EXAMPLE:a123,z456"};

    EXPECT_EQ(a, b);
    EXPECT_EQ(a, c);
    EXPECT_EQ(b, c);
    EXPECT_TRUE(a.is_urn());
    EXPECT_TRUE(b.is_urn());
    EXPECT_TRUE(c.is_urn());
}

TEST(URITests, URL) {
    // these are valid urls but they shouldn't be considers part of urn
    auto valid_urls = {"mailto:someone@example.com",
                       "http://foo.com/blah_blah",
                       "http://foo.com/blah_blah/",
                       "http://foo.com/blah_blah_(wikipedia)",
                       "http://foo.com/blah_blah_(wikipedia)_(again)",
                       "http://www.example.com/wpstyle/?p=364",
                       "https://www.example.com/foo/?bar=baz&inga=42&quux",
                       "http://✪df.ws/123",
                       "http://userid:password@example.com:8080",
                       "http://userid:password@example.com:8080/",
                       "http://userid@example.com",
                       "http://userid@example.com/",
                       "http://userid@example.com:8080",
                       "http://userid@example.com:8080/",
                       "http://userid:password@example.com",
                       "http://userid:password@example.com/",
                       "http://142.42.1.1/",
                       "http://142.42.1.1:8080/",
                       "http://➡.ws/䨹",
                       "http://⌘.ws",
                       "http://⌘.ws/",
                       "http://foo.com/blah_(wikipedia)#cite-1",
                       "http://foo.com/blah_(wikipedia)_blah#cite-1",
                       "http://foo.com/unicode_(✪)_in_parens",
                       "http://foo.com/(something)?after=parens",
                       "http://☺.ha-ha.com/",
                       "http://code.google.com/events/#&product=browser",
                       "http://j.mp",
                       "ftp://foo.bar/baz",
                       "http://foo.bar/?q=Test%20URL-encoded%20stuff",
                       "http://مثال.إختبار",
                       "http://例子.测试",
                       "http://उदाहरण.परीक्षा",
                       "http://-.~_!$&()*+,;=:%40:80%2f::::::@example.com",
                       "http://1337.net",
                       "http://a.b-c.de",
                       "http://223.255.255.254",
                       "http://0.0.0.0",
                       "http://10.1.1.0",
                       "http://10.1.1.255",
                       "http://224.1.1.1",
                       "http://1.1.1.1.1",
                       "http://123.123.123",
                       "http://3628126748",
                       "http://10.1.1.1",
                       "http://10.1.1.254",
                       "ftps://foo.bar/",
                       "http://-error-.invalid/",
                       "http://a.b--c.de/",
                       "http://-a.b.co",
                       "http://a.b-.co"};

    // these strings are not a valid URL (doesn't mean they are not a valid URI)
    // I'm kinda not sure about some of these; specially those with IP addresses
    auto invalid_urls = {"http://",
                         "http://.",
                         "http://..",
                         "http://../",
                         "http://?",
                         "http://??",
                         "http://?\?/",
                         "http://#",
                         "http://##",
                         "http://##/",
                         "http://foo.bar?q=Spaces should be encoded",
                         "//",
                         "//a",
                         "///a",
                         "///",
                         "http:///a",
                         "foo.com",
                         "rdar://1234",
                         "h://test",
                         "http:// should-fail.com",
                         ":// should fail",
                         "http://foo.bar/foo(bar)baz quux",
                         "http://.www.foo.bar/",
                         "http://www.foo.bar./",
                         "http://.www.foo.bar./"};

    for (auto const& u : valid_urls) {
        auto a = const_uri(u);
        EXPECT_TRUE(a.is_valid()) << u; // It's a valid URI
        EXPECT_TRUE(a.is_url()) << u;   // it's a valid URL too
        EXPECT_FALSE(a.is_urn()) << u;  // it shouldn't be a URN
    }

    for (auto const& u : invalid_urls) {
        auto a = const_uri(u);
        // It might be a valid URI
        EXPECT_FALSE(a.is_url()) << u; // it's a valid URL too
        EXPECT_FALSE(a.is_urn()) << u; // it shouldn't be a URN
    }
}

TEST(URITests, Set) {
    EXPECT_EQ(uri().scheme("ftp").str(), "ftp:");
    EXPECT_EQ(uri("mailto:someone@example.com").scheme("something_else").str(),
              "something_else:someone@example.com");

    // TODO: should this be allowed even???
    EXPECT_EQ(
      uri("urn:mpeg:mpeg7:schema:2001urn:isbn:0451450523").scheme("ftp").str(),
      "ftp:mpeg:mpeg7:schema:2001urn:isbn:0451450523");

    EXPECT_EQ(uri("http://example.com/").scheme("ftp").str(),
              "ftp://example.com/");
}

TEST(URITests, Domains) {
    uri u("http://coded.by.moisrex.localhost/path/to/something");
    EXPECT_FALSE(u.host.empty());
    EXPECT_EQ("coded.by.moisrex.localhost", u.host.raw());
    EXPECT_TRUE(u.has_subdomains());
    EXPECT_TRUE(u.has_top_level_domain());
    EXPECT_TRUE(u.has_second_level_domain());
    EXPECT_EQ("coded.by", u.subdomains());
    EXPECT_EQ("moisrex", u.second_level_domain());
    EXPECT_EQ("localhost", u.top_level_domain());
    auto domains = u.domains();
    EXPECT_EQ(domains.size(), 4);
    EXPECT_EQ("coded", domains[0]);
    EXPECT_EQ("by", domains[1]);
    EXPECT_EQ("moisrex", domains[2]);
    EXPECT_EQ("localhost", domains[3]);
    u.top_level_domain("dev");
    EXPECT_TRUE(u.has_host());
    EXPECT_TRUE(!u.path.empty());
    EXPECT_EQ("coded.by", u.subdomains());
    EXPECT_EQ("moisrex", u.second_level_domain());
    EXPECT_EQ("dev", u.top_level_domain());
    u.second_level_domain("god");
    EXPECT_EQ("god", u.second_level_domain());
    EXPECT_EQ("dev", u.top_level_domain());
    EXPECT_TRUE(u.has_host());
    EXPECT_TRUE(!u.path.empty());
    EXPECT_TRUE(u.has_scheme());
    EXPECT_EQ(u.scheme(), "http");
    u.clear_subdomains();
    EXPECT_FALSE(u.has_subdomains());
    EXPECT_TRUE(u.has_host());
    EXPECT_EQ("god.dev", u.host());
    EXPECT_EQ("god", u.second_level_domain());
    u.clear_second_level_domain();
    EXPECT_FALSE(u.has_second_level_domain());
    EXPECT_EQ("dev", u.host());
    u.subdomains("should.not.work");
    EXPECT_FALSE(u.has_second_level_domain());
    EXPECT_FALSE(u.has_subdomains());
    EXPECT_EQ("dev", u.host());
    u.second_level_domain("subdomain.cool");
    EXPECT_TRUE(u.has_second_level_domain());
    EXPECT_TRUE(u.has_subdomains());
    EXPECT_EQ("cool", u.second_level_domain());
    EXPECT_EQ("subdomain", u.subdomains());
    u.subdomains("sub");
    EXPECT_TRUE(u.has_second_level_domain());
    EXPECT_TRUE(u.has_subdomains());
    EXPECT_EQ("sub.cool.dev", u.host());
    u.clear_second_level_domain();
    EXPECT_FALSE(u.has_second_level_domain());
    EXPECT_EQ("dev", u.host());

    // tests for ip version 4
    u.host("192.168.1.1");
    EXPECT_EQ("192.168.1.1", u.host());
    EXPECT_TRUE(u.is_ip());
    EXPECT_FALSE(u.has_second_level_domain());
    EXPECT_FALSE(u.has_subdomains());
    EXPECT_FALSE(u.has_top_level_domain());
    u.top_level_domain("com");
    EXPECT_EQ("192.168.1.1", u.host());
    EXPECT_TRUE(u.is_ip());
    EXPECT_FALSE(u.has_second_level_domain());
    EXPECT_FALSE(u.has_subdomains());
    EXPECT_FALSE(u.has_top_level_domain());
    u.second_level_domain("com");
    EXPECT_EQ("192.168.1.1", u.host());
    EXPECT_TRUE(u.is_ip());
    EXPECT_FALSE(u.has_second_level_domain());
    EXPECT_FALSE(u.has_subdomains());
    EXPECT_FALSE(u.has_top_level_domain());
    u.subdomains("com");
    EXPECT_EQ("192.168.1.1", u.host());
    EXPECT_TRUE(u.is_ip());
    EXPECT_FALSE(u.has_second_level_domain());
    EXPECT_FALSE(u.has_subdomains());
    EXPECT_FALSE(u.has_top_level_domain());
}

TEST(URITests, EncodedDecoded) {
    // TODO
    uri u{"http://سلام.com"};
    EXPECT_TRUE(u.is_valid());

    const_uri ducky = "https://duckduckgo.com/?q=%D8%AA%D8%B3%D8%AA+%D9%85%DB%8C%DA%A9%D9%86%D9%85";
    EXPECT_EQ(ducky.queries.decoded()["q"], "تست میکنم");
}

TEST(URITests, TypedVariables) {
    uri u{"/user/{user_id}"};
    EXPECT_TRUE(!u.path.empty());
    auto _path = u.path.slugs();
    EXPECT_EQ(_path[0], "");
    EXPECT_EQ(_path[1], "user");
    EXPECT_EQ(_path[2], "{user_id}");
}

TEST(URITests, StructuredPath) {
    uri  u{"/user/19"};
    auto parsed = u.path.slugs();
    EXPECT_EQ(parsed[2], "19");
}

TEST(URITests, EqualPaths) {
    EXPECT_TRUE(equal_path("/one", "/one"));
    EXPECT_FALSE(equal_path("/two", "/one"));
    EXPECT_TRUE(equal_path("/two/", "/two/"));
    EXPECT_TRUE(equal_path("/two//", "/two//"));
    EXPECT_TRUE(equal_path("/two//three", "/two/three"));
}
TEST(URITests, Normalize) {

}
