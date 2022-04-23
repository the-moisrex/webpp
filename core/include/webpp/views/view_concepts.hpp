// Created by moisrex on 16/1/22.

#ifndef WEBPP_VIEW_CONCEPTS_HPP
#define WEBPP_VIEW_CONCEPTS_HPP

#include "../convert/lexical_cast.hpp"
#include "../std/collection.hpp"
#include "../std/string.hpp"
#include "../std/string_concepts.hpp"
#include "../std/string_view.hpp"
#include "../std/tuple.hpp"

namespace webpp::views {

    template <typename T>
    concept DataView = requires(T item) {
        item.key();
        item.value();
    };

    namespace details {
        template <typename T>
        struct is_data_view {
            static constexpr bool value = DataView<T>;
        };
    } // namespace details

    template <typename T>
    concept DataViews = stl::same_as<T, istl::nothing_type> || istl::ReadOnlyCollection<T> && requires {
        typename T::value_type;
        requires DataView<typename T::value_type>;
    };

    template <typename T>
    concept PossibleDataTypes = DataViews<T> || stl::integral<T> || istl::StringViewifiable<T> ||
      lexical::CastableTo<T, stl::string_view> || istl::Collection<T>;

    template <typename T>
    concept DataViewSettings = requires(T dv) {
        typename T::traits_type;
        Traits<typename T::traits_type>;
        dv.acceptable_types; // of type "view_data_flags"
    };

    /**
     * Features of a view:
     *
     *   - Cache-able. (we don't need the data to be cached, only the rendered scheme)
     *   - Cross-Format Transpiler
     */
    template <typename T>
    concept ViewManager = requires(T man, stl::string_view str) {
        { man.view(str) } -> istl::String;
    };

    template <typename T>
    concept ViewManagerInput = ViewManager<T> && stl::is_lvalue_reference_v<T>;

    template <typename T>
    concept View = requires(T view) {
        typename T::data_view_type;
        typename T::data_type;
        requires DataViews<typename T::data_view_type>;

        view.scheme(requires_arg(istl::StringViewifiable)); // reparse, and change the scheme

        // with the method, you can convert and store the converted data if the view need to, but it's not
        // recommended to store any data.
        view.generate_data_view(
          requires_arg_cvref(PossibleDataTypes)); // this data will be passed to the render

        // render with the data passed to it
        view.render(requires_arg_cvref(istl::String), // string ref
                    requires_arg_cvref(stl::same_as<typename T::data_view_type>));
    };

} // namespace webpp::views

#endif // WEBPP_VIEW_CONCEPTS_HPP
