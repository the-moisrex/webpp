// Created by moisrex on 12/26/22.

#ifndef WEBPP_CUSTOM_BODY_HPP
#define WEBPP_CUSTOM_BODY_HPP

#include "../../traits/enable_traits.hpp"

#include <compare>

namespace webpp::http {

    /**
     * This class is the mother of all custom bodies in which their common features will be presented here.
     */
    template <typename BodyType, TraitsAccess TraitsType = default_traits>
    struct custom_body : public enable_traits_access<TraitsType> {
        using traits_access  = enable_traits_access<TraitsType>;
        using traits_type    = typename traits_access::traits_type;
        using self_body_type = BodyType;

        template <typename T>
        static constexpr bool is_initable = !stl::same_as<custom_body, stl::remove_cvref_t<T>> &&
                                            !stl::same_as<self_body_type, stl::remove_cvref_t<T>>;

        constexpr custom_body()                            = default;
        constexpr custom_body(custom_body const&) noexcept = default;
        constexpr custom_body(custom_body&&) noexcept      = default;
        constexpr ~custom_body()                           = default;

        constexpr custom_body& operator=(custom_body&&) noexcept      = default;
        constexpr custom_body& operator=(custom_body const&) noexcept = default;


        // NOLINTBEGIN(bugprone-forwarding-reference-overload)
        template <EnabledTraits ET>
        constexpr custom_body(ET&& et) noexcept : traits_access{stl::forward<ET>(et)} {}

        template <typename T>
            requires(is_initable<T>)
        constexpr custom_body(T&& value) noexcept(stl::is_nothrow_invocable_v<self_body_type, T>) {
            as_self().operator=(stl::forward<T>(value));
        }
        // NOLINTEND(bugprone-forwarding-reference-overload)


        constexpr self_body_type& as_self() noexcept {
            return *static_cast<self_body_type*>(this);
        }

        constexpr self_body_type const& as_self() const noexcept {
            return *static_cast<self_body_type const*>(this);
        }
    };

    template <typename T, typename BodyType, typename TraitsType>
    [[nodiscard]] constexpr auto operator<=>(custom_body<BodyType, TraitsType> const& lhs, T&& rhs) {
        using type             = stl::remove_cvref_t<T>;
        using custom_body_type = custom_body<BodyType, TraitsType>;
        if constexpr (custom_body_type::template is_initable<type>) {
            return stl::compare_three_way{}(lhs.as_self(), custom_body_type{stl::forward<T>(rhs)});
        } else {
            return stl::partial_ordering::unordered;
        }
    }


} // namespace webpp::http

#endif // WEBPP_CUSTOM_BODY_HPP
