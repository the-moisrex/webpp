// Created by moisrex on 16/04/22.

#ifndef WEBPP_DATA_VIEW_HPP
#define WEBPP_DATA_VIEW_HPP

#include "../convert/lexical_cast.hpp"
#include "../std/collection.hpp"
#include "../std/string.hpp"
#include "../std/string_view.hpp"
#include "../std/tuple.hpp"
#include "../std/type_traits.hpp"
#include "../traits/traits.hpp"
#include "../utils/flags.hpp"
#include "../utils/functional.hpp"
#include "view_concepts.hpp"
#include "webpp/std/type_traits.hpp"
#include "webpp/traits/traits.hpp"

#include <variant>

namespace webpp::views {


    enum struct data_views { boolean, lambda, string, list };

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
    template <DataViewSettings auto Settings>
    struct data_view {
        using settings         = decltype(Settings);
        using traits_type      = typename settings::traits_type;
        using string_view_type = traits::string_view<traits_type>;
        using string_type      = traits::general_string<traits_type>;

        static constexpr view_data_flags acceptable_types = Settings.acceptable_types;

        template <typename V>
        struct component_view {
            static constexpr bool is_string                = istl::StringViewifiable<V>;
            static constexpr bool is_convertible_to_string = requires(V val) {
                lexical::cast<string_view_type>(val);
            };
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

            struct collection_view_calculator {
                template <bool>
                struct evaluate {
                    using value_type      = typename V::value_type;
                    using new_value_type  = typename component_view<value_type>::value_type;
                    using collection_type = istl::replace_parameter<V, value_type, new_value_type>;
                    using type            = traits::generalify_allocators<traits_type, collection_type>;
                };
            };

            using collection_view = istl::lazy_conditional_t<is_collection, collection_view_calculator, V>;


            static constexpr bool need_bool   = acceptable_types.is_on(data_views::boolean);
            static constexpr bool need_string = acceptable_types.is_on(data_views::string);
            static constexpr bool need_lambda = acceptable_types.is_on(data_views::lambda);
            static constexpr bool need_list   = acceptable_types.is_on(data_views::list);

            // clang-format off
            using value_type = stl::conditional_t<
                need_bool && is_bool,
                bool,
                stl::conditional_t<
                    need_string && is_string,
                    string_view_type,
                    stl::conditional_t<
                        need_lambda && is_lambda,
                        lambda,
                        stl::conditional_t<
                            need_list && is_list,
                            list,
                            stl::conditional_t<
                                is_convertible_to_string,
                                string_type,
                                void
                            >
                        >
                    >
                >
            >;
            // clang-format on

            // static constexpr bool value = true;
            // using fix_tuple             = istl::recursive_parameter_replacer<V, common_value_of>;

          private:
            string_view_type name;
            value_type       value;

          public:
            template <typename StrT>
                requires(istl::StringViewifiableOf<string_view_type, StrT>)
            void set_name(StrT&& str) noexcept {
                name = istl::string_viewify_of<string_view_type>(stl::forward<StrT>(str));
            }

            template <typename T>
            void set_value(T&& val) {
                if constexpr (need_bool && is_bool) {
                    value = val;
                } else if constexpr (acceptable_types.is_on(data_views::string) && is_string) {
                    value = istl::string_viewify_of<string_view_type>(stl::forward<T>(val));
                } else if constexpr (acceptable_types.is_on(data_views::lambda) && is_lambda) {
                    value = val;
                } else if constexpr (acceptable_types.is_on(data_views::string) && is_convertible_to_string) {
                }
            }
        };


        //        template <typename V>
        //        struct common_value_of {
        //
        //
        //            // clang-format off
        //            using type = stl::conditional_t<
        //                is_bool,
        //                bool,
        //                stl::conditional_t<
        //                    is_lambda,
        //                    lambda,
        //                    stl::conditional_t<
        //                        is_string,
        //                        string_view_type,
        //                        stl::conditional_t<
        //                            is_list,
        //                            as_list,
        //                            istl::nothing_type
        //                        >
        //                    >
        //                >
        //            >;
        //            // clang-format on
        //        };
    };



} // namespace webpp::views


#endif // WEBPP_DATA_VIEW_HPP
