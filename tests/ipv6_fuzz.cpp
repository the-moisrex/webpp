// Created by moisrex on 7/13/23.

#include "../webpp/ip/ipv6.hpp"
#include "common/fuzz_common.hpp"

using namespace std;
using namespace webpp;

namespace {
    void ipv6_fuzz(string_view data) {
        ipv6 ip6{data}; // from string
        ip6.clear_prefix();
        [[maybe_unused]] bool volatile res = ip6.has_prefix();
        res                                = ip6.is_loopback();
        string const str                   = ip6.string();
        ASSERT_NE(str.size(), 0);
    }
} // namespace

register_fuzz(ipv6_fuzz);
