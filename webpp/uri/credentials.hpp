// Created by moisrex on 1/23/24.

#ifndef WEBPP_URI_CREDENTIALS_HPP
#define WEBPP_URI_CREDENTIALS_HPP

#include "../std/string.hpp"
#include "../std/string_view.hpp"

namespace webpp::uri {

    /// Serialize username
    template <typename CharT, typename AllocT>
    static constexpr void render_username(stl::basic_string_view<CharT> storage, stl::basic_string<CharT, AllocT>& out)
      noexcept(false) {
        // https://url.spec.whatwg.org/#url-serializing
        if (storage.empty()) {
            return;
        }
        out.append(storage);
    }

    /// Serialize password
    template <typename CharT>
    static constexpr void render_password(stl::basic_string_view<CharT> storage, stl::basic_string<CharT>& out)
      noexcept(false) {
        // https://url.spec.whatwg.org/#url-serializing
        if (storage.empty()) {
            return;
        }
        out.append(storage);
    }

} // namespace webpp::uri

#endif // WEBPP_URI_CREDENTIALS_HPP
