// Created by moisrex on 12/26/22.

#ifndef WEBPP_CUSTOM_BODY_HPP
#define WEBPP_CUSTOM_BODY_HPP

#include "../../traits/enable_traits.hpp"

#include <compare>

namespace webpp::http {

    template <typename T>
    struct custom_body : T {
        // common custom body features go here
    };

    /**
     * This class is the mother of all custom bodies in which their common features will be presented here.
     */
    template <TraitsAccess TA>
    struct custom_body<TA> : public enable_traits_access<custom_body<TA>> {
        using traits_access = enable_traits_access<TA>;
        using traits_type   = typename traits_access::traits_type;

        constexpr custom_body()                            = default;
        constexpr custom_body(custom_body const&) noexcept = default;
        constexpr custom_body(custom_body&&) noexcept      = default;
        constexpr ~custom_body()                           = default;

        constexpr custom_body& operator=(custom_body&&) noexcept      = default;
        constexpr custom_body& operator=(custom_body const&) noexcept = default;



        // NOLINTBEGIN(bugprone-forwarding-reference-overload)
        // NOLINTEND(bugprone-forwarding-reference-overload)
    };



} // namespace webpp::http

#endif // WEBPP_CUSTOM_BODY_HPP
