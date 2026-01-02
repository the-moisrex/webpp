// Created by moisrex on 11/4/20.

#ifndef WEBPP_URI_QUERIES_HPP
#define WEBPP_URI_QUERIES_HPP

#include "./parser/parse_queries.hpp"

#include <array>

namespace webpp::uri {

    /**
     * Serialize Queries. Assuming the input query is a valid query
     */
    template <typename CharT>
    static constexpr void render_queries(
      stl::basic_string_view<CharT> storage,
      stl::basic_string<CharT>&     out,
      bool const                    add_separators = false) {
        if (storage.empty()) {
            return;
        }
        if (add_separators) {
            out += '?';
        }
        out.append(storage);
    }

    /**
     * Serialize queries
     */
    template <typename KeyValT, typename CharT>
    static constexpr void
    render_queries(stl::span<KeyValT const> storage, stl::basic_string<CharT>& out, bool const add_separators = false) {
        // https://url.spec.whatwg.org/#url-serializing
        if (storage.empty()) {
            return;
        }
        if (add_separators) {
            out += '?';
        }
        for (auto pos = storage.begin();;) {
            auto const [name, value]  = *pos;
            out                      += name;
            if (!value.empty()) {
                out += '=';
                out += value;
            }
            if (++pos == storage.end()) {
                break;
            }
            out += '&';
        }
    }

    /**
     * @brief Basic Queries (or sometimes called Searches, like in WHATWG)
     */
    template <typename CharT>
    struct basic_queries : stl::basic_string_view<CharT> {
        using string_view_type = stl::basic_string_view<CharT>;
        using char_type        = CharT;

        static constexpr auto allowed_chars = details::QUERY_OR_FRAGMENT_NOT_PCT_ENCODED<char_type>;

        /// Equality check
        /// Attention: this function doesn't parse your input
        [[nodiscard]] constexpr bool has_all(string_view_type str) const noexcept {
            webpp_static_constexpr stl::array queries_separators{'=', '&'};

            while (!str.empty()) {
                auto const name = str.substr(0, str.find_first_of(queries_separators));
                str.remove_prefix(name.size());
                if (str.starts_with('=')) {
                    str.remove_prefix(1);
                }
                auto const value = str.substr(0, str.find_first_of('&'));
                if (auto const res = this->find(name); name.empty() || res == this->end() || res->second != value) {
                    return false;
                }
                str.remove_prefix(value.size());
                if (str.starts_with('&')) {
                    str.remove_prefix(1);
                }
            }
            return true;
        }
    };

} // namespace webpp::uri

#endif // WEBPP_URI_QUERIES_HPP
