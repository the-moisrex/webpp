// Created by moisrex on 7/13/23.

#include "../webpp/ip/ipv6.hpp"
#include "common/fuzz_common.hpp"

using namespace std;
using namespace webpp;

void ipv6_fuzz(string_view data) {
    ipv6 ip{data}; // from string
    ip.clear_prefix();
    [[maybe_unused]] volatile bool res = ip.has_prefix();
    res                                = ip.is_loopback();
    const string str                   = ip.string();
    ASSERT_NE(str.size(), 0);
}

register_fuzz(ipv6_fuzz);
