// Created by moisrex on 6/6/23.


#include "../webpp/ip/endpoint.hpp"

#include "common_pch.hpp"
using namespace webpp;

static constexpr ipv4          ipv4_localhost{127, 0, 0, 1};
static constexpr ipv6          ipv6_localhost{"::1"};
static constexpr stl::uint16_t test_port = 12332;

TEST(EndPoint, Concept) {
    EXPECT_TRUE(Endpoint<ip_endpoint>);
}

TEST(EndPoint, IsBindable) {
    // testing localhost
    ip_endpoint const ep{ip_endpoint::tcp, ipv4_localhost, test_port};
    EXPECT_TRUE(ep.is_bindable());
}
