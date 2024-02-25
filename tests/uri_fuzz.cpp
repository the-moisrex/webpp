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
    auto const res1         = url.as_string();
    auto const scheme_res   = url.scheme().as_string();
    auto const username_res = url.username().as_string();
    auto const password_res = url.password().as_string();
    auto const hostname_res = url.hostname().as_string();
    auto const port_res     = url.port().as_string();
    auto const path_res     = url.path().as_string();
    auto const queries_res  = url.queries().as_string();
    auto const fragment_res = url.fragment().as_string();

    if (url.has_value()) {
        auto const url_size =
          res1.size() + scheme_res.size() + username_res.size() + password_res.size() + hostname_res.size() +
          port_res.size() + path_res.size() + queries_res.size() + fragment_res.size();
        ASSERT_NE(url_size, 0);
    }
}

register_fuzz(uri_fuzz);
