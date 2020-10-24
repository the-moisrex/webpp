// Created by moisrex on 10/24/20.

#ifndef WEBPP_FCGI_REQUEST_HPP
#define WEBPP_FCGI_REQUEST_HPP

#include "../../std/map.hpp"
#include "./interface_concepts.hpp"
#include "../../traits/enable_traits.hpp"

namespace webpp {


    template <Traits TraitsType, typename /* fixme: RequestExtensionList */ REL, Interface IfaceType>
    struct fcgi_request : public REL, public enable_traits<TraitsType> {
        using traits_type            = TraitsType;
        using interface_type         = IfaceType;
        using request_extension_list = REL;
        using allocator_type   = typename traits_type::template allocator<typename traits_type::char_type>;
        using application_type = typename interface_type::application_type;
        using logger_type      = typename traits_type::logger;
        using logger_ref       = typename traits_type::logger::logger_ref;
        using string_type      = typename traits_type::string_type;
        using etraits          = enable_traits<traits_type>;

        istl::map<traits_type, string_type, string_type> data;


        fcgi_request(logger_ref logger = logger_type{}, auto const& alloc = allocator_type{}) noexcept
          : etraits{logger, alloc},
            data{alloc} {}
    };


}

#endif // WEBPP_FCGI_REQUEST_HPP
