// Created by moisrex on 10/24/20.

#ifndef WEBPP_FCGI_REQUEST_HPP
#define WEBPP_FCGI_REQUEST_HPP

#include "../../../std/map.hpp"

namespace webpp::http::inline fastcgi {


    template <Traits TraitsType, typename CommonHTTPRequest>
    struct fcgi_request : public CommonHTTPRequest {
        using common_http_request = CommonHTTPRequest;
        using traits_type         = TraitsType;

        istl::map<traits_type, string_type, string_type> data;
    };


} // namespace webpp::http::inline fastcgi

#endif // WEBPP_FCGI_REQUEST_HPP
