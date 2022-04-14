// Created by moisrex on 16/1/22.

#ifndef WEBPP_VIEW_CONCEPTS_HPP
#define WEBPP_VIEW_CONCEPTS_HPP

#include "../std/concepts.hpp"
#include "../std/string_concepts.hpp"

namespace webpp::views {

    template <typename T>
    concept ViewData = istl::All<T>;

    template <typename T>
    concept ViewManager = requires (T man) {
                              man.render();
                          };

    /**
     * Features of a view:
     *
     *   - [ ] Cache-able. (we don't need the data to be cached, only the rendered scheme)
     */
    template <typename T>
    concept View = requires(T view) {
        view.scheme(requires_arg(istl::StringViewifiable)); // reparse, and change the scheme

        // render with the data passed to it
        view.render(satisfy_arg(istl::String<_> && stl::is_lvalue_reference_v<_>), // string ref
                    requires_arg(ViewData)                                         // any view data
        );
    };

} // namespace webpp::views

#endif // WEBPP_VIEW_CONCEPTS_HPP
