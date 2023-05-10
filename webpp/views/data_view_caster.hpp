// Created by moisrex on 2/22/23.

#ifndef WEBPP_DATA_VIEW_CASTER_HPP
#define WEBPP_DATA_VIEW_CASTER_HPP

#include "../convert/lexical_cast.hpp"
#include "../std/collection.hpp"
#include "../std/type_traits.hpp"

// This file has the responsibility of converting common types into the types that the "views" can understand.
namespace webpp::view {

    template <typename To, EnabledTraits ET, typename From>
    [[nodiscard]] constexpr To data_view_caster([[maybe_unused]] ET&& et, From&& from) {
        using to_type   = stl::remove_cvref_t<To>;
        using from_type = stl::remove_cvref_t<From>;
        if constexpr (stl::same_as<from_type, to_type>) {
            return stl::forward<From>(from);
        } else if constexpr (stl::is_constructible_v<to_type, from_type>) {
            return to_type{stl::forward<From>(from)};
        } else if constexpr (stl::is_constructible_v<to_type, ET, from_type>) {
            return to_type{et, stl::forward<From>(from)};
        } else if constexpr (stl::is_constructible_v<to_type, from_type, ET>) {
            return to_type{stl::forward<From>(from), et};
        } else if constexpr (stl::integral<to_type>) {
            return lexical::cast<to_type>(stl::forward<From>(from));
        } else if constexpr (istl::String<to_type> || istl::StringView<to_type>) {
            return lexical::cast<to_type>(stl::forward<From>(from), alloc::general_alloc_for<to_type>(et));
        } else if constexpr (stl::is_convertible_v<from_type, to_type>) {
            return stl::forward<From>(from);
        } else if constexpr (istl::Pair<to_type> && istl::Pair<from_type>) {
            using first_type  = typename to_type::first_type;
            using second_type = typename to_type::second_type;
            if constexpr (stl::is_rvalue_reference_v<From>) { // move the items
                return to_type{data_view_caster<first_type>(et, stl::move(from.first)),
                               data_view_caster<second_type>(et, stl::move(from.second))};
            } else { // copy the items
                return to_type{data_view_caster<first_type>(et, from.first),
                               data_view_caster<second_type>(et, from.second)};
            }
            // todo: add tuple support
            // todo: add map support
        } else if constexpr (istl::Collection<to_type>) {
            // standard collections like vector, ...
            using value_type = typename to_type::value_type;
            to_type to{alloc::general_alloc_for<to_type>(et)};
            if constexpr (istl::ReadOnlyCollection<from_type>) {
                for (auto const& item : from) {
                    istl::collection::emplace(to, data_view_caster<value_type>(et, item));
                }
            } else {
                istl::collection::emplace(to, data_view_caster<value_type>(et, stl::forward<From>(from)));
            }
            return to;
        } else {
            static_assert_false(From, "We're not able to convert the specified type to the desired value.");
        }
    }

    template <typename To, EnabledTraits ET, typename... From>
        requires(sizeof...(From) >= 2)
    [[nodiscard]] constexpr To data_view_caster([[maybe_unused]] ET&& et, From&&... from) {
        using to_type = stl::remove_cvref_t<To>;
        if constexpr (istl::Collection<to_type>) {
            // standard collections like vector, ...
            using value_type = typename to_type::value_type;
            to_type to{alloc::general_alloc_for<to_type>(et)};
            (istl::collection::emplace(to, data_view_caster<value_type>(et, from)), ...);
            return to;
        } else {
            static_assert_false(To, "We're not able to convert the specified type to the desired value.");
        }
    }
} // namespace webpp::view

#endif // WEBPP_DATA_VIEW_CASTER_HPP
