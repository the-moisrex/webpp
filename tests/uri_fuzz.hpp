#ifndef WEBPP_URI_FUZZ_HPP
#define WEBPP_URI_FUZZ_HPP

#include "../webpp/uri/uri.hpp"
#include "./common/test.hpp"

namespace webpp::tests {
    static void uri_fuzz(stl::string_view data) {
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
        auto const scheme_res   = url.scheme();
        auto const username_res = url.username();
        auto const password_res = url.password();
        auto const hostname_res = url.hostname();
        auto const port_res     = url.port_view();
        auto const path_res     = url.path();
        auto const queries_res  = url.queries();
        auto const fragment_res = url.fragment();

        if (url.has_value()) {
            auto const url_size =
              res1.size() + scheme_res.size() + username_res.size() + password_res.size() + hostname_res.size() +
              port_res.size() + path_res.size() + queries_res.size() + fragment_res.size();
            ASSERT_NE(url_size, 0);
        }
    }
} // namespace webpp::tests
#endif // WEBPP_URI_FUZZ_HPP
