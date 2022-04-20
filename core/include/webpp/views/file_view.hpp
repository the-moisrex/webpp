// Created by moisrex on 14/04/22.

#ifndef WEBPP_HTML_VIEW_HPP
#define WEBPP_HTML_VIEW_HPP

#include "view_concepts.hpp"

namespace webpp::views {

    /**
     * File view
     *
     * File view does no rendering or parsing. The exact input is outputted.
     */
    template <Traits TraitsType>
    struct file_view {
        using traits_type      = TraitsType;
        using string_view_type = traits::string_view<traits_type>;
        using string_type      = traits::general_string<traits_type>;
        using char_type        = traits::char_type<traits_type>;

        using data_view_type = string_view_type;
        using data_type      = string_type;

      private:
        string_type data;

      public:
        constexpr file_view() = default;

        template <EnabledTraits ET>
        constexpr file_view(ET const& et) : data{et.alloc_pack.template get_allocator<char_type>()} {}


        void scheme(string_type&& str) noexcept {
            data = str;
        }

        void scheme(string_type const& str) {
            data = str;
        }

        void render(string_type& out, data_view_type const&) const noexcept {
            // the view manager and the input data are useless.
            out = data;
        }
    };



} // namespace webpp::views

#endif // WEBPP_HTML_VIEW_HPP
