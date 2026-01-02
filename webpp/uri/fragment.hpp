// Created by moisrex on 11/4/20.

#ifndef WEBPP_URI_FRAGMENT_HPP
#define WEBPP_URI_FRAGMENT_HPP

#include "../std/string_like.hpp"
#include "./parser/iiequals.hpp"
#include "./parser/parse_fragment.hpp"

namespace webpp::uri {

    /// Serialize fragment
    template <typename CharT>
    static constexpr void render_fragment(
      stl::basic_string_view<CharT> const storage,
      stl::basic_string<CharT>&           out,
      bool const                          add_separators = false) {
        // https://url.spec.whatwg.org/#url-serializing
        if (storage.empty()) {
            return;
        }
        if (add_separators) {
            out.push_back(static_cast<CharT>('#'));
        }
        out.append(storage);
    }

} // namespace webpp::uri

#endif // WEBPP_URI_FRAGMENT_HPP
