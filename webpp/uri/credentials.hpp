// Created by moisrex on 1/23/24.

#ifndef WEBPP_URI_CREDENTIALS_HPP
#define WEBPP_URI_CREDENTIALS_HPP

#include "../std/string.hpp"
#include "../std/string_view.hpp"

namespace webpp::uri {

    /// Serialize username
    template <typename CharT, typename AllocT>
    static constexpr void render_username(
      stl::basic_string_view<CharT>                              storage,
      stl::basic_string<CharT, stl::char_traits<CharT>, AllocT>& out) noexcept(false) {
        // https://url.spec.whatwg.org/#url-serializing
        if (storage.empty()) {
            return;
        }
        out.append(storage);
    }

    /// Serialize password
    template <typename CharT, typename AllocT>
    static constexpr void render_password(
      stl::basic_string_view<CharT>                              storage,
      stl::basic_string<CharT, stl::char_traits<CharT>, AllocT>& out) noexcept(false) {
        // https://url.spec.whatwg.org/#url-serializing
        if (storage.empty()) {
            return;
        }
        out.append(storage);
    }

    /// Serialize authority which includes username, password, hostname, and port
    template <typename CharT, typename AllocT>
    static constexpr void render_authority(
      stl::basic_string_view<CharT> const&                       username,
      stl::basic_string_view<CharT> const&                       password,
      stl::basic_string_view<CharT> const&                       hostname,
      stl::basic_string_view<CharT> const&                       port,
      stl::basic_string<CharT, stl::char_traits<CharT>, AllocT>& out) {
        // https://url.spec.whatwg.org/#url-serializing
        // Authority format: [userinfo@]host[:port]

        bool const has_userinfo = !username.empty();
        bool const has_password = !password.empty();
        bool const has_hostname = !hostname.empty();
        bool const has_port     = !port.empty();

        if (has_userinfo) {
            out.append(username);
            if (has_password) {
                out.push_back(':');
                out.append(password);
            }
            out.push_back('@');
        }

        if (has_hostname) {
            out.append(hostname);
        }

        if (has_port) {
            out.push_back(':');
            out.append(port);
        }
    }

} // namespace webpp::uri

#endif // WEBPP_URI_CREDENTIALS_HPP
