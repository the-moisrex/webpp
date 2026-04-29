// Created by moisrex on 11/4/20.

#ifndef WEBPP_URI_QUERIES_HPP
#define WEBPP_URI_QUERIES_HPP

#include "./parser/parse_queries.hpp"
#include "uri_status.hpp"

namespace webpp::uri {

    /**
     * Serialize Queries. Assuming the input query is a valid query
     */
    template <typename CharT, typename AllocT>
    static constexpr void render_queries(
      stl::basic_string_view<CharT>                              storage,
      stl::basic_string<CharT, stl::char_traits<CharT>, AllocT>& out,
      uri_status_type const                                      status,
      bool const                                                 add_separators = false) {
        if (!has_flags(status, uri_status::has_non_null_queries)) {
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
    template <typename KeyValT, typename CharT, typename AllocT>
    static constexpr void render_queries(
      stl::span<KeyValT const>                                   storage,
      stl::basic_string<CharT, stl::char_traits<CharT>, AllocT>& out,
      uri_status_type const                                      status,
      bool const                                                 add_separators = false) {
        // https://url.spec.whatwg.org/#url-serializing
        if (!has_flags(status, uri_status::has_non_null_queries)) {
            return;
        }
        if (add_separators) {
            out += '?';
        }
        if (storage.empty()) [[unlikely]] {
            return;
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
    struct [[nodiscard]] basic_queries : stl::basic_string_view<CharT> {
        using string_view_type = stl::basic_string_view<CharT>;
        using char_type        = CharT;

        using string_view_type::string_view_type; // inherit constructors

        explicit constexpr basic_queries(stl::basic_string_view<CharT> const str) noexcept
          : stl::basic_string_view<CharT>{str} {}

        static constexpr auto allowed_chars = details::QUERY_OR_FRAGMENT_NOT_PCT_ENCODED<char_type>;

        [[nodiscard]] constexpr bool contains_key(string_view_type key) const noexcept {
            if (key.empty()) [[unlikely]] {
                return false;
            }
            auto pos = string_view_type::find(key);
            while (pos != string_view_type::npos) {
                auto const key_end = pos + key.size();

                // start boundary: starts with '&'
                bool found = pos == 0 || (*this)[pos - 1] == '&';

                // end boundary: and ends with '=' or '&'
                found &= key_end == string_view_type::size() || (*this)[key_end] == '=' || (*this)[key_end] == '&';

                if (found) {
                    return true;
                }
                pos = string_view_type::find(key, pos + 1);
            }
            return false;
        }

        template <typename... KeyT>
        [[nodiscard]] constexpr bool has(KeyT&&... keys) const noexcept {
            return (contains_key(stl::forward<KeyT>(keys)) && ...);
        }

        // todo: add equality check has_all("key2=val2&key1=val1") that would check if two queries are the same, even if their key ordering are not
    };

} // namespace webpp::uri

#endif // WEBPP_URI_QUERIES_HPP
