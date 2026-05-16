#ifndef WEBPP_URI_FUZZ_HPP
#define WEBPP_URI_FUZZ_HPP

#include "../webpp/http/codec/http_parser.hpp"
#include "./common/test.hpp"

namespace webpp::tests {
    static void http_parser_fuzz(stl::string_view data) {
        http::parsed_request_line req;
        auto* const               begin = data.data();
        auto*                     pos   = data.data();
        char const*               end   = data.data() + data.size();
        parse_request_line(pos, end, req);
        if (is_ok(req.state)) {
            EXPECT_FALSE(req.method.empty());
            EXPECT_FALSE(req.target.empty());
            EXPECT_FALSE(req.version.empty());
        }


        pos = begin;
        stl::string_view name;
        stl::string_view value;
        auto const       state = http::parse_header(pos, end, name, value);
        if (is_ok(state)) {
            EXPECT_FALSE(name.empty());
            EXPECT_FALSE(value.empty());
        }
    }
} // namespace webpp::tests
#endif // WEBPP_URI_FUZZ_HPP
