// Created by moisrex on 6/6/23.


#include "../webpp/ip/endpoint.hpp"

#include "common_pch.hpp"
using namespace webpp;


TEST(EndPoint, Concept) {
    EXPECT_TRUE(Endpoint<endpoint>);
}

TEST(EndPoint, SimpleUsage) {}
