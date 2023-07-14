// Created by moisrex on 7/13/23.

#include "../webpp/ip/ipv4.hpp"
#include "common/fuzz_common.hpp"

using namespace std;
using namespace webpp;

void ipv4_fuzz(string_view data) {
    ipv4 ip{data}; // from string
    ip.clear_prefix();
    [[maybe_unused]] volatile bool res = ip.has_prefix();
    res                                = ip.is_loopback();
    const string str                   = ip.string();
    ASSERT_NE(str.size(), 0);
}

register_fuzz(ipv4_fuzz);
