// Created by moisrex on 10/11/21.

#ifndef WEBPP_JSON_COMMON_HPP
#define WEBPP_JSON_COMMON_HPP

#include "../std/optional.hpp"
#include "../std/string_view.hpp"
#include "../std/tuple.hpp"
#include "json_concepts.hpp"

namespace webpp::json {


    /**
     * A helper to get the values out of json files
     */
    template <typename T, typename ObjT>
    requires requires(ObjT json_obj) {
        { json_obj.template as<T>() } -> stl::same_as<T>;
    }
    [[nodiscard]] static constexpr T as(ObjT&& obj) {
        return obj.template as<T>();
    }

    /**
     * A helper to check if the json value is of the specified type
     */
    template <typename T, typename ObjT>
    requires requires(ObjT json_obj) {
        { json_obj.template is<T>() } -> stl::same_as<bool>;
    }
    [[nodiscard]] static constexpr bool is(ObjT const& obj) {
        return obj.template is<T>();
    }


    template <typename T>
    struct field;

    template <typename... T>
    struct field_pack : public stl::tuple<field<T>&...> {

        /**
         * Add more field types to the field pack
         */
        template <typename NewT>
        [[nodiscard]] field_pack<T..., NewT> operator,(field<NewT>& input_field) noexcept {
            return {stl::tuple_cat(*this, stl::tuple<field<NewT>&>(input_field))};
        }

        /**
         * In this code:
         * @code
         *   (user_id, username, emails) = obj
         * @endcode
         * This is the operator= used there.
         */
        template <JSONObject ObjectType>
        field_pack& operator=(ObjectType& obj) {
            stl::apply(
              [&obj]<typename ValueType>(field<ValueType>& field) {
                  if (obj.has(field.key)) {
                      field = obj.template as<ValueType>;
                  }
              },
              *this);
            return *this;
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

        key_type key{};

        constexpr field(key_type input_key) noexcept : key{input_key} {}
        constexpr field(key_type input_key, value_type const& input_value) noexcept
          : optional_value_type{input_value},
            key{input_key} {}

        constexpr field(key_type input_key, value_type&& input_value) noexcept
          : optional_value_type{stl::move(input_value)},
            key{input_key} {}


        template <typename NewT>
        [[nodiscard]] field_pack<T, NewT> operator,(field<NewT>& input_field) noexcept {
            return {*this, input_field};
        }
    };
} // namespace webpp::json

#endif // WEBPP_JSON_COMMON_HPP
