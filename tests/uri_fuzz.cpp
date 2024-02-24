// Created by moisrex on 7/13/23.

#include "../webpp/uri/uri.hpp"
#include "common/fuzz_common.hpp"

using namespace webpp;

void uri_fuzz(stl::string_view data) {
    uri::uri url{data};
    url.scheme(data);
    url.username(data);
    url.password(data);
    url.port(data);
    url.hostname(data);
    url.path(data);
    url.queries(data);
    url.fragment(data);
    // ASSERT_NE(url.size(), 0);
}

register_fuzz(uri_fuzz);
