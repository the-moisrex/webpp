// Created by moisrex on 7/13/23.

#include "../webpp/ip/ipv4.hpp"
#include "common/fuzz_common.hpp"

using namespace std;
using namespace webpp;

namespace {
    void ipv4_fuzz(string_view data) {
        ipv4 ip4{data}; // from string
        ip4.clear_prefix();
        [[maybe_unused]] bool volatile res = ip4.has_prefix();
        res                                = ip4.is_loopback();
        string const str                   = ip4.string();
        ASSERT_NE(str.size(), 0);
    }
} // namespace

register_fuzz(ipv4_fuzz);
