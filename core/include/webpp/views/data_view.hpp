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
#include "view_concepts.hpp"
#include "../utils/flags.hpp"

#include <variant>

namespace webpp {


    enum struct data_views {
        boolean,
        lambda,
        string,
        list
    };

    using view_data_flags = flags::manager<data_views>;


    /**
     * This is the inter-mediate way that the data is passed from the user's data types
     * and the view data types (view as in mustache and json_view and others).
     *
     * This struct WILL convert the data to make it more interesting for the view.
     * This struct WILL not hold the actual data, it's just a view of the actual data.
     *
     * Valid views:
     *   - Boolean
     *   - String View
     *   - Lambdas
     *   - List (represented as std::span)
     */
    template <DataViewSettings Settings, istl::Tuple DataTup>
    struct view_data {
        using settings          = Settings;
        using traits_type       = typename settings::traits_type;
        using string_view_type  = traits::string_view<traits_type>;
        using passed_data       = DataTup;

        static constexpr view_data_flags acceptable_types = Settings.acceptable_types;

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
