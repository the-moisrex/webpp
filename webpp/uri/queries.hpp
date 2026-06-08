// Created by moisrex on 11/4/20.

#ifndef WEBPP_URI_QUERIES_HPP
#define WEBPP_URI_QUERIES_HPP

#include "./parser/parse_queries.hpp"
#include "parser/uri_context.hpp"
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
            if constexpr (istl::String<decltype(*pos)>) {
                out += *pos;
            } else {
                auto const [name, value]  = *pos;
                out                      += name;
                if (!value.empty()) {
                    out += '=';
                    out += value;
                }
            }
            if (++pos == storage.end()) {
                break;
            }
            out += '&';
        }
    }

    template <URIComponents CompT, typename CharT, typename AllocT>
    static constexpr void render_queries(
      CompT const&                                               comp,
      stl::basic_string<CharT, stl::char_traits<CharT>, AllocT>& out,
      uri_status_type const                                      status,
      bool                                                       add_separators = false) {
        if constexpr (URIStructuredComponents<CompT>) {
            using key_type = typename CompT::map_type::value_type;
            render_queries(stl::span<key_type const>{uri::queries(comp)}, out, status, add_separators);
        } else {
            render_queries(uri::queries_view(comp), out, status, add_separators);
        }
    }

    /// Only use this in quick tests, this possibly allocates.
    template <URIComponents CompT, typename StrT = stl::string>
    [[nodiscard]] static constexpr decltype(auto) render_queries(CompT const& comp, bool add_separators = false) {
        using enum uri_status;
        if constexpr (URIStructuredComponents<CompT>) {
            StrT out;
            // Delegate to the correct function, faking a valid status
            render_queries(comp, out, +valid | +has_non_null_queries, add_separators);
            return out;
        } else {
            return queries(comp);
        }
    }

    /// Only use this in quick tests, this possibly allocates.
    template <URIContext CtxT, typename StrT = stl::string>
    [[nodiscard]] static constexpr decltype(auto) render_queries(CtxT const& ctx, bool add_separators = false) {
        using enum uri_status;
        if constexpr (CtxT::is_segregated) {
            using key_type = typename CtxT::component_type::map_type::value_type;
            StrT out;
            // Delegate to the correct function, faking a valid status
            render_queries(stl::span<key_type const>{queries(ctx.out)}, out, ctx.status, add_separators);
            return out;
        } else {
            return queries(ctx.out);
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
