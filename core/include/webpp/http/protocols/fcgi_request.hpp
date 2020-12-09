// Created by moisrex on 10/24/20.

#ifndef WEBPP_FCGI_REQUEST_HPP
#define WEBPP_FCGI_REQUEST_HPP

#include "./common/common_request.hpp"
#include "../../std/map.hpp"

namespace webpp {


    template <Traits TraitsType, typename /* fixme: RequestExtensionList */ REL, Protocol ProtoType>
    struct fcgi_request : public common_request<TraitsType, REL> {
        using traits_type           = TraitsType;
        using protocol_type         = ProtoType;

        istl::map<traits_type, string_type, string_type> data;


    };


}

#endif // WEBPP_FCGI_REQUEST_HPP
