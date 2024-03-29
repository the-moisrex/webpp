// Created by moisrex on 6/6/23.


#include "../webpp/ip/endpoint.hpp"

#include "../webpp/ip/default_endpoints.hpp"
#include "common/tests_common_pch.hpp"
using namespace webpp;

inline constexpr stl::uint16_t test_port = 12'332;

TEST(EndPoint, Concept) {
    EXPECT_TRUE(Endpoint<ip_endpoint>);
    EXPECT_TRUE(EndpointList<decltype(localhost_endpoints)>);
}

TEST(EndPoint, IsBindable) {
    // testing localhost
    constexpr ip_endpoint ep{ip_endpoint::tcp, ipv4_localhost, test_port};
    EXPECT_TRUE(ep.is_bindable());
}
