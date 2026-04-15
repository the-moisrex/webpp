// Created by moisrex on 10/30/20.

#ifndef WEBPP_URI_SCHEME_HPP
#define WEBPP_URI_SCHEME_HPP

#include "../std/string.hpp"
#include "../std/string_view.hpp"
#include "./parser/special_schemes.hpp"

namespace webpp::uri {

    /// Serialize scheme
    template <typename CharT, typename AllocT>
    static constexpr void render_scheme(
      stl::basic_string_view<CharT> const&                       storage,
      stl::basic_string<CharT, stl::char_traits<CharT>, AllocT>& out,
      bool const                                                 add_separators = false) {
        // https://url.spec.whatwg.org/#url-serializing
        if (storage.empty()) {
            return;
        }
        out.append(storage);
        if (add_separators) {
            out.push_back(':');
        }
    }

    /**
     * Scheme or Protocol
     */
    template <typename CharT>
    struct [[nodiscard]] basic_scheme : stl::basic_string_view<CharT> {
        using char_type        = CharT;
        using string_view_type = stl::basic_string_view<CharT>;
        using string_type      = stl::basic_string<CharT>;

        // constructors
        using stl::basic_string_view<CharT>::basic_string_view;

        // Explicit constructor to handle basic_string_view initialization
        explicit constexpr basic_scheme(stl::basic_string_view<CharT> src) noexcept
          : stl::basic_string_view<CharT>(src) {}

        basic_scheme(basic_scheme const&)                = default;
        basic_scheme(basic_scheme&&) noexcept            = default;
        basic_scheme& operator=(basic_scheme const&)     = default;
        basic_scheme& operator=(basic_scheme&&) noexcept = default;
        ~basic_scheme() noexcept                         = default;

        /**
         * Return the default for the current scheme.
         *  - http, ws:   80
         *  - https, wss: 443
         *  - ftp:        21
         *  - others:     0
         */
        [[nodiscard]] constexpr stl::uint16_t known_port() const noexcept {
            return uri::known_port(*this);
        }

        /**
         * @brief checks if the URI is a relative reference
         */
        [[nodiscard]] constexpr bool is_relative_reference() const noexcept {
            return this->empty();
        }

        [[nodiscard]] constexpr bool is_special() const noexcept {
            return is_special_scheme(this->view());
        }

        constexpr void to_string(string_type& out, bool const append_separators = false) const {
            render_scheme(*this, out, append_separators);
        }
    };

} // namespace webpp::uri

#endif // WEBPP_URI_SCHEME_HPP
