// Created by moisrex on 14/04/22.

#ifndef WEBPP_HTML_VIEW_HPP
#define WEBPP_HTML_VIEW_HPP

#include "../traits/traits.hpp"

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
        void render(ViewManager auto&, istl::String auto& out, DataViews auto const&) const {
            out = data;
        }
    };



} // namespace webpp

#endif // WEBPP_HTML_VIEW_HPP
