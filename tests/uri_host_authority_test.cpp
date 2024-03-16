#include "../webpp/uri/host_authority.hpp"
#include "common/tests_common_pch.hpp"

using namespace webpp;
using namespace webpp::uri;

TEST(HdrHostAuthorityTest, HdrHostAuthorityTest) {
    host_authority const host{"example.org:80"};
    EXPECT_TRUE(host.is_valid());
    EXPECT_TRUE(host.is_domain());
    EXPECT_FALSE(host.is_ip());
    EXPECT_FALSE(host.is_ipv4());
    EXPECT_FALSE(host.is_ipv6());
    EXPECT_EQ(host.port(), 80);
    EXPECT_EQ(host.domain(), "example.org");
    EXPECT_FALSE(host.address().is_valid()) << host.address().status_string();
    EXPECT_FALSE(ip_address::invalid().is_valid()) << ip_address::invalid().status_string();
}

TEST(HdrHostAuthorityTest, AddressTest) {
    static constinit ipv4 const localhost{"127.0.0.1"};
    host_authority const        host{"127.0.0.1:80"};
    EXPECT_TRUE(host.address().is_valid());
    EXPECT_EQ(host.address(), "127.0.0.1");
    EXPECT_EQ(host.address(), localhost);
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

TEST(HdrHostAuthorityTest, ParseSeeminglyIpv4ButADomain) {
    // todo: is this really a valid domain?
    host_authority const host("192.2.1:8080");
    EXPECT_FALSE(host.is_ip());
    EXPECT_FALSE(host.is_ipv4());
    EXPECT_FALSE(host.is_ipv6());
    EXPECT_TRUE(host.is_valid());
    EXPECT_TRUE(host.is_domain());
    EXPECT_TRUE(host.has_port());
    EXPECT_EQ(host.domain(), "192.2.1");
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

TEST(HdrHostAuthorityTest, ParseDomainHostWithTrailingDot) {
    host_authority const host("example.com.");
    EXPECT_FALSE(host.is_ip());
    EXPECT_FALSE(host.is_ipv4());
    EXPECT_FALSE(host.is_ipv6());
    EXPECT_FALSE(host.is_valid());
    EXPECT_FALSE(host.is_domain());
    EXPECT_FALSE(host.has_port());
    // EXPECT_EQ(host.domain(), "example.com");
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
    EXPECT_FALSE(host.is_ip());
    EXPECT_FALSE(host.is_ipv4());
    EXPECT_FALSE(host.is_ipv6());
    EXPECT_TRUE(host.is_valid());
    EXPECT_TRUE(host.is_domain());
    EXPECT_FALSE(host.has_port());
    EXPECT_EQ(host.domain(), "subdomain.192.0.2.0");
}
