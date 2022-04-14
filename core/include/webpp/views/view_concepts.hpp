// Created by moisrex on 16/1/22.

#ifndef WEBPP_VIEW_CONCEPTS_HPP
#define WEBPP_VIEW_CONCEPTS_HPP

#include "../std/concepts.hpp"
#include "../std/string_concepts.hpp"

namespace webpp::views {

    template <typename T>
    concept ViewData = istl::All<T>;

    /**
     * Features of a view:
     *
     *   - [ ] Cache-able. (we don't need the data to be cached, only the rendered scheme)
     */
    template <typename T>
    concept View = requires(T view) {
        view.scheme(requires_arg(istl::StringViewifiable)); // reparse, and change the scheme

        // render with the data passed to it
        view.render(requires_arg(istl::String), requires_arg(ViewData));
    };

} // namespace webpp::views

#endif // WEBPP_VIEW_CONCEPTS_HPP
