// Created by moisrex on 11/5/20.

#ifndef WEBPP_URI_PORT_HPP
#define WEBPP_URI_PORT_HPP

#include "../std/string.hpp"
#include "../std/string_view.hpp"
#include "./parser/parse_port.hpp"

namespace webpp::uri {

    /// Serialize port
    template <typename CharT, typename AllocT>
    static constexpr void render_port(
      stl::basic_string_view<CharT> const& storage,
      stl::basic_string<CharT, AllocT>&    out,
      bool const                           add_separators = false) {
        // https://url.spec.whatwg.org/#url-serializing
        // https://url.spec.whatwg.org/#serialize-an-integer
        if (storage.empty()) {
            return;
        }
        if (add_separators) {
            out.push_back(':');
        }
        out.append(storage);
    }

    /**
     * Check if the specified port is valid or not
     */
    [[nodiscard]] constexpr bool is_valid(int const port) noexcept {
        return port >= 0 && port < static_cast<int>(max_port_number);
    }

    /**
     * Is the specified port the default port for the specified scheme or not?
     */
    template <typename CharT>
    [[nodiscard]] constexpr bool is_default_port(int const port, stl::basic_string_view<CharT> const scheme) noexcept {
        return known_port(scheme) == port;
    }

    /**
     * Check if the specified port is a well known port or not
     */
    [[nodiscard]] constexpr bool is_well_known(int const port) noexcept {
        return port >= 0 && port < static_cast<int>(well_known_upper_port);
    }

} // namespace webpp::uri

#endif // WEBPP_URI_PORT_HPP
