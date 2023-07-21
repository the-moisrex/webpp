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

        using tuple_type = stl::tuple<field<T>&...>;

        using stl::tuple<field<T>&...>::tuple; // ctor

        /**
         * Add more field types to the field pack
         */
        template <typename NewT>
        [[nodiscard]] auto operator,(field<NewT>& input_field) noexcept {
            // this is tuple_cat, but using tuple_cat with non-tuple types is undefined behaviour (even though
            // they could've easily generalized it)
            // more info: https://en.cppreference.com/w/cpp/utility/tuple/tuple_cat
            return this->apply([&]<typename... OT>(OT&... fields) {
                return field_pack<T..., NewT>{fields..., input_field};
            });
        }


        template <typename F>
        constexpr decltype(auto) apply(F&& func) {
            return stl::apply(stl::forward<F>(func), *static_cast<tuple_type*>(this));
        }

        /**
         * In this code:
         * @code
         *   (user_id, username, emails) = obj
         * @endcode
         * This is the operator= used there.
         */
        template <JSONObject ObjectType>
        field_pack& operator=(ObjectType&& obj) {
            this->apply([&obj]<typename... ValueType>(field<ValueType>&... fields) {
                // it's the same as this simple if statement:
                // if (obj.contains(field.key) field = obj.as<value_type>();
                (void(obj.contains(fields.key) && ((fields = obj[fields.key].template as<ValueType>()))),
                 ...);
            });
            return *this;
        }

        template <typename ObjHolder>
            requires requires(ObjHolder holder) {
                         { holder.as_object() } -> JSONObject;
                     }
        field_pack& operator=(ObjHolder&& obj) {
            return operator=(obj.as_object());
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


        template <typename U>
        constexpr field& operator=(U&& val) {
            optional_value_type::operator=(stl::forward<T>(val));
            return *this;
        }

        // get a field pack
        // example: (username, password) = doc;
        template <typename NewT>
        [[nodiscard]] field_pack<T, NewT> operator,(field<NewT>& input_field) noexcept {
            return {*this, input_field};
        }
    };
} // namespace webpp::json

#endif // WEBPP_JSON_COMMON_HPP
