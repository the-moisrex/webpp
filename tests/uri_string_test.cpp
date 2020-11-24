#include "./common_pch.hpp"

// clang-format off
#include webpp_include(traits/std_traits)
#include webpp_include(uri/uri_string)
// clang-format on


using namespace webpp;
using namespace webpp::uri;

TEST(URITests, Creation) {
    // using set and get methods twice in a row should not affect the outcome

    uri_string u("http://example.com/");
    EXPECT_TRUE(u.has_scheme());
    EXPECT_TRUE(u.has_host());
    EXPECT_EQ(u.str(), "http://example.com/");
    EXPECT_EQ(u.host_raw(), "example.com");
    EXPECT_TRUE(webpp::is::host(u.host_raw()));
    EXPECT_TRUE(u.has_authority());
    EXPECT_TRUE(u.has_path()) << "the path is '/'";
    EXPECT_FALSE(u.has_port());
    EXPECT_FALSE(u.has_user_info());
    EXPECT_FALSE(u.has_fragment());
    EXPECT_FALSE(u.has_queries());
    EXPECT_EQ(u.raw_slugs().size(), 2);
    EXPECT_EQ(u.raw_slugs().to_string(), "/");
    EXPECT_EQ(u.scheme(), "http");
    u.clear_scheme();
    u.clear_scheme();
    EXPECT_FALSE(u.has_scheme());
    EXPECT_EQ(u.scheme(), "");
    EXPECT_TRUE(u.has_authority());
    EXPECT_TRUE(u.has_host());
    EXPECT_TRUE(u.has_path());
    EXPECT_FALSE(u.has_port());
    EXPECT_EQ(u.str(), "//example.com/");
    EXPECT_TRUE(u.is_normalized());
    u.clear_host();
    u.clear_host();
    EXPECT_TRUE(!u.has_host());
    EXPECT_EQ(u.host_raw(), "");
    EXPECT_EQ(u.str(), "/");
    u.path("folder/file");
    EXPECT_EQ(u.str(), "/folder/file");
    u.path("folder/file");
    EXPECT_TRUE(!u.has_host());
    u.host("eg2.com");
    u.host("eg2.com");
    EXPECT_TRUE(u.has_host());
    EXPECT_TRUE(u.has_path());
    EXPECT_EQ(u.host_raw(), "eg2.com") << "host is: " << u.host_raw() << "\nsize: " << u.host_raw().size();
    EXPECT_EQ(u.str(), "//eg2.com/folder/file") << "str is: " << u.str();
    u.scheme("https:");
    u.scheme("https:");
    EXPECT_TRUE(u.has_scheme());
    u.clear_path();
    u.clear_path();
    EXPECT_EQ(u.str(), "https://eg2.com/");
    u.scheme("http");
    u.scheme("http");
    EXPECT_TRUE(u.has_scheme());
    EXPECT_EQ(u.str(), "http://eg2.com/");

    uri_view ipv4_host("https://192.168.1.1");
    EXPECT_TRUE(webpp::is::ipv4(ipv4_host.host_raw()));
    EXPECT_EQ(ipv4_host.scheme(), "https");
    EXPECT_FALSE(ipv4_host.has_path());
    EXPECT_FALSE(ipv4_host.has_queries());
    EXPECT_FALSE(ipv4_host.has_port());
    EXPECT_TRUE(ipv4_host.has_authority());
    EXPECT_FALSE(!ipv4_host.has_host());
    EXPECT_EQ(ipv4_host.host_raw(), "192.168.1.1");

    uri_string local_file("file:///home/test/folder/file.txt");
    EXPECT_EQ(local_file.path_raw(), "/home/test/folder/file.txt");
    EXPECT_TRUE(local_file.has_path());
    EXPECT_TRUE(local_file.has_scheme());
    EXPECT_FALSE(local_file.has_authority());
    EXPECT_TRUE(!local_file.has_host());
    EXPECT_EQ(local_file.scheme(), "file");
    EXPECT_EQ(local_file.host(), "");
    auto path = local_file.slugs();
    EXPECT_EQ(local_file.raw_slugs().size(), 5);
    EXPECT_EQ(path.size(), 5);
    EXPECT_EQ(path.at(0), "");
    EXPECT_EQ(path.at(1), "home");
    EXPECT_EQ(path.at(2), "test");
    EXPECT_EQ(path.at(3), "folder");
    EXPECT_EQ(path.at(4), "file.txt");
    EXPECT_TRUE(local_file.is_path_absolute());
    EXPECT_TRUE(local_file.is_normalized());
    EXPECT_FALSE(local_file.is_path_relative());
    local_file.clear_path();
    EXPECT_EQ(local_file.str(), "file:///");
}

TEST(URITests, IPv6HostName) {
    uri_string         u;
    std::string uri_str =
      "//[::1]:8080/folder/file.md?name=value&name2=value2#str";
    u = uri_str;
    EXPECT_EQ(u.str(), uri_str);
    EXPECT_FALSE(u.has_scheme()) << "scheme: " << u.scheme();
    EXPECT_FALSE(!u.has_host());
    EXPECT_TRUE(u.has_port());
    EXPECT_TRUE(u.has_authority());
    EXPECT_TRUE(u.has_path()) << "path: " << u.path_raw();
    EXPECT_TRUE(u.has_queries());
    EXPECT_TRUE(u.has_fragment());
    EXPECT_EQ(u.fragment(), "str");
    EXPECT_EQ(u.path_raw(), "/folder/file.md") << "path: " << u.path_raw();
    EXPECT_EQ(u.host_raw(), "[::1]") << "host: " << u.host_raw();
    EXPECT_EQ(u.port_uint16(), 8080);
    EXPECT_EQ(u.port(), "8080");
    EXPECT_TRUE(std::holds_alternative<ipv6<std_traits>>(u.host_structured()))
      << "index: " << u.host_structured().index();
    u.clear_path();
    EXPECT_EQ(u.str(), "//[::1]:8080/?name=value&name2=value2#str");
}

TEST(URITests, WieredURIs) {
    uri_view u1("ftp://ftp.is.co.za/rfc/rfc1808.txt");
    EXPECT_FALSE(!u1.has_host());
    EXPECT_TRUE(u1.has_scheme());
    EXPECT_EQ(u1.scheme(), "ftp");
    EXPECT_EQ(u1.host_raw(), "ftp.is.co.za") << "host: " << u1.host_raw();
    EXPECT_FALSE(u1.host_raw().empty());
    EXPECT_NE(u1.host_raw(), "");
    EXPECT_TRUE(u1.has_path());
    EXPECT_EQ(u1.path_raw(), "/rfc/rfc1808.txt") << "path: " << u1.path_raw();
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
        EXPECT_TRUE(uri_view(_uri).is_valid()) << "uri: " << _uri;
    }

    uri_string not_port{"http://username:password@domain.tld/path/file.ext"};
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
        EXPECT_TRUE(uri_string(_urn).is_valid());
        EXPECT_TRUE(uri_string(_urn).is_urn());
        EXPECT_FALSE(uri_string(_urn).is_url());
    }

    uri_view  a("urn:example:a123,z456");
    uri_string       b{"URN:example:a123,z456"};
    uri_string       c{"urn:EXAMPLE:a123,z456"};

    EXPECT_TRUE(a == b);
    EXPECT_TRUE(a == c);
    EXPECT_TRUE(b == c);
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
        auto a = uri_view(u);
        EXPECT_TRUE(a.is_valid()) << u; // It's a valid URI
        EXPECT_TRUE(a.is_url()) << u;   // it's a valid URL too
        EXPECT_FALSE(a.is_urn()) << u;  // it shouldn't be a URN
    }

    for (auto const& u : invalid_urls) {
        auto a = uri_view(u);
        // It might be a valid URI
        EXPECT_FALSE(a.is_url()) << u; // it's a valid URL too
        EXPECT_FALSE(a.is_urn()) << u; // it shouldn't be a URN
    }
}

TEST(URITests, Set) {
    EXPECT_EQ(uri_string().scheme("ftp").str(), "ftp:");
    EXPECT_EQ(uri_string("mailto:someone@example.com").scheme("something_else").str(),
              "something_else:someone@example.com");

    // TODO: should this be allowed even???
    EXPECT_EQ(
      uri_string("urn:mpeg:mpeg7:schema:2001urn:isbn:0451450523").scheme("ftp").str(),
      "ftp:mpeg:mpeg7:schema:2001urn:isbn:0451450523");

    EXPECT_EQ(uri_string("http://example.com/").scheme("ftp").str(),
              "ftp://example.com/");
}

TEST(URITests, Domains) {
    uri_string u("http://coded.by.moisrex.localhost/path/to/something");
    EXPECT_FALSE(!u.has_host());
    EXPECT_EQ("coded.by.moisrex.localhost", u.host_raw());
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
    EXPECT_TRUE(u.has_path());
    EXPECT_EQ("coded.by", u.subdomains());
    EXPECT_EQ("moisrex", u.second_level_domain());
    EXPECT_EQ("dev", u.top_level_domain());
    u.second_level_domain("god");
    EXPECT_EQ("god", u.second_level_domain());
    EXPECT_EQ("dev", u.top_level_domain());
    EXPECT_TRUE(u.has_host());
    EXPECT_TRUE(u.has_path());
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
    uri_string u{"http://سلام.com"};
    EXPECT_TRUE(u.is_valid());

    uri_view ducky = "https://duckduckgo.com/?q=%D8%AA%D8%B3%D8%AA+%D9%85%DB%8C%DA%A9%D9%86%D9%85";
    // todo: should we use + or space?
    EXPECT_EQ(ducky.queries()["q"], "تست+میکنم");
    // todo: add tests for UTF-8 string types after you implemented them
}


TEST(URITests, TypedVariables) {
    uri_string u{"/user/{user_id}"};
    EXPECT_TRUE(u.has_path());
    auto _path = u.slugs();
    EXPECT_EQ(_path.size(), 3);
    if (_path.size() == 3) { // to stop breaking other tests
        EXPECT_EQ(_path[0], "");
        EXPECT_EQ(_path[1], "user");
        EXPECT_EQ(_path[2], "{user_id}");
    }
}

TEST(URITests, StructuredPath) {
    uri_string  u{"/user/19"};
    auto parsed = u.slugs();
    EXPECT_EQ(parsed.size(), 3);
    if (parsed.size() == 3) { // don't break other tests if this one is failing
        EXPECT_EQ(parsed[2], "19");
    }
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



//// tests from: https://github.com/rhymu8354/Uri/blob/main/test/src/UriTests.cpp



TEST(UriTests, ParseFromStringNoScheme) {
    uri_string uri;
    ASSERT_TRUE(uri.operator=("foo/bar").is_valid());
    ASSERT_EQ("", uri.scheme());
    // fixme: the bug is in the parsing section, but I'm not worried about this bug that much for now
    ASSERT_EQ(
      (std::vector< std::string >{
        "foo",
        "bar",
      }),
      uri.slugs<std::vector<std::string>>()
    );
}

TEST(UriTests, ParseFromStringUrl) {
    uri_string uri;
    ASSERT_TRUE(uri.operator=("http://www.example.com/foo/bar").is_valid());
    ASSERT_EQ("http", uri.scheme());
    ASSERT_EQ("www.example.com", uri.host());
    ASSERT_EQ(
      (std::vector< std::string >{
        "",
        "foo",
        "bar",
      }),
      uri.slugs<std::vector<std::string>>()
    );
}

TEST(UriTests, ParseFromStringUrnDefaultPathDelimiter) {
    uri_string uri;
    ASSERT_TRUE(uri.operator=("urn:book:fantasy:Hobbit").is_valid());
    ASSERT_EQ("urn", uri.scheme());
    ASSERT_EQ("", uri.host());
    ASSERT_EQ(
      (std::vector< std::string >{
        "book:fantasy:Hobbit",
      }),
      uri.slugs<std::vector<std::string>>()
    );
}

TEST(UriTests, ParseFromStringPathCornerCases) {
    struct TestVector {
        std::string pathIn;
        std::vector< std::string > pathOut;
    };
    const std::vector< TestVector > testVectors{
      {"", {}},
      {"/", {""}},
      {"/foo", {"", "foo"} },
      {"foo/", {"foo", ""} },
    };
    size_t index = 0;
    for (const auto& testVector : testVectors) {
        uri_string uri;
        ASSERT_TRUE(uri.operator=((testVector.pathIn)).is_valid()) << index;
        ASSERT_EQ(testVector.pathOut, uri.slugs<std::vector<std::string>>()) << index;
        ++index;
    }
}

TEST(UriTests, ParseFromStringHasAPortNumber) {
    uri_string uri;
    ASSERT_TRUE(uri.operator=("http://www.example.com:8080/foo/bar").is_valid());
    ASSERT_EQ("www.example.com", uri.host());
    ASSERT_TRUE(uri.has_port());
    ASSERT_EQ(8080, uri.port_uint16());
}

TEST(UriTests, ParseFromStringDoesNotHaveAPortNumber) {
    uri_string uri;
    ASSERT_TRUE(uri.operator=("http://www.example.com/foo/bar").is_valid());
    ASSERT_EQ("www.example.com", uri.host());
    ASSERT_FALSE(uri.has_port());
}

TEST(UriTests, ParseFromStringTwiceFirstWithPortNumberThenWithout) {
    uri_string uri;
    ASSERT_TRUE(uri.operator=("http://www.example.com:8080/foo/bar").is_valid());
    ASSERT_TRUE(uri.operator=("http://www.example.com/foo/bar").is_valid());
    ASSERT_FALSE(uri.has_port());
}

TEST(UriTests, ParseFromStringBadPortNumberPurelyAlphabetic) {
    uri_string uri;
    ASSERT_FALSE(uri.operator=("http://www.example.com:spam/foo/bar").is_valid());
}

TEST(UriTests, ParseFromStringBadPortNumberStartsNumericEndsAlphabetic) {
    uri_string uri;
    ASSERT_FALSE(uri.operator=("http://www.example.com:8080spam/foo/bar").is_valid());
}

TEST(UriTests, ParseFromStringLargestValidPortNumber) {
    uri_string uri;
    ASSERT_TRUE(uri.operator=("http://www.example.com:65535/foo/bar").is_valid());
    ASSERT_TRUE(uri.has_port());
    ASSERT_EQ(65535, uri.port_uint16());
}

TEST(UriTests, ParseFromStringBadPortNumberTooBig) {
    uri_string uri;
    ASSERT_FALSE(uri.operator=("http://www.example.com:65536/foo/bar").is_valid());
}

TEST(UriTests, ParseFromStringBadPortNumberNegative) {
    uri_string uri;
    ASSERT_FALSE(uri.operator=("http://www.example.com:-1234/foo/bar").is_valid());
}

TEST(UriTests, ParseFromStringEndsAfterAuthority) {
    uri_string uri;
    ASSERT_TRUE(uri.operator=("http://www.example.com").is_valid());
}

TEST(UriTests, ParseFromStringRelativeVsNonRelativeReferences) {
    struct TestVector {
        std::string uriString;
        bool isRelativeReference;
    };
    const std::vector< TestVector > testVectors{
      {"http://www.example.com/", false},
      {"http://www.example.com", false},
      {"/", true},
      {"foo", true},
    };
    size_t index = 0;
    for (const auto& testVector : testVectors) {
        uri_string uri;
        ASSERT_TRUE(uri.operator=((testVector.uriString)).is_valid()) << index;
        ASSERT_EQ(testVector.isRelativeReference, uri.is_relative_reference()) << index;
        ++index;
    }
}

TEST(UriTests, ParseFromStringRelativeVsNonRelativePaths) {
    struct TestVector {
        std::string uriString;
        bool containsRelativePath;
    };
    const std::vector< TestVector > testVectors{
      {"http://www.example.com/", false},
      {"http://www.example.com", false},
      {"/", false},
      {"foo", true},

      /*
       * This is only a valid test vector if we understand
       * correctly that an empty string IS a valid
       * "relative reference" URI with an empty path.
       */
      {"", true},
    };
    size_t index = 0;
    for (const auto& testVector : testVectors) {
        uri_string uri;
        ASSERT_TRUE(uri.operator=((testVector.uriString)).is_valid()) << index;
        ASSERT_EQ(testVector.containsRelativePath, uri.is_path_relative()) << index;
        ++index;
    }
}

TEST(UriTests, ParseFromStringQueryAndFragmentElements) {
    struct TestVector {
        std::string uriString;
        std::string host;
        std::string query;
        std::string fragment;
    };
    const std::vector< TestVector > testVectors{
      {"http://www.example.com/", "www.example.com", "", ""},
      {"http://example.com?foo", "example.com", "foo", ""},
      {"http://www.example.com#foo", "www.example.com", "", "foo"},
      {"http://www.example.com?foo#bar", "www.example.com", "foo", "bar"},
      {"http://www.example.com?earth?day#bar", "www.example.com", "earth?day", "bar"},
      {"http://www.example.com/spam?foo#bar", "www.example.com", "foo", "bar"},

      /*
       * NOTE: curiously, but we think this is correct, that
       * having a trailing question mark is equivalent to not having
       * any question mark, because in both cases, the query element
       * is empty string.  Perhaps research deeper to see if this is right.
       */
      {"http://www.example.com/?", "www.example.com", "", ""},
    };
    size_t index = 0;
    for (const auto& testVector : testVectors) {
        uri_string uri;
        ASSERT_TRUE(uri.operator=((testVector.uriString)).is_valid()) << index;
        ASSERT_EQ(testVector.host, uri.host()) << index;
        ASSERT_EQ(testVector.query, uri.queries_string()) << index;
        ASSERT_EQ(testVector.fragment, uri.fragment()) << index;
        ++index;
    }
}


TEST(UriTests, ParseFromStringUserInfo) {
    struct TestVector {
        std::string uriString;
        std::string userInfo;
    };
    const std::vector< TestVector > testVectors{
      {"http://www.example.com/", ""},
      {"http://joe@www.example.com", "joe"},
      {"http://pepe:feelsbadman@www.example.com", "pepe:feelsbadman"},
      {"//www.example.com", ""},
      {"//bob@www.example.com", "bob"},
      {"/", ""},
      {"foo", ""},
    };
    size_t index = 0;
    for (const auto& testVector : testVectors) {
        uri_string uri;
        ASSERT_TRUE(uri.operator=((testVector.uriString)).is_valid()) << index;
        ASSERT_EQ(testVector.userInfo, uri.user_info()) << index;
        ++index;
    }
}

TEST(UriTests, ParseFromStringTwiceFirstUserInfoThenWithout) {
    uri_string uri;
    ASSERT_TRUE(uri.operator=("http://joe@www.example.com/foo/bar").is_valid());
    ASSERT_TRUE(uri.operator=("/foo/bar").is_valid());
    ASSERT_TRUE(uri.user_info().empty());
}

TEST(UriTests, ParseFromStringSchemeIllegalCharacters) {
    const std::vector< std::string > testVectors{
      {"://www.example.com/"},
      {"0://www.example.com/"},
      {"+://www.example.com/"},
      {"@://www.example.com/"},
      {".://www.example.com/"},
      {"h@://www.example.com/"},
    };
    size_t index = 0;
    for (const auto& testVector : testVectors) {
        uri_string uri;
        ASSERT_FALSE(uri.operator=((testVector)).is_valid()) << index;
        ++index;
    }
}

TEST(UriTests, ParseFromStringSchemeBarelyLegal) {
    struct TestVector {
        std::string uriString;
        std::string scheme;
    };
    const std::vector< TestVector > testVectors{
      {"h://www.example.com/", "h"},
      {"x+://www.example.com/", "x+"},
      {"y-://www.example.com/", "y-"},
      {"z.://www.example.com/", "z."},
      {"aa://www.example.com/", "aa"},
      {"a0://www.example.com/", "a0"},
    };
    size_t index = 0;
    for (const auto& testVector : testVectors) {
        uri_string uri;
        ASSERT_TRUE(uri.operator=((testVector.uriString)).is_valid()) << index;
        ASSERT_EQ(testVector.scheme, uri.scheme());
        ++index;
    }
}

TEST(UriTests, ParseFromStringSchemeMixedCase) {
    const std::vector< std::string > testVectors{
      {"http://www.example.com/"},
      {"hTtp://www.example.com/"},
      {"HTTP://www.example.com/"},
      {"Http://www.example.com/"},
      {"HttP://www.example.com/"},
    };
    size_t index = 0;
    for (const auto& testVector : testVectors) {
        uri_string uri;
        ASSERT_TRUE(uri.operator=((testVector)).is_valid()) << index;
        ASSERT_EQ("http", uri.scheme()) << ">>> Failed for test vector element " << index << " <<<";
        ++index;
    }
}

TEST(UriTests, ParseFromStringHostEndsInDot) {
    uri_string uri;
    ASSERT_TRUE(uri.operator=("http://example.com./foo").is_valid());
    ASSERT_EQ("example.com.", uri.host());
}

TEST(UriTests, ParseFromStringUserInfoIllegalCharacters) {
    const std::vector< std::string > testVectors{
      {"//%X@www.example.com/"},
      {"//{@www.example.com/"},
    };
    size_t index = 0;
    for (const auto& testVector : testVectors) {
        uri_string uri;
        ASSERT_FALSE(uri.operator=((testVector)).is_valid()) << index;
        ++index;
    }
}

TEST(UriTests, ParseFromStringUserInfoBarelyLegal) {
    struct TestVector {
        std::string uriString;
        std::string userInfo;
    };
    const std::vector< TestVector > testVectors{
      {"//%41@www.example.com/", "A"},
      {"//@www.example.com/", ""},
      {"//!@www.example.com/", "!"},
      {"//'@www.example.com/", "'"},
      {"//(@www.example.com/", "("},
      {"//;@www.example.com/", ";"},
      {"http://:@www.example.com/", ":"},
    };
    size_t index = 0;
    for (const auto& testVector : testVectors) {
        uri_string uri;
        ASSERT_TRUE(uri.operator=((testVector.uriString)).is_valid()) << index;
        ASSERT_EQ(testVector.userInfo, uri.user_info());
        ++index;
    }
}

TEST(UriTests, ParseFromStringHostIllegalCharacters) {
    const std::vector< std::string > testVectors{
      {"//%X@www.example.com/"},
      {"//@www:example.com/"},
      {"//[vX.:]/"},
    };
    size_t index = 0;
    for (const auto& testVector : testVectors) {
        uri_string uri;
        ASSERT_FALSE(uri.operator=((testVector)).is_valid()) << index;
        ++index;
    }
}

TEST(UriTests, ParseFromStringHostBarelyLegal) {
    struct TestVector {
        std::string uriString;
        std::string host;
    };
    const std::vector< TestVector > testVectors{
      {"//%41/", "a"},
      {"///", ""},
      {"//!/", "!"},
      {"//'/", "'"},
      {"//(/", "("},
      {"//;/", ";"},
      {"//1.2.3.4/", "1.2.3.4"},
      {"//[v7.:]/", "v7.:"},
      {"//[v7.aB]/", "v7.aB"},
    };
    size_t index = 0;
    for (const auto& testVector : testVectors) {
        uri_string uri;
        ASSERT_TRUE(uri.operator=((testVector.uriString)).is_valid()) << index;
        ASSERT_EQ(testVector.host, uri.host());
        ++index;
    }
}

TEST(UriTests, ParseFromStringHostMixedCase) {
    const std::vector< std::string > testVectors{
      {"http://www.example.com/"},
      {"http://www.EXAMPLE.com/"},
      {"http://www.exAMple.com/"},
      {"http://www.example.cOM/"},
      {"http://wWw.exampLe.Com/"},
    };
    size_t index = 0;
    for (const auto& testVector : testVectors) {
        uri_string uri;
        ASSERT_TRUE(uri.operator=((testVector)).is_valid()) << index;
        ASSERT_EQ("www.example.com", uri.host()) << ">>> Failed for test vector element " << index << " <<<";
        ++index;
    }
}

TEST(UriTests, ParseFromStringDontMisinterpretColonInOtherPlacesAsSchemeDelimiter) {
    const std::vector< std::string > testVectors{
      {"//foo:bar@www.example.com/"},
      {"//www.example.com/a:b"},
      {"//www.example.com/foo?a:b"},
      {"//www.example.com/foo#a:b"},
      {"//[v7.:]/"},
      {"/:/foo"},
    };
    size_t index = 0;
    for (const auto& testVector : testVectors) {
        uri_string uri;
        ASSERT_TRUE(uri.operator=((testVector)).is_valid()) << index;
        ASSERT_TRUE(uri.scheme().empty());
        ++index;
    }
}

TEST(UriTests, ParseFromStringPathIllegalCharacters) {
    const std::vector< std::string > testVectors{
      {"http://www.example.com/foo[bar"},
      {"http://www.example.com/]bar"},
      {"http://www.example.com/foo]"},
      {"http://www.example.com/["},
      {"http://www.example.com/abc/foo]"},
      {"http://www.example.com/abc/["},
      {"http://www.example.com/foo]/abc"},
      {"http://www.example.com/[/abc"},
      {"http://www.example.com/foo]/"},
      {"http://www.example.com/[/"},
      {"/foo[bar"},
      {"/]bar"},
      {"/foo]"},
      {"/["},
      {"/abc/foo]"},
      {"/abc/["},
      {"/foo]/abc"},
      {"/[/abc"},
      {"/foo]/"},
      {"/[/"},
    };
    size_t index = 0;
    for (const auto& testVector : testVectors) {
        uri_string uri;
        ASSERT_FALSE(uri.operator=((testVector)).is_valid()) << index;
        ++index;
    }
}

TEST(UriTests, ParseFromStringPathBarelyLegal) {
    struct TestVector {
        std::string uriString;
        std::vector< std::string > path;
    };
    const std::vector< TestVector > testVectors{
      {"/:/foo", {"", ":", "foo"}},
      {"bob@/foo", {"bob@", "foo"}},
      {"hello!", {"hello!"}},
      {"urn:hello,%20w%6Frld", {"hello, world"}},
      {"//example.com/foo/(bar)/", {"", "foo", "(bar)", ""}},
    };
    size_t index = 0;
    for (const auto& testVector : testVectors) {
        uri_string uri;
        ASSERT_TRUE(uri.operator=((testVector.uriString)).is_valid()) << index;
        ASSERT_EQ(testVector.path, uri.slugs<std::vector<std::string>>());
        ++index;
    }
}

TEST(UriTests, ParseFromStringQueryIllegalCharacters) {
    const std::vector< std::string > testVectors{
      {"http://www.example.com/?foo[bar"},
      {"http://www.example.com/?]bar"},
      {"http://www.example.com/?foo]"},
      {"http://www.example.com/?["},
      {"http://www.example.com/?abc/foo]"},
      {"http://www.example.com/?abc/["},
      {"http://www.example.com/?foo]/abc"},
      {"http://www.example.com/?[/abc"},
      {"http://www.example.com/?foo]/"},
      {"http://www.example.com/?[/"},
      {"?foo[bar"},
      {"?]bar"},
      {"?foo]"},
      {"?["},
      {"?abc/foo]"},
      {"?abc/["},
      {"?foo]/abc"},
      {"?[/abc"},
      {"?foo]/"},
      {"?[/"},
    };
    size_t index = 0;
    for (const auto& testVector : testVectors) {
        uri_string uri;
        ASSERT_FALSE(uri.operator=((testVector)).is_valid()) << index;
        ++index;
    }
}

TEST(UriTests, ParseFromStringQueryBarelyLegal) {
    struct TestVector {
        std::string uriString;
        std::string query;
    };
    const std::vector< TestVector > testVectors{
      {"/?:/foo", ":/foo"},
      {"?bob@/foo", "bob@/foo"},
      {"?hello!", "hello!"},
      {"urn:?hello,%20w%6Frld", "hello, world"},
      {"//example.com/foo?(bar)/", "(bar)/"},
      {"http://www.example.com/?foo?bar", "foo?bar" },
    };
    size_t index = 0;
    for (const auto& testVector : testVectors) {
        uri_string uri;
        ASSERT_TRUE(uri.operator=((testVector.uriString)).is_valid()) << index;
        ASSERT_EQ(testVector.query, uri.queries_string());
        ++index;
    }
}

TEST(UriTests, ParseFromStringFragmentIllegalCharacters) {
    const std::vector< std::string > testVectors{
      {"http://www.example.com/#foo[bar"},
      {"http://www.example.com/#]bar"},
      {"http://www.example.com/#foo]"},
      {"http://www.example.com/#["},
      {"http://www.example.com/#abc/foo]"},
      {"http://www.example.com/#abc/["},
      {"http://www.example.com/#foo]/abc"},
      {"http://www.example.com/#[/abc"},
      {"http://www.example.com/#foo]/"},
      {"http://www.example.com/#[/"},
      {"#foo[bar"},
      {"#]bar"},
      {"#foo]"},
      {"#["},
      {"#abc/foo]"},
      {"#abc/["},
      {"#foo]/abc"},
      {"#[/abc"},
      {"#foo]/"},
      {"#[/"},
    };
    size_t index = 0;
    for (const auto& testVector : testVectors) {
        uri_string uri;
        ASSERT_FALSE(uri.operator=((testVector)).is_valid()) << index;
        ++index;
    }
}

TEST(UriTests, ParseFromStringFragmentBarelyLegal) {
    struct TestVector {
        std::string uriString;
        std::string fragment;
    };
    const std::vector< TestVector > testVectors{
      {"/#:/foo", ":/foo"},
      {"#bob@/foo", "bob@/foo"},
      {"#hello!", "hello!"},
      {"urn:#hello,%20w%6Frld", "hello, world"},
      {"//example.com/foo#(bar)/", "(bar)/"},
      {"http://www.example.com/#foo?bar", "foo?bar" },
    };
    size_t index = 0;
    for (const auto& testVector : testVectors) {
        uri_string uri;
        ASSERT_TRUE(uri.operator=((testVector.uriString)).is_valid()) << index;
        ASSERT_EQ(testVector.fragment, uri.fragment());
        ++index;
    }
}

TEST(UriTests, ParseFromStringPathsWithPercentEncodedCharacters) {
    struct TestVector {
        std::string uriString;
        std::string pathFirstSegment;
    };
    const std::vector< TestVector > testVectors{
      {"%41", "A"},
      {"%4A", "J"},
      {"%4a", "J"},
      {"%bc", "\xbc"},
      {"%Bc", "\xbc"},
      {"%bC", "\xbc"},
      {"%BC", "\xbc"},
      {"%41%42%43", "ABC"},
      {"%41%4A%43%4b", "AJCK"},
    };
    size_t index = 0;
    for (const auto& testVector : testVectors) {
        uri_string uri;
        ASSERT_TRUE(uri.operator=((testVector.uriString)).is_valid()) << index;
        ASSERT_EQ(testVector.pathFirstSegment, uri.slugs()[0]);
        ++index;
    }
}

TEST(UriTests, NormalizePath) {
    struct TestVector {
        std::string uriString;
        std::vector< std::string > normalizedPathSegments;
    };
    const std::vector< TestVector > testVectors{
      {"/a/b/c/./../../g", {"", "a", "g"}},
      {"mid/content=5/../6", {"mid", "6"}},
      {"http://example.com/a/../b", {"", "b"}},
      {"http://example.com/../b", {"", "b"}},
      {"http://example.com/a/../b/", {"", "b", ""}},
      {"http://example.com/a/../../b", {"", "b"}},
      {"./a/b", {"a", "b"}},
      {"..", {}},
      {"/", {""}},
      {"a/b/..", {"a", ""}},
      {"a/b/.", {"a", "b", ""}},
      {"a/b/./c", {"a", "b", "c"}},
      {"a/b/./c/", {"a", "b", "c", ""}},
      {"/a/b/..", {"", "a", ""}},
      {"/a/b/.", {"", "a", "b", ""}},
      {"/a/b/./c", {"", "a", "b", "c"}},
      {"/a/b/./c/", {"", "a", "b", "c", ""}},
      {"./a/b/..", {"a", ""}},
      {"./a/b/.", {"a", "b", ""}},
      {"./a/b/./c", {"a", "b", "c"}},
      {"./a/b/./c/", {"a", "b", "c", ""}},
      {"../a/b/..", {"a", ""}},
      {"../a/b/.", {"a", "b", ""}},
      {"../a/b/./c", {"a", "b", "c"}},
      {"../a/b/./c/", {"a", "b", "c", ""}},
      {"../a/b/../c", {"a", "c"}},
      {"../a/b/./../c/", {"a", "c", ""}},
      {"../a/b/./../c", {"a", "c"}},
      {"../a/b/./../c/", {"a", "c", ""}},
      {"../a/b/.././c/", {"a", "c", ""}},
      {"../a/b/.././c", {"a", "c"}},
      {"../a/b/.././c/", {"a", "c", ""}},
      {"/./c/d", {"", "c", "d"}},
      {"/../c/d", {"", "c", "d"}},
    };
    size_t index = 0;
    for (const auto& testVector : testVectors) {
        uri_string uri;
        ASSERT_TRUE(uri.operator=((testVector.uriString)).is_valid()) << index;
        uri.normalize();
        ASSERT_EQ(testVector.normalizedPathSegments, uri.slugs<std::vector<std::string>>()) << index;
        ++index;
    }
}

TEST(UriTests, ConstructNormalizeAndCompareEquivalentUris) {
    // This was inspired by section 6.2.2
    // of RFC 3986 (https://tools.ietf.org/html/rfc3986).
    uri_string uri1, uri2;
    ASSERT_TRUE(uri1.operator=("example://a/b/c/%7Bfoo%7D").is_valid());
    ASSERT_TRUE(uri2.operator=("eXAMPLE://a/./b/../b/%63/%7bfoo%7d").is_valid());
    ASSERT_NE(uri1, uri2);
    uri2.normalize();
    ASSERT_EQ(uri1, uri2);
}

TEST(UriTests, ReferenceResolution) {
    struct TestVector {
        std::string baseString;
        std::string relativeReferenceString;
        std::string targetString;
    };
    const std::vector< TestVector > testVectors{
      // These are all taken from section 5.4.1
      // of RFC 3986 (https://tools.ietf.org/html/rfc3986).
      {"http://a/b/c/d;p?q", "g:h", "g:h"},
      {"http://a/b/c/d;p?q", "g", "http://a/b/c/g"},
      {"http://a/b/c/d;p?q", "./g", "http://a/b/c/g"},
      {"http://a/b/c/d;p?q", "g/", "http://a/b/c/g/"},
      {"http://a/b/c/d;p?q", "//g", "http://g"},
      {"http://a/b/c/d;p?q", "?y", "http://a/b/c/d;p?y"},
      {"http://a/b/c/d;p?q", "g?y", "http://a/b/c/g?y"},
      {"http://a/b/c/d;p?q", "#s", "http://a/b/c/d;p?q#s"},
      {"http://a/b/c/d;p?q", "g#s", "http://a/b/c/g#s"},
      {"http://a/b/c/d;p?q", "g?y#s", "http://a/b/c/g?y#s"},
      {"http://a/b/c/d;p?q", ";x", "http://a/b/c/;x"},
      {"http://a/b/c/d;p?q", "g;x", "http://a/b/c/g;x"},
      {"http://a/b/c/d;p?q", "g;x?y#s", "http://a/b/c/g;x?y#s"},
      {"http://a/b/c/d;p?q", "", "http://a/b/c/d;p?q"},
      {"http://a/b/c/d;p?q", ".", "http://a/b/c/"},
      {"http://a/b/c/d;p?q", "./", "http://a/b/c/"},
      {"http://a/b/c/d;p?q", "..", "http://a/b/"},
      {"http://a/b/c/d;p?q", "../", "http://a/b/"},
      {"http://a/b/c/d;p?q", "../g", "http://a/b/g"},
      {"http://a/b/c/d;p?q", "../..", "http://a"},
      {"http://a/b/c/d;p?q", "../../", "http://a"},
      {"http://a/b/c/d;p?q", "../../g", "http://a/g"},

      // Here are some examples of our own.
      {"http://example.com", "foo", "http://example.com/foo"},
      {"http://example.com/", "foo", "http://example.com/foo"},
      {"http://example.com", "foo/", "http://example.com/foo/"},
      {"http://example.com/", "foo/", "http://example.com/foo/"},
      {"http://example.com", "/foo", "http://example.com/foo"},
      {"http://example.com/", "/foo", "http://example.com/foo"},
      {"http://example.com", "/foo/", "http://example.com/foo/"},
      {"http://example.com/", "/foo/", "http://example.com/foo/"},
      {"http://example.com/", "?foo", "http://example.com/?foo"},
      {"http://example.com/", "#foo", "http://example.com/#foo"},
    };
    size_t index = 0;
    for (const auto& testVector : testVectors) {
        uri_string baseUri, relativeReferenceUri, expectedTargetUri;
        ASSERT_TRUE(baseUri.operator=((testVector.baseString)).is_valid());
        ASSERT_TRUE(relativeReferenceUri.operator=((testVector.relativeReferenceString)).is_valid()) << index;
        ASSERT_TRUE(expectedTargetUri.operator=((testVector.targetString)).is_valid()) << index;
        const auto actualTargetUri = baseUri.resolve(relativeReferenceUri);
        ASSERT_EQ(expectedTargetUri, actualTargetUri) << index;
        ++index;
    }
}

TEST(UriTests, EmptyPathInUriWithAuthorityIsEquivalentToSlashOnlyPath) {
    uri_string uri1, uri2;
    ASSERT_TRUE(uri1.operator=("http://example.com").is_valid());
    ASSERT_TRUE(uri2.operator=("http://example.com/").is_valid());
    ASSERT_EQ(uri1, uri2);
    ASSERT_TRUE(uri1.operator=("//example.com").is_valid());
    ASSERT_TRUE(uri2.operator=("//example.com/").is_valid());
    ASSERT_EQ(uri1, uri2);
}

TEST(UriTests, IPv6Address) {
    struct TestVector {
        std::string uriString;
        std::string expectedHost;
        bool isValid;
    };
    const std::vector< TestVector > testVectors{
      // valid
      {"http://[::1]/", "::1", true},
      {"http://[::ffff:1.2.3.4]/", "::ffff:1.2.3.4", true},
      {"http://[2001:db8:85a3:8d3:1319:8a2e:370:7348]/", "2001:db8:85a3:8d3:1319:8a2e:370:7348", true},
      {"http://[2001:db8:85a3:8d3:1319:8a2e:370::]/", "2001:db8:85a3:8d3:1319:8a2e:370::", true},
      {"http://[2001:db8:85a3:8d3:1319:8a2e::1]/", "2001:db8:85a3:8d3:1319:8a2e::1", true},
      {"http://[fFfF::1]", "fFfF::1", true},
      {"http://[1234::1]", "1234::1", true},
      {"http://[fFfF:1:2:3:4:5:6:a]", "fFfF:1:2:3:4:5:6:a", true},
      {"http://[2001:db8:85a3::8a2e:0]/", "2001:db8:85a3::8a2e:0", true},
      {"http://[2001:db8:85a3:8a2e::]/", "2001:db8:85a3:8a2e::", true},

      // invalid
      {"http://[::fFfF::1]", "", false},
      {"http://[::ffff:1.2.x.4]/", "", false},
      {"http://[::ffff:1.2.3.4.8]/", "", false},
      {"http://[::ffff:1.2.3]/", "", false},
      {"http://[::ffff:1.2.3.]/", "", false},
      {"http://[::ffff:1.2.3.256]/", "", false},
      {"http://[::fxff:1.2.3.4]/", "", false},
      {"http://[::ffff:1.2.3.-4]/", "", false},
      {"http://[::ffff:1.2.3. 4]/", "", false},
      {"http://[::ffff:1.2.3.4 ]/", "", false},
      {"http://[::ffff:1.2.3.4/", "", false},
      {"http://::ffff:1.2.3.4]/", "", false},
      {"http://::ffff:a.2.3.4]/", "", false},
      {"http://::ffff:1.a.3.4]/", "", false},
      {"http://[2001:db8:85a3:8d3:1319:8a2e:370:7348:0000]/", "", false},
      {"http://[2001:db8:85a3:8d3:1319:8a2e:370:7348::1]/", "", false},
      {"http://[2001:db8:85a3:8d3:1319:8a2e:370::1]/", "", false},
      {"http://[2001:db8:85a3::8a2e:0:]/", "", false},
      {"http://[2001:db8:85a3::8a2e::]/", "", false},
      {"http://[]/", "", false},
      {"http://[:]/", "", false},
      {"http://[v]/", "", false},
    };
    size_t index = 0;
    for (const auto& testVector : testVectors) {
        uri_string uri;
        uri = (testVector.uriString);
        ASSERT_EQ(testVector.isValid, uri.is_valid()) << index;
        if (uri.is_valid()) {
            ASSERT_EQ(testVector.expectedHost, uri.host());
        }
        ++index;
    }
}

TEST(UriTests, IPvFutureAddress) {
    struct TestVector {
        std::string uriString;
        std::string expectedHost;
        bool isValid;
    };
    const std::vector< TestVector > testVectors{
      // valid
      {"http://[v1.x]/", "v1.x", true},
      {"http://[vf.xy]/", "vf.xy", true},
      {"http://[vf.x:y]/", "vf.x:y", true},

      // invalid
      {"http://[vx]/", "", false},
      {"http://[v12]/", "", false},
      {"http://[v1.?]/", "", false},
      {"http://[v1.x?]/", "", false},
    };
    size_t index = 0;
    for (const auto& testVector : testVectors) {
        uri_string uri;
        uri = (testVector.uriString);
        ASSERT_EQ(testVector.isValid, uri.is_valid()) << index;
        if (uri.is_valid()) {
            ASSERT_EQ(testVector.expectedHost, uri.host());
        }
        ++index;
    }
}

TEST(UriTests, to_string) {
    struct TestVector {
        std::string scheme;
        std::string userinfo;
        std::string host;
        bool has_port;
        uint16_t port;
        std::vector< std::string > path;
        bool has_queries;
        std::string query;
        bool has_fragment;
        std::string fragment;
        std::string expectedUriString;
    };
    const std::vector< TestVector > testVectors{
      // general test vectors
      {"http", "bob", "www.example.com", true,  8080, {"", "abc", "def"}, true,  "foobar", true,  "ch2", "http://bob@www.example.com:8080/abc/def?foobar#ch2"},
      {"http", "bob", "www.example.com", true,  0,    {},                 true,  "foobar", true,  "ch2", "http://bob@www.example.com:0?foobar#ch2"},
      {"http", "bob", "www.example.com", true,  0,    {},                 true,  "foobar", true,  "",    "http://bob@www.example.com:0?foobar#"},
      {"",     "",    "example.com",     false, 0,    {},                 true,  "bar",    false, "",    "//example.com?bar"},
      {"",     "",    "example.com",     false, 0,    {},                 true,  ""   ,    false, "",    "//example.com?"},
      {"",     "",    "example.com",     false, 0,    {},                 false, "",       false, "",    "//example.com"},
      {"",     "",    "example.com",     false, 0,    {""},               false, "",       false, "",    "//example.com/"},
      {"",     "",    "example.com",     false, 0,    {"", "xyz"},        false, "",       false, "",    "//example.com/xyz"},
      {"",     "",    "example.com",     false, 0,    {"", "xyz", ""},    false, "",       false, "",    "//example.com/xyz/"},
      {"",     "",    "",                false, 0,    {""},               false, "",       false, "",    "/"},
      {"",     "",    "",                false, 0,    {"", "xyz"},        false, "",       false, "",    "/xyz"},
      {"",     "",    "",                false, 0,    {"", "xyz", ""},    false, "",       false, "",    "/xyz/"},
      {"",     "",    "",                false, 0,    {},                 false, "",       false, "",    ""},
      {"",     "",    "",                false, 0,    {"xyz"},            false, "",       false, "",    "xyz"},
      {"",     "",    "",                false, 0,    {"xyz", ""},        false, "",       false, "",    "xyz/"},
      {"",     "",    "",                false, 0,    {},                 true,  "bar",    false, "",    "?bar"},
      {"http", "",    "",                false, 0,    {},                 true,  "bar",    false, "",    "http:?bar"},
      {"http", "",    "",                false, 0,    {},                 false, "",       false, "",    "http:"},
      {"http", "",    "::1",             false, 0,    {},                 false, "",       false, "",    "http://[::1]"},
      {"http", "",    "::1.2.3.4",       false, 0,    {},                 false, "",       false, "",    "http://[::1.2.3.4]"},
      {"http", "",    "1.2.3.4",         false, 0,    {},                 false, "",       false, "",    "http://1.2.3.4"},
      {"",     "",    "",                false, 0,    {},                 false, "",       false, "",    ""},
      {"http", "bob", "",                false, 0,    {},                 true,  "foobar", false, "",    "http://bob@?foobar"},
      {"",     "bob", "",                false, 0,    {},                 true,  "foobar", false, "",    "//bob@?foobar"},
      {"",     "bob", "",                false, 0,    {},                 false, "",       false, "",    "//bob@"},

      // percent-encoded character test vectors
      {"http", "b b", "www.example.com", true,  8080, {"", "abc", "def"}, true,  "foobar", true,  "ch2", "http://b%20b@www.example.com:8080/abc/def?foobar#ch2"},
      {"http", "bob", "www.e ample.com", true,  8080, {"", "abc", "def"}, true,  "foobar", true,  "ch2", "http://bob@www.e%20ample.com:8080/abc/def?foobar#ch2"},
      {"http", "bob", "www.example.com", true,  8080, {"", "a c", "def"}, true,  "foobar", true,  "ch2", "http://bob@www.example.com:8080/a%20c/def?foobar#ch2"},
      {"http", "bob", "www.example.com", true,  8080, {"", "abc", "def"}, true,  "foo ar", true,  "ch2", "http://bob@www.example.com:8080/abc/def?foo%20ar#ch2"},
      {"http", "bob", "www.example.com", true,  8080, {"", "abc", "def"}, true,  "foobar", true,  "c 2", "http://bob@www.example.com:8080/abc/def?foobar#c%202"},
      {"http", "bob", "ሴ.example.com",   true,  8080, {"", "abc", "def"}, true,  "foobar", false, "",    "http://bob@%E1%88%B4.example.com:8080/abc/def?foobar"},

      // normalization of IPv6 address hex digits
      {"http", "bob", "fFfF::1", true,  8080, {"", "abc", "def"}, true,  "foobar", true,  "c 2", "http://bob@[ffff::1]:8080/abc/def?foobar#c%202"},
    };
    size_t index = 0;
    for (const auto& testVector : testVectors) {
        uri_string uri;
        uri.scheme(testVector.scheme);
        uri.user_info(testVector.userinfo);
        uri.host(testVector.host);
        if (testVector.has_port) {
            uri.port(testVector.port);
        } else {
            uri.clear_port();
        }
        uri.path(testVector.path);
        if (testVector.has_queries) {
            uri.queries(testVector.query);
        } else {
            uri.clear_queries();
        }
        if (testVector.has_fragment) {
            uri.fragment(testVector.fragment);
        } else {
            uri.clear_fragment();
        }
        const auto actualUriString = uri.to_string();
        ASSERT_EQ(testVector.expectedUriString, actualUriString) << index;
        ++index;
    }
}

TEST(UriTests, FragmentEmptyButPresent) {
    uri_string uri;
    ASSERT_TRUE(uri.operator=("http://example.com#").is_valid());
    ASSERT_TRUE(uri.has_fragment());
    ASSERT_EQ("", uri.fragment());
    ASSERT_EQ("http://example.com/#", uri.to_string());
    uri.clear_fragment();
    ASSERT_EQ("http://example.com/", uri.to_string());
    ASSERT_FALSE(uri.has_fragment());
    ASSERT_TRUE(uri.operator=("http://example.com").is_valid());
    ASSERT_FALSE(uri.has_fragment());
    uri.fragment("");
    ASSERT_TRUE(uri.has_fragment());
    ASSERT_EQ("", uri.fragment());
    ASSERT_EQ("http://example.com/#", uri.to_string());
}

TEST(UriTests, QueryEmptyButPresent) {
    uri_string uri;
    ASSERT_TRUE(uri.operator=("http://example.com?").is_valid());
    ASSERT_TRUE(uri.has_queries());
    ASSERT_EQ("", uri.queries_string());
    ASSERT_EQ("http://example.com/?", uri.to_string());
    uri.clear_queries();
    ASSERT_EQ("http://example.com/", uri.to_string());
    ASSERT_FALSE(uri.has_queries());
    ASSERT_TRUE(uri.operator=("http://example.com").is_valid());
    ASSERT_FALSE(uri.has_queries());
    uri.queries("");
    ASSERT_TRUE(uri.has_queries());
    ASSERT_EQ("", uri.queries_string());
    ASSERT_EQ("http://example.com/?", uri.to_string());
}

TEST(UriTests, MakeACopy) {
    uri_string uri1;
    uri1 = "http://www.example.com/foo.txt";
    uri_string uri2(uri1);
    uri1.queries("bar");
    uri2.fragment("page2");
    uri2.host("example.com");
    EXPECT_EQ("http://www.example.com/foo.txt?bar", uri1.to_string());
    EXPECT_EQ("http://example.com/foo.txt#page2", uri2.to_string());
}

TEST(UriTests, AssignACopy) {
    uri_string uri1;
    uri1 = "http://www.example.com/foo.txt";
    uri_string uri2;
    uri2 = uri1;
    uri1.queries("bar");
    uri2.fragment("page2");
    uri2.host("example.com");
    EXPECT_EQ("http://www.example.com/foo.txt?bar", uri1.to_string());
    EXPECT_EQ("http://example.com/foo.txt#page2", uri2.to_string());
}

TEST(UriTests, clear_queries) {
    uri_string uri;
    uri = "http://www.example.com/?foo=bar";
    uri.clear_queries();
    EXPECT_EQ("http://www.example.com/", uri.to_string());
    EXPECT_FALSE(uri.has_queries());
}

TEST(UriTests, PercentEncodePlusInQueries) {
    // Although RFC 3986 doesn't say anything about '+', some web services
    // treat it the same as ' ' due to how HTML originally defined how
    // to encode the query portion of a URL
    // (see https://stackoverflow.com/questions/2678551/when-to-encode-space-to-plus-or-20).
    //
    // To avoid issues with these web services, make sure '+' is
    // percent-encoded in a URI when the URI is encoded.
    uri_string uri;
    uri.queries("foo+bar");
    EXPECT_EQ("?foo%2Bbar", uri.to_string());
}








