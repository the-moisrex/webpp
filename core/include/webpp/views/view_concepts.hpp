// Created by moisrex on 16/1/22.

#ifndef WEBPP_VIEW_CONCEPTS_HPP
#define WEBPP_VIEW_CONCEPTS_HPP

#include "../std/string.hpp"
#include "../std/string_concepts.hpp"
#include "../std/string_view.hpp"
#include "../std/tuple.hpp"

namespace webpp::views {

    template <typename T>
    concept DataView = requires(T item) {
        item.name;
        item.value;
    };

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
    concept ViewManager = requires(T man) {
        man.render(requires_arg(istl::StringViewifiable));
    };

    template <typename T>
    concept View = requires(T view) {
        view.scheme(requires_arg(istl::StringViewifiable)); // reparse, and change the scheme

        // render with the data passed to it
        view.render(satisfy_arg(ViewManager<_> && stl::is_lvalue_reference_v<_>),  // view_manager ref
                    satisfy_arg(istl::String<_> && stl::is_lvalue_reference_v<_>), // string ref
                    requires_arg(DataView)                                         // any view data
        );
    };

} // namespace webpp::views

#endif // WEBPP_VIEW_CONCEPTS_HPP
