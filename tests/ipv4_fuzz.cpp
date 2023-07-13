// Created by moisrex on 7/13/23.

#include "../webpp/ip/ipv4.hpp"
#include "common/fuzz_common.hpp"

#include <string>

using namespace std;
using namespace webpp;

int ipv4_fuzz(string_view data) {
    ipv4 ip{data};
    ip.clear_prefix();
    [[maybe_unused]] bool res   = ip.has_prefix();
    res                         = ip.is_loopback();
    [[maybe_unused]] string str = ip.string();
    return static_cast<int>(str.size());
}

register_fuzz(ipv4_fuzz)
