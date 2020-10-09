#ifndef WEBPP_UTILS_URI_H
#define WEBPP_UTILS_URI_H

#include "../std/map.hpp"
#include "../std/optional.hpp"
#include "../std/vector.hpp"
#include "../strings/ascii.hpp"
#include "../strings/charset.hpp"
#include "../traits/traits_concepts.hpp"
#include "../validators/validators.hpp"
#include "./casts.hpp"
#include "./ipv4.hpp"
#include "./ipv6.hpp"

namespace webpp {

    namespace is {
        /**
         * @brief check if scheme is correct or not
         * @param _scheme
         * @return
         */
        template <Traits TraitsType>
        [[nodiscard]] constexpr bool scheme(typename TraitsType::string_view_type const& _scheme) noexcept {
            return ALPHA<typename TraitsType::char_type>.contains(_scheme);
        }

    } // namespace is

    /**
     * @brief this function will decode parts of uri
     * @details this function is almost the same as "decodeURIComponent" in
     * javascript
     */
    template <Traits TraitsType, stl::size_t N>
    [[nodiscard]] stl::optional<typename TraitsType::string_type>
    decode_uri_component(typename TraitsType::string_view_type const&        encoded_str,
                         charset_t<typename TraitsType::char_type, N> const& allowed_chars) noexcept {
        int                            digits_left  = 2;
        typename TraitsType::char_type decoded_char = 0;
        // FIXME: decoding is assigned but never used; check if the algorithm is correct
        bool                             decoding = false;
        typename TraitsType::string_type res;
        for (const auto c : encoded_str) {
            if (decoding && digits_left) {
                decoded_char <<= 4;
                if (c >= '0' && c <= '9') { // DIGITS
                    decoded_char += c - '0';
                } else if (c >= 'A' && c <= 'F') { // UPPER_HEX
                    decoded_char += c - 'A' + 10;
                } else if (c >= 'a' && c <= 'f') { // LOWER_HEX
                    decoded_char += c - 'a' + 10;
                } else {
                    return stl::nullopt; // not encoded well
                }
                --digits_left;

                if (digits_left == 0) {
                    decoding = false;
                    res.push_back(static_cast<typename TraitsType::char_type>(decoded_char));
                }
            } else if (c == '%') {
                decoding = true;

                // resetting:
                digits_left  = 2;
                decoded_char = 0;
            } else {
                if (!allowed_chars.contains(c))
                    return stl::nullopt; // bad chars
                res.push_back(c);
            }
        }
        return stl::move(res);
    }

    /**
     * This method encodes the given URI element.
     * What we are calling a "URI element" is any part of the URI
     * which is a sequence of characters that:
     * - may be percent-encoded
     * - if not percent-encoded, are in a restricted set of characters
     *
     * @param[in] element
     *     This is the element to encode.
     *
     * @param[in] allowedCharacters
     *     This is the set of characters that do not need to
     *     be percent-encoded.
     *
     * @return
     *     The encoded element is returned.
     *
     *
     * @details this function is almost the same as "encodeURIComponent" in
     * javascript
     */
    template <Traits TraitsType, stl::size_t N>
    [[nodiscard]] typename TraitsType::string_type
    encode_uri_component(const typename TraitsType::string_view_type&        element,
                         const charset_t<typename TraitsType::char_type, N>& allowedCharacters) {
        using char_type     = typename TraitsType::char_type;
        auto make_hex_digit = [](unsigned int value) {
            if (value < 10) {
                return static_cast<char_type>(value + '0');
            } else {
                return static_cast<char_type>(value - 10 + 'A');
            }
        };

        typename TraitsType::string_type encodedElement;
        for (auto c : element) {
            if (allowedCharacters.contains(c)) {
                encodedElement.push_back(c);
            } else {
                encodedElement.push_back('%');
                encodedElement.push_back(make_hex_digit(static_cast<unsigned int>(c) >> 4u));
                encodedElement.push_back(make_hex_digit(static_cast<unsigned int>(c) & 0x0Fu));
            }
        }
        return encodedElement;
    }

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
    class basic_uri {
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

      public:
        /**
         * source:
         * https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/encodeURI
         */
        static constexpr auto ALLOWED_CHARACTERS_IN_URI =
          charset(ALPHA<char_type>, DIGIT<char_type>,
                  charset_t<char_type, 20>{';', ',', '/', '?', ':', '@', '&',  '=', '+', '$',
                                           '-', '_', '.', '!', '~', '*', '\'', '(', ')', '#'});
        /**
         * This is the character set corresponds to the second part
         * of the "scheme" syntax
         * specified in RFC 3986 (https://tools.ietf.org/html/rfc3986).
         */
        static constexpr auto SCHEME_NOT_FIRST =
          charset(ALPHA<char_type>, DIGIT<char_type>, charset_t<char_type, 3>{'+', '-', '.'});

        /**
         * This is the character set corresponds to the "unreserved" syntax
         * specified in RFC 3986 (https://tools.ietf.org/html/rfc3986).
         */
        static constexpr auto UNRESERVED =
          charset(ALPHA<char_type>, DIGIT<char_type>, charset_t<char_type, 4>{'-', '.', '_', '~'});

        /**
         * This is the character set corresponds to the "sub-delims" syntax
         * specified in RFC 3986 (https://tools.ietf.org/html/rfc3986).
         */
        static constexpr auto SUB_DELIMS =
          charset_t<char_type, 11>('!', '$', '&', '\'', '(', ')', '*', '+', ',', ';', '=');

        /**
         * This is the character set corresponds to the "userinfo" syntax
         * specified in RFC 3986 (https://tools.ietf.org/html/rfc3986),
         * leaving out "pct-encoded".
         */
        static constexpr auto USER_INFO_NOT_PCT_ENCODED =
          charset(UNRESERVED, SUB_DELIMS, charset_t<char_type, 1>{':'});

        /**
         * This is the character set corresponds to the last part of
         * the "IPvFuture" syntax
         * specified in RFC 3986 (https://tools.ietf.org/html/rfc3986).
         */
        static constexpr auto IPV_FUTURE_LAST_PART =
          charset(UNRESERVED, SUB_DELIMS, charset_t<char_type, 1>{':'});

        /**
         * This is the character set corresponds to the "reg-name" syntax
         * specified in RFC 3986 (https://tools.ietf.org/html/rfc3986),
         * leaving out "pct-encoded".
         */
        static constexpr auto REG_NAME_NOT_PCT_ENCODED = charset(UNRESERVED, SUB_DELIMS);

        /**
         * This is the character set corresponds to the "pchar" syntax
         * specified in RFC 3986 (https://tools.ietf.org/html/rfc3986),
         * leaving out "pct-encoded".
         */
        static constexpr auto PCHAR_NOT_PCT_ENCODED =
          charset(UNRESERVED, SUB_DELIMS, webpp::charset_t<char_type, 2>{':', '@'});

        /**
         * This is the character set corresponds to the "query" syntax
         * and the "fragment" syntax
         * specified in RFC 3986 (https://tools.ietf.org/html/rfc3986),
         * leaving out "pct-encoded".
         */
        static constexpr auto QUERY_OR_FRAGMENT_NOT_PCT_ENCODED =
          charset(PCHAR_NOT_PCT_ENCODED, charset_t<char_type, 2>{'/', '?'});

      private:
        /**
         * This is the whole url (if we need to own the uri ourselves)
         */
        storred_str_t data{};


        mutable stl::size_t scheme_end = str_view_t::npos, authority_start = str_view_t::npos,
                            user_info_end = str_view_t::npos, port_start = str_view_t::npos,
                            authority_end = str_view_t::npos, query_start = str_view_t::npos,
                            fragment_start = str_view_t::npos;

        /**
         * scheme    :    start=0       end=[0]
         * user_info :    start=[1]     end=[2]
         * host      :    start=[2|1]   end=[3|4|...]
         * port      :    start=[3]     end=[4]
         * path      :    start=[4]     end=[5]
         * query     :    start=[5]     end=[6]
         * fragment  :    start=[6]     end=[...]
         *
         * port_end == authority_end == path_start
         */

        /**
         * parse the scheme
         * this method will fill the "authority_start" and "scheme_end" vars
         */
        void parse_scheme() const noexcept {
            if (scheme_end != str_view_t::npos)
                return; // It's already parsed

            auto _data = this->string_view();

            // extracting scheme
            if (ascii::starts_with(_data, "//")) {
                authority_start = 2;
                scheme_end      = data.size(); // so we don't have to check again
                return;
            } else if (const auto colon = _data.find(':'); colon != str_view_t::npos) {
                auto __scheme = _data.substr(0, colon);
                if (ALPHA<char_type>.contains(__scheme[0]) &&
                    __scheme.substr(1).find_first_not_of(SCHEME_NOT_FIRST.string_view())) {
                    scheme_end = colon;

                    if (_data.substr(colon + 1, 2) == "//") {
                        authority_start = colon + 3;
                    } else {
                        // it should be a URN or an invalid URI at this point
                        authority_start = data.size();
                    }
                    return;
                }
            }

            scheme_end = authority_start = data.size();
        }

        /**
         * parse user info
         */
        void parse_user_info() const noexcept {
            if (user_info_end != str_view_t::npos)
                return; // It's already parsed

            parse_scheme(); // to get "authority_start"

            if (authority_start == data.size()) {
                user_info_end = data.size();
                return; // there's no user_info_end without authority_start
            }

            parse_path(); // to get "authority_end"

            auto _data = (stl::is_same_v<storred_str_t, str_view_t> ? data : str_view_t(data));

            user_info_end = _data.substr(authority_start, authority_end - authority_start).find_first_of('@');
            if (user_info_end == str_view_t::npos) {
                user_info_end = data.size();
            } else {
                user_info_end += authority_start;
            }
        }

        /**
         * parse path
         * this will make sure that the "authority_end" variable is filled
         */
        void parse_path() const noexcept {
            if (authority_end != str_view_t::npos)
                return; // It's already parsed

            parse_scheme(); // to get "authority_start"
            parse_query();  // to get "query_start"

            auto _data = this->string_view();

            auto starting_point =
              authority_start != data.size()
                ? authority_start
                : (scheme_end != data.size() && scheme_end != str_view_t::npos ? scheme_end : 0);
            authority_end = _data.substr(starting_point, query_start - starting_point).find_first_of('/');
            if (authority_end == str_view_t::npos) {
                authority_end = data.size();
            } else {
                authority_end += starting_point;
            }
        }

        /**
         * parse port
         * this makes sure that the "port_start" variable is filled
         */
        void parse_port() const noexcept {
            if (port_start != str_view_t::npos)
                return; // It's already parsed

            parse_user_info(); // to get "authority_start" and "user_info_end"

            if (authority_start == data.size()) {
                port_start = data.size();
                return; // there's no user_info_end without authority_start
            }

            parse_path(); // to get "authority_end"

            auto _data = this->string_view();

            auto starting_point = user_info_end != data.size() ? user_info_end : authority_start;
            port_start = _data.substr(starting_point, authority_end - starting_point).find_last_of(':');
            if (port_start == str_view_t::npos) {
                port_start = data.size(); // there's no port
            } else {
                port_start += starting_point;
                auto str_view = _data.substr(port_start + 1, authority_end - (port_start + 1));
                if (!is::digit(str_view)) {
                    port_start = data.size();
                }
            }
        }

        //        /**
        //         * parsing the authority parts of the basic_uri
        //         */
        //        void parse_authority_full() const noexcept {
        //
        //            parse_scheme();
        //            parse_path();
        //
        //            auto _data = (std::is_same_v<string_type,
        //            str_view_t>
        //                              ? data
        //                              : str_view_t(data))
        //                             .substr(authority_start, authority_end);
        //
        //            auto _authority_start = data.find("//");
        //            if (_authority_start != str_view_t::npos) {
        //                _data.remove_prefix(_authority_start + 2);
        //            }
        //            auto path_start = _data.find('/');
        //
        //            if (_authority_start != str_view_t::npos &&
        //            path_start != 0) {
        //
        //                auto port_start = _data.find(":", 0, path_start);
        //
        //                // extracting user info
        //                if (auto delim = _data.find("@", 0, path_start);
        //                    delim != str_view_t::npos) {
        //
        //                    pieces[_user_info] = delim;
        //                    _data.remove_prefix(delim + 1);
        //                }
        //
        //                // extracting host
        //
        //                /* IP future versions can be specified like this:
        //                 * (RFC 3986)
        //                 *
        //                 * IP-literal = "[" ( IPv6address / IPvFuture  ) "]"
        //                 * IPvFuture = "v" 1*HEXDIG "." 1*( unreserved /
        //                 sub-delims
        //                 * / ":" )
        //                 */
        //                if (ascii::starts_with(_data, '[')) { // IP
        //                Literal
        //                    if (_data.size() > 2 &&
        //                        _data[1] == 'v') { // IPv Future Number
        //                        if (auto dot_delim = _data.find('.');
        //                            dot_delim != str_view_t::npos) {
        //
        //                            auto ipvf_version = _data.substr(2,
        //                            dot_delim); if
        //                            (HEXDIG.contains(ipvf_version)) {
        //
        //                                if (auto ipvf_end = _data.find(']');
        //                                    ipvf_end !=
        //                                    str_view_t::npos) { auto
        //                                    ipvf =
        //                                        _data.substr(dot_delim + 1,
        //                                        ipvf_end);
        //                                    if
        //                                    (IPV_FUTURE_LAST_PART.contains(ipvf))
        //                                    {
        //                                        // returning the ipvf and it's
        //                                        // version
        //                                        _host = _data.substr(1,
        //                                        ipvf_end + 1);
        //                                    }
        //                                } else {
        //                                    // totally not a URI
        //                                    return;
        //                                }
        //                            }
        //                        }
        //                    } else if (_data.size() >= 5) { // IPv6
        //
        //                        if (auto ipv6_end = _data.find(']');
        //                            ipv6_end != str_view_t::npos) {
        //
        //                            if (auto ipv6_view = _data.substr(1,
        //                            ipv6_end);
        //                                is::ipv6(ipv6_view)) {
        //                                // TODO: probably use
        //                                std::variant<ipv6,
        //                                // ipv4, string>
        //                                _host = ipv6_view;
        //                                _data.remove_prefix(ipv6_end + 1);
        //                            }
        //                        } else {
        //                            return; // totally not a valid URI
        //                        }
        //                    }
        //                } else { // Not IP Literal
        //                    auto port_or_path_start =
        //                        port_start != str_view_t::npos ?
        //                        port_start
        //                                                             :
        //                                                             path_start;
        //                    auto hostname = _data.substr(0,
        //                    port_or_path_start);
        //                    // we're not going to decode hostname here. We'll
        //                    do
        //                    // this in another method because this function is
        //                    //  and will only return const stuff
        //
        //                    // we have our answer but we will check for the
        //                    // correctness of the hostname now
        //                    auto HOSTNAME_CHARS =
        //                        charset(REG_NAME_NOT_PCT_ENCODED,
        //                        charset('%'));
        //                    if (HOSTNAME_CHARS.contains(hostname)) {
        //                        _host = hostname;
        //                        if (port_or_path_start !=
        //                        str_view_t::npos)
        //                            _data.remove_prefix(port_or_path_start);
        //                        else {
        //                            return;
        //                        }
        //                    }
        //                }
        //
        //                // extracting port
        //                if (port_start != str_view_t::npos) {
        //                    auto port_end =
        //                        _data.find_first_not_of(DIGIT.string_view());
        //                    _port = _data.substr(port_start + 1, port_end);
        //                    _data.remove_prefix(port_end);
        //                }
        //            }
        //        }

        /**
         * parse fragment (it finds fragment_start)
         */
        void parse_fragment() const noexcept {
            if (fragment_start != str_view_t::npos)
                return; // It's already parsed

            auto _data = this->string_view();

            fragment_start = _data.find_first_of('#');
            if (fragment_start == str_view_t::npos) {
                fragment_start = data.size();
            }
        }

        /**
         * parse query; it ensures that query_start and fragment_start are
         * changed
         */
        void parse_query() const noexcept {
            if (query_start != str_view_t::npos)
                return; // there's nothing to do

            parse_fragment();

            auto _data  = this->string_view();
            query_start = _data.substr(0, fragment_start).find_first_of('?');
            if (query_start == str_view_t::npos) {
                query_start = data.size();
            }
        }

        /**
         * parse the host
         */
        void parse_host() const noexcept {
            parse_user_info(); // to get "authority_start" and "user_info_end"
            parse_port();      // to get "port_start" and hopefully "authority_end"
            parse_path();      // to make sure we have "authority_end"
        }

        /**
         * Remove the cache and make sure calling the functions will cause
         * re-parsing the uri.
         */
        inline void unparse() const noexcept {
            scheme_end = authority_start = user_info_end = port_start = authority_end = query_start =
              fragment_start                                                          = str_view_t::npos;
        }

        /**
         * Replace the specified part with the specified replacement
         */
        void replace_value(stl::size_t start, stl::size_t len, str_view_t const& replacement) noexcept {
            static_assert(is_mutable(), "You cannot change a const_uri (string_view is not "
                                        "modifiable)");
            if (start == str_view_t::npos || len == str_view_t::npos || (len == 0 && replacement.empty()))
                return;
            typename traits_type::stringstream_type _data;
            _data << substr(0, start) << replacement << substr(stl::min(data.size(), start + len));
            data = _data.str();
            unparse();
            // TODO: you may want to not unparse everything
        }

      public:
        constexpr basic_uri() noexcept {
            static_assert(is_mutable(), "You can't modify this basic_uri, there's no point in "
                                        "default constructing this class since it can't be changed"
                                        " and thus can't be used in any way.");
        }

        ~basic_uri() noexcept = default;

        static constexpr bool is_mutable() noexcept {
            return Mutable;
        }


        constexpr basic_uri(const char_type* u) noexcept : data(u) {
        }


        /**
         * @brief parse from string, it will trim the spaces for generality too
         * @param string_view URI string
         */
        constexpr basic_uri(storred_str_t const& u) noexcept : data(u) {
        }

        /**
         * If the user uses this
         * @param u
         */
        constexpr basic_uri(storred_str_t&& u) noexcept : data(stl::move(u)) {
        }

        template <bool UMutable = Mutable>
        constexpr basic_uri(basic_uri<TraitsType, UMutable> const& bu) noexcept
          : data{bu.data},
            scheme_end{bu.scheme_end},
            authority_start{bu.authority_start},
            user_info_end{bu.user_info_end},
            port_start{bu.port_start},
            authority_end{bu.authority_end},
            query_start{bu.query_start},
            fragment_start{bu.fragment_start} {
        }

        template <bool UMutable = Mutable>
        constexpr basic_uri(basic_uri<TraitsType, UMutable>&& bu) noexcept
          : data{stl::move(bu.data)},
            scheme_end{stl::move(bu.scheme_end)},
            authority_start{stl::move(bu.authority_start)},
            user_info_end{stl::move(bu.user_info_end)},
            port_start{stl::move(bu.port_start)},
            authority_end{stl::move(bu.authority_end)},
            query_start{stl::move(bu.query_start)},
            fragment_start{stl::move(bu.fragment_start)} {
        }



        // assignment operators
        template <bool UMutable = Mutable>
        basic_uri& operator=(basic_uri<TraitsType, UMutable> const& u) noexcept {
            data            = u.data;
            scheme_end      = u.scheme_end;
            authority_start = u.authority_start;
            user_info_end   = u.user_info_end;
            port_start      = u.port_start;
            authority_end   = u.authority_end;
            query_start     = u.query_start;
            fragment_start  = u.fragment_start;
        }

        template <bool UMutable = Mutable>
        basic_uri& operator=(basic_uri<TraitsType, UMutable>&& u) noexcept {
            data            = stl::move(u.data);
            scheme_end      = stl::move(u.scheme_end);
            authority_start = stl::move(u.authority_start);
            user_info_end   = stl::move(u.user_info_end);
            port_start      = stl::move(u.port_start);
            authority_end   = stl::move(u.authority_end);
            query_start     = stl::move(u.query_start);
            fragment_start  = stl::move(u.fragment_start);
        }

        basic_uri& operator=(storred_str_t const& u) noexcept {
            data = u;
            unparse();
            return *this;
        }

        basic_uri& operator=(storred_str_t&& u) noexcept {
            data = stl::move(u);
            unparse();
            return *this;
        }

        template <bool UMutable = Mutable>
        bool operator==(const basic_uri<TraitsType, UMutable>& other) const noexcept {
            return str() == other.str();
        }

        template <bool UMutable = Mutable>
        bool operator!=(const basic_uri<TraitsType, UMutable>& other) const noexcept {
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
        [[nodiscard]] str_t encoded_uri() noexcept {
            return encode_uri_component<traits_type, ALLOWED_CHARACTERS_IN_URI.size()>(
              str(), ALLOWED_CHARACTERS_IN_URI);
        }

        /**
         * @brief this function is the same as "decodeURI" in javascript
         * @return this function will return an optional<string> object. it will
         * be nullopt when the uri is not valid and has invalid characters
         */
        [[nodiscard]] auto decoded_uri() noexcept {
            return decode_uri_component<traits_type, ALLOWED_CHARACTERS_IN_URI.size()>(
              str(), ALLOWED_CHARACTERS_IN_URI);
        }

        /**
         * @brief check if the specified uri has a scheme or not
         */
        [[nodiscard]] bool has_scheme() const noexcept {
            parse_scheme();
            return scheme_end != data.size() || scheme_end == 0;
        }

        /**
         * @brief scheme
         * @return get scheme
         */
        [[nodiscard]] str_view_t scheme() const noexcept {
            parse_scheme();
            return scheme_end == data.size() ? str_view_t() : substr(0, scheme_end);
        }

        /**
         * @brief set scheme
         * @param _scheme
         * @throws logic_error if uri is const
         */
        basic_uri& scheme(str_view_t __scheme) {
            if (ascii::ends_with(__scheme, ':'))
                __scheme.remove_suffix(1);
            if (!is::scheme<traits_type>(__scheme))
                throw stl::invalid_argument("The specified scheme is not valid");
            parse_scheme();
            if (scheme_end != data.size()) {
                replace_value(0,
                              __scheme.empty() && data.size() > scheme_end + 1 && data[scheme_end] == ':'
                                ? scheme_end + 1
                                : scheme_end,
                              __scheme);
            } else {
                // the URI doesn't have a scheme now, we have to put it in the
                // right place
                auto scheme_colon = __scheme.empty() ? "" : str_t(__scheme) + ':';
                if (authority_start != data.size()) {
                    replace_value(0, 0, scheme_colon + (ascii::starts_with(data, "//") ? "" : "//"));
                } else {
                    // It's a URN (or URN like URI)
                    replace_value(0, 0, scheme_colon);
                }
            }
            return *this;
        }

        /**
         * @brief clear scheme from uri
         */
        auto& clear_scheme() noexcept {
            return scheme({});
        }

        /**
         * @brief checks if the uri has user info or not
         */
        [[nodiscard]] bool has_user_info() const noexcept {
            parse_user_info();
            return user_info_end != data.size() && authority_start != data.size();
        }

        /**
         * @brief get the user info or an empty value
         */
        [[nodiscard]] str_view_t user_info() const noexcept {
            parse_user_info();
            return (user_info_end == data.size() || authority_start == data.size())
                     ? str_view_t()
                     : substr(authority_start, user_info_end - authority_start);
        }

        /**
         * @brief decode user_info and return it as a string
         */
        [[nodiscard]] auto user_info_decoded() const noexcept {
            return decode_uri_component<traits_type>(user_info(), USER_INFO_NOT_PCT_ENCODED);
        }

        /**
         * @brief set the user info if it's possible
         */
        basic_uri& user_info(str_view_t const& info) noexcept {
            parse_user_info();
            auto encoded_info = encode_uri_component<traits_type>(info, USER_INFO_NOT_PCT_ENCODED);
            if (user_info_end == data.size() || authority_start == data.size()) {
                // the URI already has user info, I just have to replace it
                replace_value(authority_start, user_info_end - authority_start, encoded_info);
            } else {
                // I don't know where is it, let's find it

                if (authority_start == data.size()) {
                    // this URI doesn't have authority in it, but I'm going to
                    // insert authority into it because the user is obviously
                    // demanding it; of course after this, the URI won't be a
                    // valid URI until the user sets at lease scheme too.

                    parse_scheme(); // to get "scheme_end"
                    if (scheme_end == data.size()) {
                        // there's no scheme either
                        replace_value(0, 0, str_t("//") + encoded_info + "@");
                    } else {
                        // there's scheme and we have to put it after that
                        replace_value(scheme_end + 1, 0, str_t("//") + encoded_info + "@");
                    }
                } else {
                    // there's authority start but there's no user_info_end
                    replace_value(authority_start, 0, encoded_info + "@");
                }
            }
            return *this;
        }

        /**
         * @brief clears the user info if exists
         */
        auto& clear_user_info() noexcept {
            return user_info({});
        }

        /**
         * @brief return host as an string_view
         * @return string_view
         */
        [[nodiscard]] str_view_t host() const noexcept {
            parse_host();
            if (authority_start == data.size()) {
                // there will not be a host without the authority_start
                return {};
            }

            stl::size_t start, len;

            // you know I could do this in one line of code, but I did this
            // because I don't want you to curse me :)

            // we have authority_start, let's check user_info and port too
            if (user_info_end == data.size()) {
                // there's no user info
                start = authority_start;
            } else {
                // there's a user info
                start = user_info_end;
            }

            if (port_start != data.size()) {
                // but there's a port
                len = port_start - start;
            } else {
                // there's no port either
                if (authority_end != data.size()) {
                    // there's a path
                    len = authority_end - start;
                } else {
                    // there's no path either
                    len = data.size() - 1; // till the end
                }
            }

            return substr(start, len);
        }

        /**
         * @brief returns const_ipv4/const_ipv6/hostname; if the URI doesn't
         * include a valid ip/hostname you'll get an empty string. this method
         * will only return the hostname/ip if it's in the correct format and
         * doesn't include invalid syntax.
         * @return
         */
        [[nodiscard]] stl::variant<ipv4<traits_type>, ipv6<traits_type>, str_view_t>
        host_structured() const noexcept {
            auto _host = host();
            if (is::ipv4(_host))
                return ipv4<traits_type>(_host);
            if (is::ipv6(_host))
                return ipv6<traits_type>(_host);
            return _host;
        }

        /**
         * @brief get the decoded version of hostname/ip of the uri or an empty
         * string if the specified URI does not include a hostname/ip or its
         * hostname has the wrong character encodings.
         * @return string
         */
        [[nodiscard]] auto host_decoded() const noexcept {
            return decode_uri_component<traits_type>(host(), REG_NAME_NOT_PCT_ENCODED);
        }

        /**
         * @brief this method will check if the hostname/ip exists in the uri or
         * not.
         * @return true if it find a hostname/ip in the uri
         */
        [[nodiscard]] bool has_host() const noexcept {
            return !host().empty();
        }

        /**
         * @brief set the hostname/ip in the uri if possible
         */
        basic_uri& host(str_view_t const& new_host) noexcept {
            parse_host();

            // todo: are you sure it can handle punycode as well?
            auto encoded_host = encode_uri_component<traits_type>(new_host, REG_NAME_NOT_PCT_ENCODED);
            if ((!ascii::starts_with(new_host, '[') || !ascii::ends_with(new_host, ']')) &&
                is::ipv6(new_host)) {
                encoded_host = '[' + encoded_host + ']';
            }

            if (authority_start == data.size()) {
                // there's no authority start

                if (encoded_host.empty())
                    return *this; // there's nothing to do here. It's already
                                  // what the user wants

                if (scheme_end == data.size()) {
                    // there's no scheme either, so we just have to add to the
                    // beginning of the string
                    replace_value(0, 0, str_t("//") + encoded_host);
                    return *this;
                } else {
                    // there's a scheme
                    replace_value(scheme_end, 0, str_t("//") + encoded_host);
                    return *this;
                }
            }

            stl::size_t start, finish;

            // you know I could do this in one line of code, but I did this
            // because I don't want you to curse me :)

            // we have authority_start, let's check user_info and port too
            if (user_info_end == data.size()) {
                // there's no user info
                if (scheme_end == data.size()) {
                    start = 0;
                    if (!new_host.empty() && !ascii::starts_with(str_view_t{encoded_host}, "//")) {
                        encoded_host = "//" + encoded_host;
                    }
                } else {
                    start = authority_start;
                }
            } else {
                // there's a user info
                start = user_info_end;
            }

            if (port_start != data.size()) {
                // but there's a port
                finish = port_start;
            } else {
                // there's no port either
                if (authority_end != data.size()) {
                    // there's a path
                    finish = authority_end;
                } else {
                    // there's no path either
                    finish = data.size() - 1; // till the end
                }
            }

            replace_value(start, finish - start, encoded_host);

            return *this;
        }

        /**
         * @brief get the hostname/ipv4/ipv6 from the URI. if the URI doesn't
         * include a hostname/ip or its hostname/ip is not in a valid shape, it
         * will return an empty string
         * @return string/ipv4/ipv6
         * @default empty string
         */
        [[nodiscard]] stl::variant<ipv4<traits_type>, ipv6<traits_type>, str_t>
        host_structured_decoded() const noexcept {
            if (auto _host_structured = host_structured();
                stl::holds_alternative<str_view_t>(_host_structured))
                return decode_uri_component<traits_type>(stl::get<str_view_t>(_host_structured),
                                                         REG_NAME_NOT_PCT_ENCODED);
            else
                return _host_structured;
        }

        /**
         * @brief clear host part from URI
         * @return
         */
        basic_uri& clear_host() noexcept {
            return host({});
        }

        /**
         * Check if the specified host is an IP address or not
         * This method check if the specified host is a IPv4 Address or has a
         * valid syntax for either IPv6 or Future version of IP addresses which
         * also starts with "[" and ends with "]" with an unknown syntax (as of
         * writing this code obv)
         * @return an indication of weather or not the specified Hostname is a
         * valid IP address or not
         */
        [[nodiscard]] bool is_ip() const noexcept {
            auto _host = host();
            return is::ipv4(_host) ||
                   (ascii::starts_with(_host, '[') && ascii::ends_with(_host, ']'));
        }

        /**
         * Get the host and split it by dot separator. TLD (Top Level Domain)
         * will be the last one and Second Level Domain will be the one before
         * that and the rest will be subdomains.
         */
        [[nodiscard]] istl::vector<traits_type, str_t> domains() const noexcept {
            auto _host = host();
            if (_host.empty() || is_ip())
                return {};
            istl::vector<traits_type, str_t> subs;
            for (;;) {
                auto dot = _host.find('.');
                auto sub = _host.substr(0, dot);
                if (sub.empty())
                    break;
                subs.emplace_back(stl::move(sub));
                if (dot == str_view_t::npos)
                    break;
                _host.remove_prefix(dot + 1);
            }
            return subs;
        }

        /**
         * Get the TLD (top level domain) or sometimes called extension
         */
        [[nodiscard]] str_view_t top_level_domain() const noexcept {
            auto _host = host();
            if (_host.empty() || is_ip())
                return {};
            auto dot = _host.find_last_of('.');
            return _host.substr(dot != str_view_t::npos ? dot + 1 : 0);
        }

        /**
         * Set the TLD (Top Level Domain) in the uri
         * @param tld
         * @return
         */
        auto& top_level_domain(str_view_t const& tld) noexcept {
            auto _host = host();
            if (_host.empty()) {
                // I've already written that code. Yay, I'm so happy
                static_cast<void>(host(tld));
            } else if (!is_ip() && !is::ip(tld)) {
                // cannot put an ip address as a tld, user should use set host
                // instead of this method.
                auto dot   = _host.find_last_of('.');
                auto start = dot != str_view_t::npos ? dot + 1 : 0;
                static_cast<void>(host(str_t(_host.substr(0, start)) + str_t(tld)));
            }
            return *this;
        }

        /**
         * Check if the specified uri has a top level domain (TLD) or not
         * @return an indication of weather or not the URI has TLD or not
         */
        [[nodiscard]] bool has_top_level_domain() const noexcept {
            return !top_level_domain().empty();
        }

        /**
         * Get the second level domain out of the host
         */
        [[nodiscard]] str_view_t second_level_domain() const noexcept {
            auto _host = host();
            if (_host.empty() || is_ip())
                return {};
            auto last_dot = _host.find_last_of('.');
            if (last_dot == str_view_t::npos)
                return {};
            auto bef_last_dot = _host.find_last_of('.', last_dot - 1);
            auto start        = bef_last_dot == str_view_t::npos ? 0 : bef_last_dot + 1;
            auto sld          = _host.substr(start, last_dot - start);
            return sld;
        }

        /**
         * Set the second level domain to the specified string.
         * Attention: this method will only work if Top Level Domain already
         * exists
         * @param sld
         */
        basic_uri& second_level_domain(str_view_t const& sld) noexcept {
            auto _host = host();
            if (_host.empty() || is_ip())
                return *this;

            auto last_dot = _host.find_last_of('.');
            if (last_dot == str_view_t::npos) {
                // we have to insert it at the beginning of the host string

                // there's nothing to do it's empty
                if (!sld.empty()) {
                    static_cast<void>(host(str_t(sld) + '.' + str_t(_host)));
                }
            } else {
                auto bef_last_dot = _host.find_last_of('.', last_dot - 1);
                auto start        = bef_last_dot == str_view_t::npos ? 0 : bef_last_dot + 1;
                if (!sld.empty())
                    static_cast<void>(
                      host(str_t(_host.substr(0, start)) + str_t(sld) + str_t(_host.substr(last_dot))));
                else
                    static_cast<void>(host(str_t(_host.substr(last_dot + 1))));
            }
            return *this;
        }

        /**
         * This method will remove the Second Level Domain and also any
         * Sub-Domains if there are any.
         */
        basic_uri& clear_second_level_domain() noexcept {
            return second_level_domain({});
        }

        /**
         * Check if the specified uri has a second level domain or not.
         * @return An indication of weather or not the URI has SLD or not
         */
        [[nodiscard]] bool has_second_level_domain() const noexcept {
            return !second_level_domain().empty();
        }

        /**
         * Get the sub-domain (with sub-sub-...-sub-domain)
         * @return
         */
        [[nodiscard]] str_view_t subdomains() const noexcept {
            auto _host = host();
            if (_host.empty() || is_ip())
                return {};
            auto last_dot = _host.find_last_of('.');
            if (last_dot == str_view_t::npos)
                return {};
            auto bef_last_dot = _host.find_last_of('.', last_dot - 1);
            if (bef_last_dot == str_view_t::npos)
                return {};
            return _host.substr(0, bef_last_dot);
        }

        /**
         * Set the sub-domain part of the host name
         * Attention: this method will only work if Top Level Domain and Second
         * Level Domain already exists
         * @param sds
         */
        auto& subdomains(str_view_t const& sds) noexcept {
            auto _host = host();
            if (_host.empty() || is_ip())
                return *this;
            auto last_dot = _host.find_last_of('.');
            if (last_dot == str_view_t::npos)
                return *this;
            auto bef_last_dot = _host.find_last_of('.', last_dot - 1);
            if (bef_last_dot == str_view_t::npos)
                return *this;
            if (sds.empty()) // special check for when we want to remove the SDS
                bef_last_dot++;
            static_cast<void>(host(str_t(sds) + str_t(_host.substr(bef_last_dot))));
            return *this;
        }

        /**
         * Remove the sub-domains if exists. This method should not have
         * side-effects if there's no sub-domain
         */
        basic_uri& clear_subdomains() noexcept {
            return subdomains({});
        }

        /**
         * Check if the specified uri has at least one sub-domain or not
         * @return an indication of weather or not the URI has subdomains or not
         * TODO: we could be smarter here
         */
        [[nodiscard]] bool has_subdomains() const noexcept {
            return !subdomains().empty();
        }

        /**
         * Get the default port for the specified scheme
         * TODO: make this function a free function
         */
        [[nodiscard]] uint16_t default_port() const noexcept {
            auto _scheme = scheme();
            if (_scheme == "http")
                return 80u;
            if (_scheme == "https")
                return 443u;
            if (_scheme == "ftp")
                return 21u;
            if (_scheme == "ssh")
                return 22u;
            if (_scheme == "telnet")
                return 23u;
            if (_scheme == "ftps")
                return 990u;

            // TODO: add more protocols here
            return 0u;
        }

        /**
         * @brief port number of the uri;
         * @return port number
         * @default 80
         */
        [[nodiscard]] uint16_t port_uint16() const noexcept {
            if (has_port()) {
                return static_cast<uint16_t>(to_uint(port()));
            }
            return default_port();
        }

        /**
         * Get the port in a string_view format; there's no default value.
         * @return string_view
         */
        [[nodiscard]] str_view_t port() const noexcept {
            parse_port();

            // there's no port
            if (port_start == data.size())
                return {};

            // don't worry authority_end will be the end of the string anyway
            return substr(port_start + 1, (authority_end == data.size() ? authority_end - 1 : authority_end) -
                                            (port_start + 1));
        }

        /**
         * @brief checks if the uri has explicitly specified a port number or
         * not
         * @return bool
         */
        [[nodiscard]] bool has_port() const noexcept {
            parse_port();
            return port_start != data.size();
        }

        /**
         * @brief set port
         * @param new_port
         */
        auto& port(uint16_t new_port) noexcept {
            return port(to_str_copy<traits_type>(new_port));
        }

        /**
         * Set new port value
         * @param new_port
         */
        basic_uri& port(str_view_t new_port) noexcept {
            if (ascii::starts_with(new_port, ':'))
                new_port.remove_prefix(1);
            if (!is::digit(new_port))
                throw stl::invalid_argument("The specified port is not valid");
            parse_port();
            if (port_start == data.size()) {
                // there's no port, I have to insert it myself:
                parse_host();

                if (authority_end != data.size()) {
                    // found it at the end of the line
                    replace_value(authority_end, 0, ":" + str_t(new_port));
                } else if (user_info_end != data.size()) {
                    // there's authority and there might be a host
                    replace_value(user_info_end + 1, user_info_end + 1, ":" + str_t(new_port));
                } else if (authority_start != data.size()) {
                    // there's a authority_start at least
                    replace_value(authority_start + 1, 0, ":" + str_t(new_port));
                } else {
                    // there's no authority at all.
                    if (scheme_end == data.size()) {
                        // there's no scheme either
                        replace_value(0, 0, "//:" + str_t(new_port));
                    } else {
                        // there's scheme
                        replace_value(scheme_end + 1, 0, "//:" + str_t(new_port));
                    }
                }
            } else {
                // there's a port and we are going to just replace it
                replace_value(port_start + 1,
                              authority_end == data.size() ? authority_end - 1 : authority_end, new_port);
            }
            return *this;
        }

        /**
         * @brief clear the port number from the uri and defaults to 80
         * @return self
         */
        basic_uri& clear_port() noexcept {
            return port({});
        }

        /**
         * @brief check if the URI has a path or not
         * @return
         */
        [[nodiscard]] bool has_path() const noexcept {
            parse_path();
            return authority_end != data.size();
        }

        /**
         * @brief get path in non-decoded, string format
         * @return
         */
        [[nodiscard]] str_view_t path() const noexcept {
            if (!has_path())
                return {};
            return substr(authority_end, stl::min(query_start, fragment_start) - authority_end);
        }

        /**
         * @brief decoded path as a string
         * @return
         */
        [[nodiscard]] auto path_decoded() const noexcept {
            return decode_uri_component<traits_type>(
              path(), charset(PCHAR_NOT_PCT_ENCODED, charset_t<char_type, 1>('/')));
        }

        /**
         * @brief get the path as the specified type
         * @details this method will returns a vector/list/... of
         * string/string_views
         * this method does not just response to the fact that Container should
         * be an std container, but if string/string_view is presented as a
         * container, it will return the whole path.
         *
         * todo: should we rename path_structured to slugs?
         */
        template <typename Container = istl::vector<traits_type, str_view_t>>
        [[nodiscard]] Container path_structured() const noexcept {
            auto _path = path();
            if (_path.empty())
                return {};
            Container   container;
            stl::size_t slash_start      = 0;
            stl::size_t last_slash_start = 0;
            auto        _path_size       = _path.size();
            if (_path.front() == '/')
                container.emplace_back(); // empty string
            do {
                slash_start = _path.find('/', last_slash_start + 1);
                container.emplace_back(_path.data() + last_slash_start + 1,
                                       stl::min(slash_start, _path_size) - last_slash_start - 1);
                // if (slash_start != str_view_t::npos)
                // _path.remove_prefix(slash_start + 1);
                // else
                // _path.remove_prefix(_path.size());
                last_slash_start = slash_start;
            } while (slash_start != str_view_t::npos);
            return container;
        }

        /**
         * @brief this one will return a container containing decoded strings of
         * the path.
         * @attention do not use string_view or any alternatives for this method
         * as this method should own its data.
         */
        template <typename Container = istl::vector<traits_type, str_t>>
        [[nodiscard]] Container path_structured_decoded() const noexcept {
            auto _slugs = path_structured<Container>();
            for (auto& slug : _slugs) {
                slug = decode_uri_component<traits_type>(slug, PCHAR_NOT_PCT_ENCODED)
                         .value_or((typename Container::value_type)(slug));
            }
            return _slugs;
        }

        /**
         * @brief set path
         */
        template <typename Container, typename = stl::enable_if_t<stl::negation_v<
                                        stl::is_convertible_v<Container::value_type, str_view_t>>>>
        basic_uri& path(const Container& __path) noexcept {
            static_assert(stl::is_convertible_v<typename Container::value_type, str_view_t>,
                          "the specified container is not valid");
            return path(__path.cbegin(), __path.cend());
        }

        /**
         * Set path by begin and end of an iterator
         * @tparam Iter
         * @param _start
         * @param _end
         * @return
         */
        template <typename Iter>
        basic_uri& path(const Iter& _start, const Iter& _end) noexcept {
            using ostream_iterator_t = typename traits_type::ostream_iterator_type;
            typename traits_type::ostringstream_type joined_path;
            // TODO: check if we need std::string here
            copy(_start, _end - 1, ostream_iterator_t(joined_path, "/"));
            joined_path << *stl::prev(_end);
            return path(str_view_t(joined_path.str()));
        }

        /**
         * @brief set the path for the uri
         * @param _path
         * @return
         */
        basic_uri& path(str_view_t const& __path) noexcept {
            parse_path();
            auto _encoded_path = (ascii::starts_with(__path, '/') ? "" : "/") +
                                 encode_uri_component<traits_type>(
                                   __path, charset(PCHAR_NOT_PCT_ENCODED, charset_t<char_type, 1>('/')));

            replace_value(authority_end, query_start - authority_end, _encoded_path);
            return *this;
        }
        /**
         * @brief clear path from the URI
         * @return
         */
        basic_uri& clear_path() noexcept {
            return path(str_view_t{});
        }

        /**
         * @brief checks if the path is an absolute path or relative path
         * @return
         */
        [[nodiscard]] bool is_absolute() const noexcept {
            return ascii::starts_with(path(), '/');
        }

        /**
         * @brief checks if the path is a relative path or an absolute one
         * @return
         */
        [[nodiscard]] bool is_relative() const noexcept {
            return !is_absolute();
        }

        /**
         * @brief checks if the uri has query or not
         * @return
         */
        [[nodiscard]] bool has_query() const noexcept {
            parse_query();
            return query_start != data.size();
        }

        [[nodiscard]] str_view_t query() const noexcept {
            parse_query();
            if (query_start == data.size())
                return {};
            return substr(query_start + 1, fragment_start - query_start - 1);
        }

        /**
         * @brief set query
         * @param _query
         * @return
         */
        basic_uri& query(str_view_t const& __query) {
            if (!is::query(__query))
                throw stl::invalid_argument("The specified string is not a valid query");

            auto encoded_query =
              (ascii::starts_with(__query, '?') ? "" : "?") +
              encode_uri_component<traits_type>(__query, QUERY_OR_FRAGMENT_NOT_PCT_ENCODED);

            parse_query();

            if (query_start != data.size()) {
                // we don't have a query
                if (fragment_start != data.size()) {
                    replace_value(fragment_start, 0, encoded_query);
                } else {
                    parse_path();
                    if (authority_end == data.size()) {
                        // we don't even have authority_end
                        parse_scheme();
                        if (authority_start == data.size()) {
                            // there's no authority_start
                            if (scheme_end == data.size()) {
                                // it's an empty string
                                replace_value(0, 0, "///" + encoded_query);
                            } else {
                                replace_value(scheme_end, 0, "/" + encoded_query);
                            }
                        } else {
                            replace_value(authority_start, 0, "/" + encoded_query);
                        }
                    } else {
                        // we have authority_end
                        if (data[authority_end] == '/') {
                            replace_value(authority_end + 1, 0, encoded_query);
                        } else {
                            replace_value(authority_end + 1, 0, "/" + encoded_query);
                        }
                    }
                }
            } else {
                // we have query
                replace_value(query_start, fragment_start - query_start, encoded_query);
            }
            return *this;
        }

        /**
         *
         * @param queries
         * @return
         */
        template <typename Map>
        basic_uri& query(Map const& _queries) noexcept {
            static_assert(stl::is_convertible_v<typename Map::key_type, str_view_t> &&
                            stl::is_convertible_v<typename Map::mapped_type, str_view_t>,
                          "The specified map is not valid");
            str_t _query_data;
            bool  first = true;
            for (auto it = _queries.cbegin(); it != _queries.cend(); it++) {
                auto name  = encode_uri_component<traits_type>(it->first, QUERY_OR_FRAGMENT_NOT_PCT_ENCODED);
                auto value = encode_uri_component<traits_type>(it->second, QUERY_OR_FRAGMENT_NOT_PCT_ENCODED);
                if (name.empty()) // when name is empty, we just don't care
                    continue;
                _query_data =
                  name + (value.empty() ? "" : ("=" + value)) + (stl::next(it) != _queries.cend() ? "&" : "");
            }
            query(_query_data);
            return *this;
        }

        /**
         * @brief clear the query section of the URI
         * @return
         */
        basic_uri& clear_query() noexcept {
            return query({});
        }

        /**
         * Get the query in a decoded string format
         * @return optional<string>
         */
        [[nodiscard]] auto query_decoded() const noexcept {
            return decode_uri_component<traits_type>(query(), QUERY_OR_FRAGMENT_NOT_PCT_ENCODED);
        }

        /**
         * Get the query in as a map<string, string>
         * It's also in a decoded format
         * @return unordered_map<string, string>
         */
        [[nodiscard]] stl::map<traits_type, str_t, str_t> query_structured() const noexcept {
            stl::map<traits_type, str_t, str_t> q_structured;
            stl::size_t                         last_and_sep = 0;
            auto                                _query       = query();
            do {
                auto and_sep = _query.find('&', last_and_sep); // find the delimiter
                auto eq_sep  = _query.find("=", last_and_sep, and_sep - last_and_sep);
                auto name    = _query.substr(last_and_sep + 1, stl::min(eq_sep, and_sep));
                last_and_sep = and_sep;
                if (name.empty()) // a name should not be empty
                    continue;
                str_t value;
                if (and_sep != str_view_t::npos) { // we have a value as well
                    value = _query.substr(eq_sep + 1, and_sep);
                }
                auto d_name = decode_uri_component<traits_type>(name, QUERY_OR_FRAGMENT_NOT_PCT_ENCODED);
                if (d_name)
                    q_structured[d_name.value()] =
                      decode_uri_component<traits_type>(value, QUERY_OR_FRAGMENT_NOT_PCT_ENCODED)
                        .value_or("");
            } while (last_and_sep != str_view_t::npos);
            return q_structured;
        }

        /**
         * @brief checks if the uri path is normalized or not (contains relative
         * . or .. paths)
         * @return
         */
        [[nodiscard]] bool is_normalized() const noexcept {
            auto __path = path_structured();
            return __path.cend() != stl::find_if(__path.cbegin(), __path.cend(), [](auto const& p) {
                       return p == "." || p == "..";
                   });
        }

        /**
         * @details This method applies the "remove_dot_segments" routine talked
         * about in RFC 3986 (https://tools.ietf.org/html/rfc3986) to the path
         * segments of the URI, in order to normalize the path
         * (apply and remove "." and ".." segments).
         */
        basic_uri& normalize_path() noexcept {
            // TODO
            return *this;
        }

        /**
         * @brief get fragment
         */
        [[nodiscard]] str_view_t fragment() const noexcept {
            parse_fragment();
            return substr(fragment_start + 1);
        }

        /**
         * @brief an indication of whether the URI has fragment or not.
         * @return
         */
        [[nodiscard]] bool has_fragment() const noexcept {
            parse_fragment();
            return fragment_start != data.size();
        }

        /**
         * @brief clear the fragment part of the uri
         * @return
         */
        basic_uri& clear_fragment() noexcept {
            return fragment({});
        }

        /**
         * @brief checks if the URI is a relative reference
         * @return
         */
        [[nodiscard]] bool is_relative_reference() const noexcept {
            return !has_scheme();
        }

        /**
         * This method returns an indication of whether or not the URI includes
         * any element that is part of the authority URI.
         * @return bool
         */
        [[nodiscard]] bool has_authority() const noexcept {
            return has_host() || has_user_info() || has_port();
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
        [[nodiscard]] basic_uri resolve(const basic_uri& relative_uri) const noexcept {
            // Resolve the reference by following the algorithm
            // from section 5.2.2 in
            // RFC 3986 (https://tools.ietf.org/html/rfc3986).
            basic_uri target;
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
                    target.query(relative_uri.query());
                    target.normalize_path();
                } else {
                    target.host(host());
                    target.user_info(user_info());
                    target.port(port());
                    if (!relative_uri.has_path()) {
                        target.path(path());
                        if (relative_uri.has_query()) {
                            target.query(relative_uri.query());
                        } else {
                            target.query(query());
                        }
                    } else {
                        target.query(relative_uri._query);
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
            return has_host();
        }

        /**
         * Check if the specified string is a valid URI or not
         */
        [[nodiscard]] bool is_valid() const noexcept {
            return has_scheme() || has_authority() || has_path() || has_fragment();
        }

        /**
         * Check if the user info has a username in it or not
         * @return bool
         */
        [[nodiscard]] bool has_username() const noexcept {
            return user_info_end != data.size();
        }

        /**
         * Get the username in the user info if it exists or otherwise an empty
         * string view
         * @return
         */
        [[nodiscard]] str_view_t username() const noexcept {
            auto _userinfo = user_info();
            if (auto colon = _userinfo.find(':'); colon != str_view_t::npos)
                _userinfo.remove_suffix(_userinfo.size() - colon);
            return _userinfo;
        }

        /**
         * Get the decoded version of the username if it exists
         * @return
         */
        [[nodiscard]] str_t username_decoded() const noexcept {
            return decode_uri_component<traits_type>(username(), USER_INFO_NOT_PCT_ENCODED);
        }

        /**
         * An indication of whether or not the user info has a password
         * @return
         */
        [[nodiscard]] bool has_password() const noexcept {
            return user_info().find(':') != str_view_t::npos;
        }

        /**
         * The password in the user info
         * @return
         */
        [[nodiscard]] str_view_t password() const noexcept {
            auto _user_info = user_info();
            if (auto found = _user_info.find(':'); found != str_view_t::npos) {
                return _user_info.substr(found + 1);
            }
            return {};
        }

        /**
         * The decoded version of the password
         * @return
         */
        [[nodiscard]] str_t password_decoded() const noexcept {
            return decode_uri_component<traits_type>(password(), USER_INFO_NOT_PCT_ENCODED);
        }
    };

    template <typename CharT = char>
    basic_uri(stl::enable_if_t<stl::is_integral_v<CharT>, void> const* const)
      -> basic_uri<basic_std_traits<CharT>, false>;
    // basic_uri(const char[])->basic_uri<std::string_view>;

    template <typename CharT = char>
    basic_uri(stl::basic_string_view<CharT>) -> basic_uri<basic_std_traits<CharT>, false>;

    template <typename CharT = char>
    basic_uri(stl::basic_string<CharT>) -> basic_uri<basic_std_traits<CharT>, true>;

    using const_uri = basic_uri<std_traits, false>;
    using uri       = basic_uri<std_traits, true>;


    template <Traits TraitsType, bool Mutable1, bool Mutable2>
    bool operator==(basic_uri<TraitsType, Mutable1> const& one,
                    basic_uri<TraitsType, Mutable2> const& two) noexcept {
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
    [[nodiscard]] bool equal_path(basic_uri<TraitsType, Mutable1> const& p1,
                                  basic_uri<TraitsType, Mutable2> const& p2) noexcept {
        auto _p1 = p1.path_structured_decoded();
        auto _p2 = p2.path_structured_decoded();
        auto it2 = _p2.cbegin();
        auto it1 = _p1.cbegin();
        while (it1 != _p1.cend() && it2 != _p2.cend()) {
            if (*it1 != *it2) {
                if (*it1 == "") {
                    ++it1;
                    continue;
                } else if (*it2 == "") {
                    ++it2;
                    continue;
                }

                return false;
            }
            ++it1;
            ++it2;
        }

        if (it1 != _p1.cend()) {
            if (!stl::all_of(it1, _p1.cend(), [](auto const& a) {
                    return a == "";
                }))
                return false;
        }
        if (it1 != _p2.cend()) {
            if (!stl::all_of(it2, _p2.cend(), [](auto const& a) {
                    return a == "";
                }))
                return false;
        }
        return true;
    }

    [[nodiscard]] inline auto equal_path(stl::string_view const& p1, stl::string_view const& p2) noexcept {
        return p1 == p2 || equal_path<std_traits, false, false>(const_uri{p1}, const_uri{p2});
    }

    template <Traits TraitsType, bool Mutable>
    [[nodiscard]] inline auto equal_path(basic_uri<TraitsType, Mutable> const&        p1,
                                         typename TraitsType::string_view_type const& p2) noexcept {
        return p1 == p2 || equal_path<TraitsType, false>(p1, const_uri{p2});
    }

    template <Traits TraitsType, bool Mutable>
    [[nodiscard]] inline auto equal_path(typename TraitsType::string_view_type const& p1,
                                         basic_uri<TraitsType, Mutable> const&        p2) noexcept {
        return p2 == p1 || equal_path<TraitsType, false>(const_uri{p1}, p2);
    }

} // namespace webpp

#endif // WEBPP_UTILS_URI_H
