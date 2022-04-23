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

        using data_view_type = istl::nothing_type;
        using data_type      = istl::nothing_type;

      private:
        string_type data;

      public:
        constexpr file_view() = default;

        template <EnabledTraits ET>
            requires(!stl::same_as<stl::remove_cvref_t<ET>, file_view>)
        constexpr file_view(ET&& et) : data{et.alloc_pack.template general_allocator<char_type>()} {}

        constexpr file_view(file_view const&)     = default;
        constexpr file_view(file_view&&) noexcept = default;

        constexpr file_view& operator=(file_view const&) = default;
        constexpr file_view& operator=(file_view&&) noexcept = default;


        constexpr data_view_type generate_data_view(auto&&) const noexcept {
            return {};
        }


        void scheme(string_type&& str) noexcept {
            data = stl::move(str);
        }

        void scheme(string_view_type str) {
            data = str;
        }

        void render(string_type& out) const noexcept {
            out = data;
        }

        void render(string_type& out, data_view_type const&) const noexcept {
            out = data;
        }
    };



} // namespace webpp::views

#endif // WEBPP_HTML_VIEW_HPP
