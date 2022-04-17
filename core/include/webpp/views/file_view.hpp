// Created by moisrex on 14/04/22.

#ifndef WEBPP_HTML_VIEW_HPP
#define WEBPP_HTML_VIEW_HPP

#include "view_concepts.hpp"

namespace webpp::views {

    /**
     * File view is
     */
    template <Traits TraitsType>
    struct file_view {
        using traits_type      = TraitsType;
        using string_view_type = traits::string_view<traits_type>;

      private:
        string_view_type data;

      public:
        void scheme(istl::StringViewifiable auto&& str) {
            data = istl::string_viewify_of<string_view_type>(stl::forward<decltype(str)>(str));
        }

        void render(ViewManager auto&, istl::String auto& out, DataViews auto const&) const {
            out = data;
        }
    };



} // namespace webpp::views

#endif // WEBPP_HTML_VIEW_HPP
