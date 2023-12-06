// Created by moisrex on 11/28/20.

#ifndef WEBPP_COMMON_HTTP_PROTOCOL_HPP
#define WEBPP_COMMON_HTTP_PROTOCOL_HPP

#include "../../traits/enable_traits.hpp"
#include "../app_wrapper.hpp"
#include "../http_concepts.hpp"

namespace webpp::http {


    template <Traits TraitsType, Application App>
    struct common_http_protocol : enable_owner_traits<TraitsType> {
        using traits_type         = TraitsType;
        using application_type    = stl::remove_cvref_t<App>;
        using string_view_type    = traits::string_view<traits_type>;
        using char_type           = traits::char_type<traits_type>;
        using string_type         = traits::general_string<traits_type>;
        using etraits             = enable_owner_traits<traits_type>;
        using app_wrapper_type    = http_app_wrapper<traits_type, application_type>;

        app_wrapper_type app; // NOLINT(misc-non-private-member-variables-in-classes)

        template <typename... Args>
        explicit constexpr common_http_protocol(Args&&... args)
          : app{this->get_traits(), stl::forward<Args>(args)...} {}
    };

} // namespace webpp::http

#endif // WEBPP_COMMON_HTTP_PROTOCOL_HPP
