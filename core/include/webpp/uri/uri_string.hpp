#ifndef WEBPP_UTILS_URI_H
#define WEBPP_UTILS_URI_H

#include "../std/map.hpp"
#include "../std/optional.hpp"
#include "../std/vector.hpp"
#include "../strings/charset.hpp"
#include "../strings/to_case.hpp"
#include "../traits/traits_concepts.hpp"
#include "../utils/allocators.hpp"
#include "../utils/casts.hpp"
#include "../utils/ipv4.hpp"
#include "../utils/ipv6.hpp"
#include "../validators/validators.hpp"

/**
 * URI features that we might need separately:
 *   - Full URI parser/creator
 *     - scheme
 *     - user info
 *     - host
 *     - port
 *     - path
 *     - queries
 *     - fragments
 *
 * There are two types of URI types:
 *   1. String Based: designed for parsing the URIs
 *   2. Structured  : designed to change or create URIs
 */

namespace webpp::uri {

    /**
     * This class represents a Uniform Resource Identifier (URI),
     * as defined in RFC 3986 (https://tools.ietf.org/html/rfc3986).
     *
     * Most URIs will never change in their life time (at least in webpp
     * project) and they mostly used to get details of the URL we have as a
     * string; so I decided that probably half of the calculations can be done
     * at compile time; so fo that point, I reimplemented the URI class with
     *  and string_view in mind.
     *
     * RFC: https://tools.ietf.org/html/rfc3986
     *
     *    foo://example.com:8042/over/there?name=ferret#nose
     *    \_/   \______________/\_________/ \_________/ \__/
     *     |           |            |            |        |
     *  scheme     authority       path        query   fragment
     *     |   _____________________|__
     *    / \ /                        \
     *    urn:example:animal:ferret:nose
     *
     *  [protocol"://"[username[":"password]"@"]hostname[":"port]"/"?][path]["?"querystring]["#"fragment]
     */
    template <Traits TraitsType, bool Mutable = true>
    struct uri_string {
      public:
        using traits_type = TraitsType;
        using char_type   = typename traits_type::char_type;

        /**
         * Getting the appropriate string type to use.
         * If the specified string type cannot be changed, the string_view will
         * be used, otherwise, string itself.
         */
        using storred_str_t = auto_string_type<TraitsType, Mutable>;
        using str_t         = typename traits_type::string_type;
        using str_view_t    = typename traits_type::string_view_type;

      private:

      public:
        constexpr uri_string(allocator_type const& alloc = allocator_type{}) noexcept
          : alloc_holder_type{alloc} {
            static_assert(is_mutable(), "You can't modify this basic_uri, there's no point in "
                                        "default constructing this class since it can't be changed"
                                        " and thus can't be used in any way.");
        }

        ~uri_string() noexcept = default;

        static constexpr bool is_mutable() noexcept {
            return Mutable;
        }


        template <bool UMutable = Mutable>
        bool operator==(const uri_string<TraitsType, UMutable>& other) const noexcept {
            return str() == other.str();
        }

        template <bool UMutable = Mutable>
        bool operator!=(const uri_string<TraitsType, UMutable>& other) const noexcept {
            return str() != other.str();
        }

        bool operator==(str_view_t const& u) const noexcept {
            return str() == u;
        }

        bool operator!=(str_view_t const& u) const noexcept {
            return str() != u;
        }

        /**
         * Get a part of the uri
         * @param start
         * @param len
         * @return str_view_t
         */
        [[nodiscard]] str_view_t substr(stl::size_t const& start = 0,
                                        stl::size_t const& len   = str_view_t::npos) const noexcept {
            if (len == 0)
                return {};
            if constexpr (stl::is_same_v<storred_str_t, str_view_t>) {
                return data.substr(start, len);
            } else {
                return str_view_t(data).substr(start, len);
            }
        }

        /**
         * @brief this function is the same as "encodeURI" in javascript
         */
        void encode_to(istl::String auto& output) const noexcept {
            encode_uri_component(str(), output, ALLOWED_CHARACTERS_IN_URI);
        }

        /**
         * @brief this function is the same as "decodeURI" in javascript
         * @return this function will return an optional<string> object. it will
         * be nullopt when the uri is not valid and has invalid characters
         */
        [[nodiscard]] bool decode_to(istl::String auto& output) const noexcept {
            return decode_uri_component(str(), output, ALLOWED_CHARACTERS_IN_URI);
        }

        /**
         * This method returns an indication of whether or not the URI includes
         * any element that is part of the authority URI.
         * @return bool
         */
        [[nodiscard]] bool has_authority() const noexcept {
            return !host.empty() || has_user_info() || has_port();
        }

        /**
         * Get the string representation of the uri
         * @return string
         */
        [[nodiscard]] storred_str_t const& str() const noexcept {
            return data;
        }

        /**
         * Get a string_view version of the uri
         * @return str_view_t
         */
        [[nodiscard]] str_view_t string_view() const noexcept {
            if constexpr (stl::is_same_v<storred_str_t, str_view_t>) {
                return data;
            } else {
                return str_view_t(data);
            }
        }

        /**
         * This method resolves the given relative reference, based on the given
         * base URI, returning the resolved target URI.
         *
         * @param[in] relative_uri
         *     This describes how to get to the target starting at the base.
         *
         * @return
         *     The resolved target URI is returned.
         *
         * @note
         *     It only makes sense to call this method on an absolute URI
         *     (in which I mean, the base URI should be absolute,
         *     as in IsRelativeReference() should return false).
         */
        [[nodiscard]] uri_string resolve(const uri_string& relative_uri) const noexcept {
            // Resolve the reference by following the algorithm
            // from section 5.2.2 in
            // RFC 3986 (https://tools.ietf.org/html/rfc3986).
            uri_string target;
            if (relative_uri.has_scheme()) {
                target = relative_uri;
                target.normalize_path();
            } else {
                target.scheme(scheme());
                target.fragment(relative_uri.fragment());
                if (relative_uri.has_host()) {
                    target.host(relative_uri.host());
                    target.port(relative_uri.port());
                    target.user_info(relative_uri.user_info());
                    target.path(relative_uri.path());
                    target.set_queries(relative_uri.queries_raw());
                    target.normalize_path();
                } else {
                    target.host(host());
                    target.user_info(user_info());
                    target.port(port());
                    if (!relative_uri.has_path()) {
                        target.path(path.raw());
                        if (relative_uri.has_queries()) {
                            target.set_queries(relative_uri.queries_raw());
                        } else {
                            target.set_queries(queries.raw());
                        }
                    } else {
                        target.set_queries(relative_uri._queries);
                        // RFC describes this as:
                        // "if (R.path starts-with "/") then"
                        if (relative_uri.is_absolute()) {
                            target.path(relative_uri._path);
                            target.normalize_path();
                        } else {
                            // RFC describes this as:
                            // "T.path = merge(Base.path, R.path);"
                            target.path(path());
                            auto target_path       = target.path_structured();
                            auto relative_uri_path = relative_uri.path_structured();
                            if (target_path.size() > 1) {
                                target_path.pop_back();
                            }
                            stl::copy(relative_uri_path.cbegin(), relative_uri_path.cend(),
                                      stl::back_inserter(target_path));
                            target.path(target_path);
                            target.normalize_path();
                        }
                    }
                }
            }

            return target;
        }

        /**
         * Check if the specified URI is in fact a URN
         *
         * URN syntax based on https://tools.ietf.org/html/rfc8141#section-2
         *
         *      namestring    = assigned-name
         *                      [ rq-components ]
         *                      [ "#" f-component ]
         *      assigned-name = "urn" ":" NID ":" NSS
         *      NID           = (alphanum) 0*30(ldh) (alphanum)
         *      ldh           = alphanum / "-"
         *      NSS           = pchar *(pchar / "/")
         *      rq-components = [ "?+" r-component ]
         *                      [ "?=" q-component ]
         *      r-component   = pchar *( pchar / "/" / "?" )
         *      q-component   = pchar *( pchar / "/" / "?" )
         *      f-component   = fragment
         *
         */
        [[nodiscard]] bool is_urn() const noexcept {
            return scheme() == "urn" && authority_start == data.size();
        }

        /**
         * Check if the specified URI is in fact a URL
         */
        [[nodiscard]] bool is_url() const noexcept {
            return !host.empty();
        }

        /**
         * Check if the specified string is a valid URI or not
         */
        [[nodiscard]] bool is_valid() const noexcept {
            return has_scheme() || has_authority() || !path.empty() || has_fragment();
        }

    };

    template <istl::CharType CharT = char>
    uri_string(CharT const*) -> uri_string<basic_std_traits<CharT>, false>;
    // basic_uri(const char[])->basic_uri<std::string_view>;

    template <typename CharT = char>
    uri_string(stl::basic_string_view<CharT>) -> uri_string<basic_std_traits<CharT>, false>;

    template <typename CharT = char>
    uri_string(stl::basic_string<CharT>) -> uri_string<basic_std_traits<CharT>, true>;

    using const_uri = uri_string<std_traits, false>;
    using uri       = uri_string<std_traits, true>;


    template <Traits TraitsType, bool Mutable1, bool Mutable2>
    bool operator==(uri_string<TraitsType, Mutable1> const& one,
                    uri_string<TraitsType, Mutable2> const& two) noexcept {
        return one.operator==(two.str());
    }


    //    template <typename CharT>
    //    bool operator==(uri<CharT> const&       one,
    //                    const_uri<CharT> const& two) noexcept {
    //        return one.operator==(two.str());
    //    }
    //
    //    template <typename CharT>
    //    bool operator==(const_uri<CharT> const& one,
    //                    uri<CharT> const&       two) noexcept {
    //        return one.operator==(two.str());
    //    }
    //
    template <Traits TraitsType, bool Mutable1, bool Mutable2>
    [[nodiscard]] bool equal_path(uri_string<TraitsType, Mutable1> const& p1,
                                  uri_string<TraitsType, Mutable2> const& p2) noexcept {
        auto _p1 = p1.path.slugs();
        auto _p2 = p2.path.slugs();
        auto it2 = _p2.cbegin();
        auto it1 = _p1.cbegin();
        while (it1 != _p1.cend() && it2 != _p2.cend()) {
            if (*it1 != *it2) {
                if (stl::empty(*it1)) {
                    ++it1;
                    continue;
                } else if (stl::empty(*it2)) {
                    ++it2;
                    continue;
                }

                return false;
            }
            ++it1;
            ++it2;
        }

        if (it1 != _p1.cend()) {
            if (!stl::all_of(
                  it1, _p1.cend(), [](auto const& a) constexpr noexcept { return stl::empty(a); }))
                return false;
        }
        if (it1 != _p2.cend()) {
            if (!stl::all_of(
                  it2, _p2.cend(), [](auto const& a) constexpr noexcept { return stl::empty(a); }))
                return false;
        }
        return true;
    }

    [[nodiscard]] inline auto equal_path(stl::string_view const& p1, stl::string_view const& p2) noexcept {
        return p1 == p2 || equal_path<std_traits, false, false>(const_uri{p1}, const_uri{p2});
    }

    template <Traits TraitsType, bool Mutable>
    [[nodiscard]] inline auto equal_path(uri_string<TraitsType, Mutable> const&        p1,
                                         typename TraitsType::string_view_type const& p2) noexcept {
        return p1 == p2 || equal_path<TraitsType, false>(p1, const_uri{p2});
    }

    template <Traits TraitsType, bool Mutable>
    [[nodiscard]] inline auto equal_path(typename TraitsType::string_view_type const& p1,
                                         uri_string<TraitsType, Mutable> const&        p2) noexcept {
        return p2 == p1 || equal_path<TraitsType, false>(const_uri{p1}, p2);
    }

} // namespace webpp

#endif // WEBPP_UTILS_URI_H
