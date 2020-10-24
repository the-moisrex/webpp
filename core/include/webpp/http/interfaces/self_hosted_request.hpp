// Created by moisrex on 10/24/20.

#ifndef WEBPP_SELF_HOSTED_REQUEST_HPP
#define WEBPP_SELF_HOSTED_REQUEST_HPP

#include "../../traits/enable_traits.hpp"
#include "./interface_concepts.hpp"
#include "../../version.hpp"

namespace webpp {


    template <Traits TraitsType, typename REL, Interface IfaceType>
    struct self_hosted_request : public REL, enable_traits<TraitsType> {
        using traits_type            = stl::remove_cvref_t<TraitsType>;
        using interface_type         = stl::remove_cvref_t<IfaceType>;
        using request_extension_list = REL;
        using allocator_type   = typename traits_type::template allocator<typename traits_type::char_type>;
        using application_type = typename interface_type::application_type;
        using logger_type      = typename traits_type::logger_type;
        using logger_ref       = typename logger_type::logger_ref;
        using etraits          = enable_traits<traits_type>;
        using string_type = typename traits_type::string_type;
        using string_view_type = typename traits_type::string_view_type;


        template <typename AllocType>
        self_hosted_request(auto&&...args) noexcept
          : etraits(stl::forward<decltype(args)>(args)...) {}


#define WEBPP_SHOSTED_HEADER(name, value)                  \
    [[nodiscard]] string_view_type name() const noexcept { \
        return interface_type::header(value);              \
    }


#undef WEBPP_SHOSTED_HEADER

#define WEBPP_SHOSTED_STR_METHOD(name, value)         \
    [[nodiscard]] string_type name() const noexcept { \
        return value;                                 \
    }

        // todo: add openssl or other stuff's version as well here
        WEBPP_SHOSTED_STR_METHOD(server_software, stl::format(FMT_COMPILE("WEB++/{}"), webpp_version));

#undef WEBPP_SHOSTED_STR_METHOD
    };


}

#endif // WEBPP_SELF_HOSTED_REQUEST_HPP
