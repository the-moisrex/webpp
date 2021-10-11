// Created by moisrex on 10/11/21.

#ifndef WEBPP_JSON_COMMON_HPP
#define WEBPP_JSON_COMMON_HPP

#include "../std/optional.hpp"
#include "../std/string_view.hpp"
#include "../std/tuple.hpp"

namespace webpp::json {

    template <typename T>
    struct field;

    template <typename... T>
    struct field_pack : public stl::tuple<field<T>&...> {

        template <typename NewT>
        [[nodiscard]] operator,(field<NewT>& input_field) noexcept {
            return field_pack<T&..., NewT&>{stl::tuple_cat(*this, stl::tuple<field<NewT>&>(input_field))};
        }
    };

    /**
     *  A json field, containing a key and a value of specified type.
     *  Initial idea: https://github.com/the-moisrex/webpp/issues/13#issuecomment-940163493
     */
    template <typename T>
    struct field : public stl::optional<T> {
        using key_type            = stl::string_view;
        using value_type          = T;
        using optional_value_type = stl::optional<value_type>;

        key_type key;

        constexpr field(key_type input_key) noexcept : key{input_key} {}
        constexpr field(key_type input_key, value_type const& input_value) noexcept
          : optional_value_type{input_value},
            key{input_key} {}

        constexpr field(key_type input_key, value_type&& input_value) noexcept
          : optional_value_type{stl::move(input_value)},
            key{input_key} {}


        template <typename NewT>
        [[nodiscard]] operator,(field<NewT>& input_field) noexcept {
            return field_pack<T, NewT>{*this, input_field};
        }
    };
} // namespace webpp::json

#endif // WEBPP_JSON_COMMON_HPP
