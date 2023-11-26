#ifndef WEBPP_HTTP_HTTP2_HPP
#define WEBPP_HTTP_HTTP2_HPP

#include "../../std/string_view.hpp"

namespace webpp::http::h2 {


    // HTTP/2 Connection Preface
    // https://httpwg.org/specs/rfc9113.html#preface
    // PRI * HTTP/2.0\r\n\r\nSM\r\n\r\n
    // 0x505249202a20485454502f322e300d0a0d0a534d0d0a0d0a
    static constexpr stl::string_view h2_conn_preface =
      "\x50\x52\x49\x20\x2a\x20\x48\x54"
      "\x54\x50\x2f\x32\x2e\x30\x0d\x0a"
      "\x0d\x0a\x53\x4d\x0d\x0a\x0d\x0a";


} // namespace webpp::http::h2

#endif
