// Created by moisrex on 11/28/20.

#ifndef WEBPP_COMMON_REQUEST_HPP
#define WEBPP_COMMON_REQUEST_HPP

#include "../../../std/format.hpp"
#include "../../../traits/enable_traits.hpp"
#include "../../../version.hpp"


namespace webpp {


    template <Traits TraitsType, typename /* fixme: RequestExtensionList */ REL>
    struct common_request : public stl::remove_cvref_t<REL>, public enable_traits<TraitsType> {
        using traits_type            = stl::remove_cvref_t<TraitsType>;
        using request_extension_list = stl::remove_cvref_t<REL>;
        using etraits                = enable_traits<TraitsType>;
        using string_type            = typename etraits::string_type;


        template <typename... Args>
        common_request(Args&&... args) noexcept : etraits(stl::forward<Args>(args)...) {}


#define WEBPP_COMMON_STR_METHOD(name, value)          \
    [[nodiscard]] string_type name() const noexcept { \
        return value;                                 \
    }

        // todo: add openssl or other stuff's version as well here
        WEBPP_COMMON_STR_METHOD(server_software, stl::format(FMT_COMPILE("WEB++/{}"), webpp_version));

#undef WEBPP_COMMON_STR_METHOD
    };


} // namespace webpp

#endif // WEBPP_COMMON_REQUEST_HPP
