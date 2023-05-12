#include "../webpp/http/headers/host.hpp"
#include "common_pch.hpp"

using namespace webpp::http;

TEST(HdrHostAuthorityTest, HdrHostAuthorityTest) {
    host_authority const host{"example.org:80"};
    EXPECT_TRUE(host.is_valid());
    EXPECT_TRUE(host.is_domain());
    EXPECT_FALSE(host.is_ip());
    EXPECT_FALSE(host.is_ipv4());
    EXPECT_FALSE(host.is_ipv6());
    EXPECT_EQ(host.port(), 80);
    EXPECT_EQ(host.domain(), "example.org");
}


// Modified AI Generated Tests:

TEST(HdrHostAuthorityTest, ParseIpv4Host) {
    host_authority const host("192.0.2.0:8080");
    EXPECT_TRUE(host.is_ip());
    EXPECT_TRUE(host.is_ipv4());
    EXPECT_FALSE(host.is_ipv6());
    EXPECT_TRUE(host.is_valid());
    EXPECT_FALSE(host.is_domain());
    EXPECT_TRUE(host.has_port());
    EXPECT_EQ(host.port(), 8080);
    EXPECT_EQ(host.ipv4(), "192.0.2.0");
}

TEST(HdrHostAuthorityTest, ParseIpv6Host) {
    host_authority const host("[2001:0db8:85a3:0000:0000:8a2e:0370:7334]:80");
    EXPECT_TRUE(host.is_ip());
    EXPECT_FALSE(host.is_ipv4());
    EXPECT_TRUE(host.is_ipv6());
    EXPECT_TRUE(host.is_valid());
    EXPECT_FALSE(host.is_domain());
    EXPECT_TRUE(host.has_port());
    EXPECT_EQ(host.port(), 80);
    EXPECT_EQ(host.ipv6(), "2001:0db8:85a3:0000:0000:8a2e:0370:7334") << host.ipv6().string();
}

TEST(HdrHostAuthorityTest, ParseDomainHostWithoutPort) {
    host_authority const host("example.com");
    EXPECT_FALSE(host.is_ip());
    EXPECT_FALSE(host.is_ipv4());
    EXPECT_FALSE(host.is_ipv6());
    EXPECT_TRUE(host.is_valid());
    EXPECT_TRUE(host.is_domain());
    EXPECT_FALSE(host.has_port());
    EXPECT_EQ(host.domain(), "example.com");
}

TEST(HdrHostAuthorityTest, ParseDomainHostWithPort) {
    host_authority const host("subdomain.example.com:9000");
    EXPECT_FALSE(host.is_ip());
    EXPECT_FALSE(host.is_ipv4());
    EXPECT_FALSE(host.is_ipv6());
    EXPECT_TRUE(host.is_valid());
    EXPECT_TRUE(host.is_domain());
    EXPECT_TRUE(host.has_port());
    EXPECT_EQ(host.port(), 9000);
    EXPECT_EQ(host.domain(), "subdomain.example.com");
}

TEST(HdrHostAuthorityTest, ParseInvalidHost) {
    host_authority const host("invalid+host");
    EXPECT_FALSE(host.is_ip());
    EXPECT_FALSE(host.is_ipv4());
    EXPECT_FALSE(host.is_ipv6());
    EXPECT_FALSE(host.is_valid());
    EXPECT_FALSE(host.is_domain());
    EXPECT_FALSE(host.has_port());
}

TEST(HdrHostAuthorityTest, ParseEmptyHost) {
    host_authority const host("");
    EXPECT_FALSE(host.is_ip());
    EXPECT_FALSE(host.is_ipv4());
    EXPECT_FALSE(host.is_ipv6());
    EXPECT_FALSE(host.is_valid());
    EXPECT_FALSE(host.is_domain());
    EXPECT_FALSE(host.has_port());
}

TEST(HdrHostAuthorityTest, ParseIpv4HostWithoutPort) {
    host_authority const host("192.0.2.0");
    EXPECT_TRUE(host.is_ip());
    EXPECT_TRUE(host.is_ipv4());
    EXPECT_FALSE(host.is_ipv6());
    EXPECT_TRUE(host.is_valid());
    EXPECT_FALSE(host.is_domain());
    EXPECT_FALSE(host.has_port());
    EXPECT_EQ(host.ipv4(), "192.0.2.0");
}

TEST(HdrHostAuthorityTest, ParseIpv6HostWithoutPort) {
    host_authority const host("[2001:0db8:85a3:0000:0000:8a2e:0370:7334]");
    EXPECT_TRUE(host.is_ip());
    EXPECT_FALSE(host.is_ipv4());
    EXPECT_TRUE(host.is_ipv6());
    EXPECT_TRUE(host.is_valid());
    EXPECT_FALSE(host.is_domain());
    EXPECT_FALSE(host.has_port());
    EXPECT_EQ(host.ipv6(), "2001:0db8:85a3:0000:0000:8a2e:0370:7334");
}

TEST(HdrHostAuthorityTest, ParseDomainHostWithInvalidPort) {
    host_authority const host("example.com:invalid");
    EXPECT_FALSE(host.is_ip());
    EXPECT_FALSE(host.is_ipv4());
    EXPECT_FALSE(host.is_ipv6());
    EXPECT_FALSE(host.is_valid());
    EXPECT_TRUE(host.is_domain());
    EXPECT_FALSE(host.has_port());
    EXPECT_EQ(host.domain(), "example.com");
}

TEST(HdrHostAuthorityTest, ParseIpv4HostWithInvalidPort) {
    host_authority const host("192.0.2.0:invalid");
    EXPECT_TRUE(host.is_ip());
    EXPECT_TRUE(host.is_ipv4());
    EXPECT_FALSE(host.is_ipv6());
    EXPECT_FALSE(host.is_valid());
    EXPECT_FALSE(host.is_domain());
    EXPECT_FALSE(host.has_port());
    EXPECT_EQ(host.ipv4(), "192.0.2.0");
}

TEST(HdrHostAuthorityTest, ParseIpv6HostWithInvalidPort) {
    host_authority const host("[2001:0db8:85a3:0000:0000:8a2e:0370:7334]:invalid");
    EXPECT_TRUE(host.is_ip());
    EXPECT_FALSE(host.is_ipv4());
    EXPECT_TRUE(host.is_ipv6());
    EXPECT_FALSE(host.is_valid());
    EXPECT_FALSE(host.is_domain());
    EXPECT_FALSE(host.has_port());
    EXPECT_EQ(host.ipv6(), "2001:0db8:85a3:0000:0000:8a2e:0370:7334");
}

TEST(HdrHostAuthorityTest, ParseDomainHostWithLeadingWhitespace) {
    host_authority const host("  example.com");
    EXPECT_FALSE(host.is_ip());
    EXPECT_FALSE(host.is_ipv4());
    EXPECT_FALSE(host.is_ipv6());
    EXPECT_TRUE(host.is_valid());
    EXPECT_TRUE(host.is_domain());
    EXPECT_FALSE(host.has_port());
    EXPECT_EQ(host.domain(), "example.com");
}

TEST(HdrHostAuthorityTest, ParseDomainHostWithTrailingWhitespace) {
    host_authority const host("example.com  ");
    EXPECT_FALSE(host.is_ip());
    EXPECT_FALSE(host.is_ipv4());
    EXPECT_FALSE(host.is_ipv6());
    EXPECT_TRUE(host.is_valid());
    EXPECT_TRUE(host.is_domain());
    EXPECT_FALSE(host.has_port());
    EXPECT_EQ(host.domain(), "example.com");
}

TEST(HdrHostAuthorityTest, ParseDomainHostWithWhitespaceAroundPort) {
    host_authority const host("example.com :80");
    EXPECT_FALSE(host.is_ip());
    EXPECT_FALSE(host.is_ipv4());
    EXPECT_FALSE(host.is_ipv6());
    EXPECT_TRUE(host.is_valid());
    EXPECT_TRUE(host.is_domain());
    EXPECT_TRUE(host.has_port());
    EXPECT_EQ(host.port(), 80);
    EXPECT_EQ(host.domain(), "example.com");
}

TEST(HdrHostAuthorityTest, ParseIpv4HostWithLeadingWhitespace) {
    host_authority const host("  192.0.2.0");
    EXPECT_TRUE(host.is_ip());
    EXPECT_TRUE(host.is_ipv4());
    EXPECT_FALSE(host.is_ipv6());
    EXPECT_TRUE(host.is_valid());
    EXPECT_FALSE(host.is_domain());
    EXPECT_FALSE(host.has_port());
    EXPECT_EQ(host.ipv4(), "192.0.2.0");
}

TEST(HdrHostAuthorityTest, ParseIpv6HostWithLeadingWhitespace) {
    host_authority const host("  [2001:0db8:85a3:0000:0000:8a2e:0370:7334]");
    EXPECT_TRUE(host.is_ip());
    EXPECT_FALSE(host.is_ipv4());
    EXPECT_TRUE(host.is_ipv6());
    EXPECT_TRUE(host.is_valid());
    EXPECT_FALSE(host.is_domain());
    EXPECT_FALSE(host.has_port());
    EXPECT_EQ(host.ipv6(), "2001:0db8:85a3:0000:0000:8a2e:0370:7334");
}

TEST(HdrHostAuthorityTest, ParseDomainHostWithTrailingDot) {
    host_authority const host("example.com.");
    EXPECT_FALSE(host.is_ip());
    EXPECT_FALSE(host.is_ipv4());
    EXPECT_FALSE(host.is_ipv6());
    EXPECT_TRUE(host.is_valid());
    EXPECT_TRUE(host.is_domain());
    EXPECT_FALSE(host.has_port());
    EXPECT_EQ(host.domain(), "example.com");
}

TEST(HdrHostAuthorityTest, ParseDomainHostWithSubdomain) {
    host_authority const host("subdomain.example.com");
    EXPECT_FALSE(host.is_ip());
    EXPECT_FALSE(host.is_ipv4());
    EXPECT_FALSE(host.is_ipv6());
    EXPECT_TRUE(host.is_valid());
    EXPECT_TRUE(host.is_domain());
    EXPECT_FALSE(host.has_port());
    EXPECT_EQ(host.domain(), "subdomain.example.com");
}

TEST(HdrHostAuthorityTest, ParseIpv4HostWithSubdomain) {
    host_authority const host("subdomain.192.0.2.0");
    EXPECT_TRUE(host.is_ip());
    EXPECT_TRUE(host.is_ipv4());
    EXPECT_FALSE(host.is_ipv6());
    EXPECT_TRUE(host.is_valid());
    EXPECT_FALSE(host.is_domain());
    EXPECT_FALSE(host.has_port());
    EXPECT_EQ(host.ipv4(), "192.0.2.0");
}
