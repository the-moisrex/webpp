// Created by moisrex on 10/24/20.

#ifndef WEBPP_FCGI_REQUEST_HPP
#define WEBPP_FCGI_REQUEST_HPP

#include "../../../std/map.hpp"
#include "../common/common_http_request.hpp"

namespace webpp::http::inline fastcgi {


    template <Traits TraitsType, typename /* fixme: RequestExtensionList */ REL, Protocol ProtoType>
    struct fcgi_request : public common_request<TraitsType, REL> {
        using traits_type   = TraitsType;
        using protocol_type = ProtoType;

        istl::map<traits_type, string_type, string_type> data;
    };


} // namespace webpp::http::inline fastcgi

#endif // WEBPP_FCGI_REQUEST_HPP
