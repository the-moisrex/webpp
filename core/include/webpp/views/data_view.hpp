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

#include <variant>

namespace webpp::views {


    enum struct data_views { boolean, lambda, string, list };

    using view_data_flags = flags::manager<data_views>;


    /**
     * This is the inter-mediate way that the data is passed from the user's data types
     * and the view data types (view as in mustache and json_view and others).
     *
     * This struct WILL convert the data to make it more interesting for the view.
     * In the beginning, I didn't want this struct to be able to hold a value and just be a view, but
     * turns out that in order to convert types like "integers" into "strings" I do need to have a way
     * of storing strings and not just storing a view of the string because it's impossible to store a view
     * of a string that doesn't exists.
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

        static constexpr bool need_bool   = acceptable_types.is_on(data_views::boolean);
        static constexpr bool need_string = acceptable_types.is_on(data_views::string);
        static constexpr bool need_lambda = acceptable_types.is_on(data_views::lambda);
        static constexpr bool need_list   = acceptable_types.is_on(data_views::list);

        using lambda = stl::variant<function_ref<string_view_type(string_view_type)>,
                                    function_ref<string_view_type(string_view_type, bool)>>;

        using list_type = traits::generalify_allocators<traits_type, stl::vector<value_type>>;

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

            struct collection_view_calculator {
                template <bool>
                struct evaluate {
                    using value_type      = typename V::value_type;
                    using new_value_type  = typename component_view<value_type>::value_type;
                    using collection_type = istl::replace_parameter<V, value_type, new_value_type>;
                    using type            = traits::generalify_allocators<traits_type, collection_type>;
                };
            };

            struct tuple_view_calculator {

                template <typename VV>
                struct fix_val {
                    static constexpr bool value = true;
                    using type                  = typename component_view<VV>::value_type;
                };

                template <bool>
                struct evaluate {
                    using type = istl::recursive_parameter_replacer<V, fix_val>;
                };
            };

            // okay debate:
            // do we need to use a collection
            using collection_view = istl::lazy_conditional_t<is_collection, collection_view_calculator, V>;
            using tuple_view      = istl::lazy_conditional_t<is_tuple, tuple_view_calculator, V>;
            using list_view       = stl::conditional_t<is_collection, collection_view, tuple_view>;



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
                            list_view,
                            stl::conditional_t<
                                is_convertible_to_string,
                                string_type,
                                istl::nothing_type
                            >
                        >
                    >
                >
            >;
            // clang-format on


          private:
            string_view_type name;
            value_type       value;

          public:
            template <typename StrT>
                requires(istl::StringViewifiableOf<string_view_type, StrT>)
            void set_name(StrT&& str) noexcept {
                name = istl::string_viewify_of<string_view_type>(stl::forward<StrT>(str));
            }

            template <typename T, EnabledTraits ET>
            static void set_value(T const& val, auto& out, ET&& et) {
                // I'm passing val as a const& and not a && because this function cannot handle moves since
                // we might make a string_view of val and then val is going out of scope for the user of this
                // function and our string view is gone too with it. So no moves :)

                if constexpr ((need_bool && is_bool) || (need_lambda && is_lambda)) {
                    out = val;
                } else if constexpr (need_string && is_string) {
                    out = istl::string_viewify_of<string_view_type>(stl::forward<T>(val));
                } else if constexpr (need_string && is_convertible_to_string) {
                    // yes, value_type now should be a string type not a string view type
                    out = lexical::cast<string_type>(val, et.alloc_pack);
                } else if constexpr (need_list && is_collection) {
                    using collection_value_type = typename value_type::value_type;
                    out                         = object::make_general<value_type>(et.alloc_pack);
                    out.reserve(val.size());
                    stl::transform(stl::begin(val), stl::end(val), stl::back_inserter(out), [&](auto&& v) {
                        auto nv = object::make_general<collection_value_type>(et.alloc_pack);
                        component_view::set_value(v, nv, et);
                        return nv;
                    });
                } else if constexpr (need_list && is_tuple) {
                    tuple_transform(val, out, [&]<typename OldT>(OldT&& old_val) {
                        using new_view_type = typename component_view<OldT>::value_type;
                        auto nv             = object::make_general<new_view_type>(et.alloc_pack);
                        component_view::set_value(old_val, nv, et);
                        return nv;
                    });
                }
            }

            template <typename T, EnabledTraits ET>
            void set_value(T const& val, ET&& et) {
                component_view::set_value(val, value, et);
            }


            string_view_type get_name() const noexcept {
                return name;
            }

            auto get_value() const noexcept {
                if constexpr (istl::String<value_type> && !istl::StringView<value_type>) {
                    // convert to string view if it's a normal string
                    return istl::string_viewify_of<string_view_type>(value);
                } else {
                    return value;
                }
            }
        };


        using tuple_of_types = stl::tuple<stl::conditional_t<need_bool, bool, void>,
                                          stl::conditional_t<need_lambda, lambda, void>,
                                          stl::conditional_t<need_string, string_view_type, void>,
                                          stl::conditional_t<need_list, list_type, void>>;
        using unique_types   = typename istl::unique_parameters<tuple_of_types>::type;

        using type = istl::replace_templated_parameter<unique_types, stl::tuple, stl::variant>;
    };



} // namespace webpp::views


#endif // WEBPP_DATA_VIEW_HPP
