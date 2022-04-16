// Created by moisrex on 16/04/22.

#ifndef WEBPP_DATA_VIEW_HPP
#define WEBPP_DATA_VIEW_HPP

#include "../std/collection.hpp"
#include "../std/string.hpp"
#include "../std/string_view.hpp"
#include "../std/tuple.hpp"
#include "../std/type_traits.hpp"
#include "../traits/traits.hpp"
#include "../utils/functional.hpp"

#include <variant>

namespace webpp {



    // this view holds one data
    // this struct holds all the data
    template <Traits TraitsType, typename... ValuesT>
    struct view_data {
        using traits_type      = TraitsType;
        using string_view_type = traits::string_view<traits_type>;

        template <typename ValueT>
        struct view_component {
            using value_type = ValueT;

            string_view_type name;
            value_type       value;
        };


        template <typename V>
        struct common_value_of {

            static constexpr bool is_string = istl::StringViewifiable<V>;
            static constexpr bool is_bool   = stl::same_as<V, bool>;
            static constexpr bool is_lambda = requires(V v) {
                v(requires_arg(istl::String));
            }
            || requires(V v) {
                v(requires_arg(istl::String), requires_arg(stl::same_as<bool>));
            };
            static constexpr bool is_tuple      = istl::Tuple<V>;
            static constexpr bool is_collection = istl::Collection<V>;
            static constexpr bool is_list       = is_tuple || is_collection;

            using lambda = stl::variant<function_ref<string_view_type(string_view_type)>,
                                        function_ref<string_view_type(string_view_type, bool)>>;


            static constexpr bool value = true;
            using fix_tuple             = istl::recursive_parameter_replacer<V, common_value_of>;
            using as_list               = fix_tuple;

            // clang-format off
            using type = stl::conditional_t<
              is_bool,
              bool,
              stl::conditional_t<
                is_lambda,
                lambda,
                stl::conditional_t<
                  is_string,
                  string_view_type,
                  stl::conditional_t<
                    is_list,
                    as_list,
                    istl::nothing_type
                  >
                >
              >
            >;
            // clang-format on
        };
    };



} // namespace webpp


#endif // WEBPP_DATA_VIEW_HPP
