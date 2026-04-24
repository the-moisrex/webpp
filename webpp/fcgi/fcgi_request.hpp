// Created by moisrex on 10/24/20.

#ifndef WEBPP_FCGI_REQUEST_HPP
#define WEBPP_FCGI_REQUEST_HPP

#include "../std/map.hpp"

namespace webpp::fastcgi {


    template <typename CommonHTTPRequest, istl::CharType CharT, Allocator AllocT>
    struct fcgi_request : public CommonHTTPRequest {
        using common_http_request = CommonHTTPRequest;
        using string_type         = stl::basic_string<CharT, stl::char_traits<CharT>, AllocT>;

        // todo: use allocator
        stl::map<string_type, string_type> data;
    };


} // namespace webpp::fastcgi

#endif // WEBPP_FCGI_REQUEST_HPP
