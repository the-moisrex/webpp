// Created by moisrex on 16/1/22.

#ifndef WEBPP_VIEW_CONCEPTS_HPP
#define WEBPP_VIEW_CONCEPTS_HPP

namespace webpp::views {

    /**
     * Features of a view:
     *
     *   - [ ] Cache-able. (we don't need the data to be cached, only the rendered scheme)
     */
    template <typename T>
    concept View = requires(T view) {
                       view.scheme("");                  // reparse, and change the scheme
                       view.render(/* data or empty */); // render with the data passed to it
                   };

} // namespace webpp::views

#endif // WEBPP_VIEW_CONCEPTS_HPP
