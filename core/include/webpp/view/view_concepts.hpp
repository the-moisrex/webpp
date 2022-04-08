// Created by moisrex on 16/1/22.

#ifndef WEBPP_VIEW_CONCEPTS_HPP
#define WEBPP_VIEW_CONCEPTS_HPP

namespace webpp {

    template <typename T>
    concept View = requires(T view) {
        view.page("home/page");
    };

} // namespace webpp

#endif // WEBPP_VIEW_CONCEPTS_HPP
