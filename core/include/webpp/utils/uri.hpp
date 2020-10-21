#ifndef WEBPP_UTILS_URI_H
#define WEBPP_UTILS_URI_H

#include "../std/map.hpp"
#include "../std/optional.hpp"
#include "../std/vector.hpp"
#include "../strings/charset.hpp"
#include "../strings/to_case.hpp"
#include "../traits/traits_concepts.hpp"
#include "../validators/validators.hpp"
#include "./casts.hpp"
#include "./ipv4.hpp"
#include "./ipv6.hpp"
#include "./allocators.hpp"

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

    enum struct uri_encoding_policy { allowed_chars, disallowed_chars };

    /**
     * @brief this function will decode parts of uri
     * @details this function is almost the same as "decodeURIComponent" in javascript
     */
    template <uri_encoding_policy Policy = uri_encoding_policy::allowed_chars, stl::size_t N>
    [[nodiscard]] bool
    decode_uri_component(istl::StringViewifiable auto&& encoded_str, istl::String auto& output,
                         charset<istl::char_type_of<decltype(encoded_str)>, N>& chars) noexcept {
        using char_type          = istl::char_type_of<stl::remove_cvref_t<decltype(encoded_str)>>;
        stl::size_t digits_left  = 2;
        char_type   decoded_char = 0;
        bool        decoding     = false;
        for (const auto c : istl::string_viewify(encoded_str)) {
            if (decoding && digits_left) {
                decoded_char <<= 4;
                if (c >= '0' && c <= '9') { // DIGITS
                    decoded_char += c - '0';
                } else if (c >= 'A' && c <= 'F') { // UPPER_HEX
                    decoded_char += c - 'A' + 10;
                } else if (c >= 'a' && c <= 'f') { // LOWER_HEX
                    decoded_char += c - 'a' + 10;
                } else {
                    return false;
                }
                --digits_left;

                if (digits_left == 0) {
                    decoding = false;
                    output += static_cast<char_type>(decoded_char);
                }
            } else if (c == '%') {
                decoding = true;

                // resetting:
                digits_left  = 2;
                decoded_char = 0;
            } else {
                if constexpr (uri_encoding_policy::allowed_chars == Policy) {
                    if (!chars.contains(c))
                        return false; // bad chars
                } else {
                    if (chars.contains(c))
                        return false; // bad chars
                }
                output += c;
            }
        }
        return true;
    }

    /**
     * This method encodes the given URI element.
     * What we are calling a "URI element" is any part of the URI
     * which is a sequence of characters that:
     * - may be percent-encoded
     * - if not percent-encoded, are in a restricted set of characters
     *
     * @param[in] src
     *     This is the element to encode.
     *
     * @param[in] allowed_chars
     *     This is the set of characters that do not need to
     *     be percent-encoded.
     *
     * @return
     *     The encoded element is returned.
     *
     *
     * @details this function is almost the same as "encodeURIComponent" in javascript
     */
    template <uri_encoding_policy Policy = uri_encoding_policy::allowed_chars, stl::size_t N>
    static void encode_uri_component(istl::StringViewifiable auto&& src, istl::String auto& output,
                                     charset<istl::char_type_of<decltype(src)>, N> const& chars) {
        using char_type   = istl::char_type_of<decltype(src)>;
        using uchar_type  = stl::make_unsigned_t<char_type>;
        using string_type = stl::remove_cvref_t<decltype(output)>;
        static_assert(stl::is_same_v<char_type, typename string_type::value_type>,
                      "The specified string do not have the same char type.");
        static constexpr auto make_hex_digit = [](auto value) constexpr noexcept->char_type {
            if (value < 10) {
                return static_cast<char_type>(value + '0');
            } else {
                return static_cast<char_type>(value - 10 + 'A');
            }
        };

        const auto input      = istl::string_viewify(src);
        const auto input_size = input.size();
        auto       it         = input.data();
        const auto input_end  = it + input_size;
        output.reserve(output.size() + input_size * 1.5); // 1.5 is by chance
        for (; it != input_end; ++it) {
            bool need_conversion;
            if constexpr (uri_encoding_policy::allowed_chars == Policy) {
                need_conversion = chars.contains(*it);
            } else {
                need_conversion = !chars.contains(*it);
            }
            if (need_conversion) {
                output += *it;
            } else {
                output += '%';
                output += make_hex_digit(static_cast<uchar_type>(*it) >> 4u);
                output += make_hex_digit(static_cast<uchar_type>(*it) & 0x0Fu);
            }
        }
        output.shrink_to_fit();
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
      struct basic_uri : public allocator_holder<typename TraitsType::template allocator<typename TraitsType::char_type>> {
      public:
        using traits_type = TraitsType;
        using char_type   = typename traits_type::char_type;
        using alloc_holder_type = allocator_holder<typename TraitsType::template allocator<typename TraitsType::char_type>>;
        using allocator_type = typename alloc_holder_type::allocator_type;

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
                  charset<char_type, 20>{';', ',', '/', '?', ':', '@', '&', '=', '+', '$',
                                           '-', '_', '.', '!', '~', '*', '\'', '(', ')', '#'});
        /**
         * This is the character set corresponds to the second part
         * of the "scheme" syntax
         * specified in RFC 3986 (https://tools.ietf.org/html/rfc3986).
         */
        static constexpr auto SCHEME_NOT_FIRST =
          charset(ALPHA<char_type>, DIGIT<char_type>, charset<char_type, 3>{'+', '-', '.'});

        /**
         * This is the character set corresponds to the "unreserved" syntax
         * specified in RFC 3986 (https://tools.ietf.org/html/rfc3986).
         */
        static constexpr auto UNRESERVED =
          charset(ALPHA<char_type>, DIGIT<char_type>, charset<char_type, 4>{'-', '.', '_', '~'});

        /**
         * This is the character set corresponds to the "sub-delims" syntax
         * specified in RFC 3986 (https://tools.ietf.org/html/rfc3986).
         */
        static constexpr auto SUB_DELIMS =
          charset<char_type, 11>('!', '$', '&', '\'', '(', ')', '*', '+', ',', ';', '=');

        /**
         * This is the character set corresponds to the "userinfo" syntax
         * specified in RFC 3986 (https://tools.ietf.org/html/rfc3986),
         * leaving out "pct-encoded".
         */
        static constexpr auto USER_INFO_NOT_PCT_ENCODED =
          charset(UNRESERVED, SUB_DELIMS, charset<char_type, 1>{':'});

        /**
         * This is the character set corresponds to the last part of
         * the "IPvFuture" syntax
         * specified in RFC 3986 (https://tools.ietf.org/html/rfc3986).
         */
        static constexpr auto IPV_FUTURE_LAST_PART =
          charset(UNRESERVED, SUB_DELIMS, charset<char_type, 1>{':'});

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
          charset(UNRESERVED, SUB_DELIMS, webpp::charset<char_type, 2>{':', '@'});

        /**
         * This is the character set corresponds to the "query" syntax
         * and the "fragment" syntax
         * specified in RFC 3986 (https://tools.ietf.org/html/rfc3986),
         * leaving out "pct-encoded".
         */
        static constexpr auto QUERY_OR_FRAGMENT_NOT_PCT_ENCODED =
          charset(PCHAR_NOT_PCT_ENCODED, charset<char_type, 2>{'/', '?'});

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
                auto m_scheme = _data.substr(0, colon);
                if (ALPHA<char_type>.contains(m_scheme[0]) &&
                    m_scheme.substr(1).find_first_not_of(SCHEME_NOT_FIRST.string_view())) {
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
                if (!ascii::is::digit(str_view)) {
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
        constexpr basic_uri(allocator_type const& alloc = allocator_type{}) noexcept : alloc_holder_type{alloc} {
            static_assert(is_mutable(), "You can't modify this basic_uri, there's no point in "
                                        "default constructing this class since it can't be changed"
                                        " and thus can't be used in any way.");
        }

        ~basic_uri() noexcept = default;

        static constexpr bool is_mutable() noexcept {
            return Mutable;
        }


        constexpr basic_uri(const char_type* u) noexcept : data(u) {}


        /**
         * @brief parse from string, it will trim the spaces for generality too
         * @param string_view URI string
         */
        constexpr basic_uri(storred_str_t const& u) noexcept : data(u) {}

        /**
         * If the user uses this
         * @param u
         */
        constexpr basic_uri(storred_str_t&& u) noexcept : data(stl::move(u)) {}

        template <bool UMutable = Mutable>
        constexpr basic_uri(basic_uri<TraitsType, UMutable> const& bu) noexcept
          : data{bu.data},
            scheme_end{bu.scheme_end},
            authority_start{bu.authority_start},
            user_info_end{bu.user_info_end},
            port_start{bu.port_start},
            authority_end{bu.authority_end},
            query_start{bu.query_start},
            fragment_start{bu.fragment_start} {}

        template <bool UMutable = Mutable>
        constexpr basic_uri(basic_uri<TraitsType, UMutable>&& bu) noexcept
          : data{stl::move(bu.data)},
            scheme_end{stl::move(bu.scheme_end)},
            authority_start{stl::move(bu.authority_start)},
            user_info_end{stl::move(bu.user_info_end)},
            port_start{stl::move(bu.port_start)},
            authority_end{stl::move(bu.authority_end)},
            query_start{stl::move(bu.query_start)},
            fragment_start{stl::move(bu.fragment_start)} {}



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
        basic_uri& scheme(str_view_t m_scheme) {
            if (ascii::ends_with(m_scheme, ':'))
                m_scheme.remove_suffix(1);
            if (!is::scheme<traits_type>(m_scheme))
                throw stl::invalid_argument("The specified scheme is not valid");
            parse_scheme();
            if (scheme_end != data.size()) {
                replace_value(0,
                              m_scheme.empty() && data.size() > scheme_end + 1 && data[scheme_end] == ':'
                                ? scheme_end + 1
                                : scheme_end,
                              m_scheme);
            } else {
                // the URI doesn't have a scheme now, we have to put it in the
                // right place
                auto scheme_colon = m_scheme.empty() ? "" : str_t(m_scheme) + ':';
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
         * @brief this method will check if the hostname/ip exists in the uri or not.
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
            return is::ipv4(_host) || (ascii::starts_with(_host, '[') && ascii::ends_with(_host, ']'));
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
            if (!ascii::is::digit(new_port))
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

        struct path_type {
            basic_uri& self;

            static constexpr auto LEGAL_PATH_CHARS = charset(PCHAR_NOT_PCT_ENCODED, charset<char_type, 1>('/'));

            basic_uri& operator=(istl::StringViewifiable auto&& new_path) {
                set(stl::forward<decltype(new_path)>(new_path));
                return self;
            }



            /**
             * @brief check if the URI has a path or not
             * @return bool true if the URI doesn't have a path
             */
            [[nodiscard]] bool empty() const noexcept {
                self.parse_path();
                return self.authority_end != self.data.size();
            }

            /**
             * @brief get path in non-decoded, string format
             * @return
             */
            [[nodiscard]] str_view_t raw() const noexcept {
                if (!empty())
                    return {};
                return self.substr(self.authority_end, stl::min(self.query_start, self.fragment_start) - self.authority_end);
            }

            /**
             * @brief decoded path as a string
             * @return std::optional<string> the string might not have the right format
             */
            [[nodiscard]] stl::optional<str_t> decoded() const noexcept {
                str_t res{self.get_allocator()};
                if (!decode_uri_component(raw(), res, LEGAL_PATH_CHARS)) {
                    return stl::nullopt;
                }
                return res;
            }

            /**
             * Get the decoded slugs with the specified container type
             * We know how to get the allocator, don't worry.
             * The specified container's string type should be a string and not a string_view
             */
            template <typename Container = istl::vector<traits_type, str_t>>
            [[nodiscard]] Container slugs() const noexcept {
            	Container container(self.get_allocator());
            	extract_slugs_to<Container>(container);
            	return container;
            }

            /**
             * Get the non-decoded slugs.
             * You can use string_view as the underlying string type of the container since we don't
             * decode the string. As long as the class has access to the string_view, this method has too.
             */
            template <typename Container = istl::vector<traits_type, str_view_t>>
            [[nodiscard]] Container raw_slugs() const noexcept {
            	Container container(self.get_allocator());
            	extract_raw_slugs_to<Container>(container);
            	return container;
            }

            /**
             * @brief get the path as the specified type
             * @details this method will returns a vector/list/... of string/string_views
             * this method does not just response to the fact that Container should
             * be an std container, but if string/string_view is presented as a
             * container, it will return the whole path.
             */
            template <typename Container = istl::vector<traits_type, str_view_t>>
            basic_uri& extract_raw_slugs_to(Container &container) const noexcept {
                auto _path = raw();
                if (_path.empty())
                    return self;
                stl::size_t slash_start      = 0;
                stl::size_t last_slash_start = 0;
                auto        _path_size       = _path.size();
                if (_path.front() == '/')
                    container.emplace_back(); // empty string
                do {
                    slash_start = _path.find('/', self.last_slash_start + 1);
                    container.emplace_back(_path.data() + self.last_slash_start + 1,
                                           stl::min(self.slash_start, _path_size) - self.last_slash_start - 1);
                    // if (slash_start != str_view_t::npos)
                    // _path.remove_prefix(slash_start + 1);
                    // else
                    // _path.remove_prefix(_path.size());
                    self.last_slash_start = self.slash_start;
                } while (self.slash_start != str_view_t::npos);
                return self;
            }

            /**
             * @brief this one will return a container containing decoded strings of the path.
             * @attention do not use string_view or any alternatives for this method
             * as this method should own its data.
             */
            template <typename Container = istl::vector<traits_type, str_t>>
            [[nodiscard]] bool extract_slugs_to(Container &container) const noexcept {
                using vec_str_t = typename Container::value_type;
                for (auto& slug : container) {
                    vec_str_t tmp(container.get_allocator());
                    if (!decode_uri_component(slug, tmp, PCHAR_NOT_PCT_ENCODED)) {
                        return false;
                    }
                    slug.swap(tmp);
                }
                return true;
            }

            /**
             * @brief set path
             */
            template <typename Container>
            basic_uri& set_from(const Container& m_path) noexcept {
                return set(m_path.begin(), m_path.end());
            }

            /**
             * Set path by begin and end of an iterator
             */
            template <typename Iter>
            basic_uri& set_from(const Iter& _start, const Iter& _end) noexcept {
                const auto almost_end = stl::prev(_end);
                str_t new_path{self.get_allocator()};
                for (auto it = _start; it != almost_end; ++it) {
                    encode_uri_component(*it, new_path, PCHAR_NOT_PCT_ENCODED);
                    new_path.append('/');
                }
                // append the last slug
                encode_uri_component(*almost_end, new_path, PCHAR_NOT_PCT_ENCODED);
                return set(stl::move(new_path));
            }

            /**
             * @brief set the path for the uri
             * @param _path
             * @return
             */
            basic_uri& set(istl::StringViewifiable auto&& m_path) noexcept {
                self.parse_path();
                str_t str(self.get_allocator());
                encode_uri_component(m_path, str, charset(PCHAR_NOT_PCT_ENCODED, charset<char_type, 1>('/')));
                auto _encoded_path = (ascii::starts_with(m_path, '/') ? "" : "/") + str;
                self.replace_value(self.authority_end, self.query_start - self.authority_end, _encoded_path);
                return *this;
            }

            /**
             * @brief clear path from the URI
             */
            basic_uri& clear() noexcept {
                return set("");
            }

            /**
             * @brief checks if the path is an absolute path or relative path
             * @return bool
             */
            [[nodiscard]] bool is_absolute() const noexcept {
                return ascii::starts_with(raw(), '/');
            }

            /**
             * @brief checks if the path is a relative path or an absolute one
             * @return bool
             */
            [[nodiscard]] bool is_relative() const noexcept {
                return !is_absolute();
            }


        };
        path_type path{*this};





        /**
         * This is designed to separate the queries' methods from other uri methods
         */
        struct queries_type {
            basic_uri& self;

            queries_type& operator=(istl::StringViewifiable auto&& str) {
                set(stl::forward<decltype(str)>(str));
                return *this;
            }

            /**
             * @brief checks if the uri has query or not
             */
            [[nodiscard]] bool empty() const noexcept {
                self.parse_query();
                return self.query_start != self.data.size();
            }

            [[nodiscard]] str_view_t raw() const noexcept {
                self.parse_query();
                if (self.query_start == self.data.size())
                    return {};
                return self.substr(self.query_start + 1, self.fragment_start - self.query_start - 1);
            }

            /**
             * Get the query in a decoded string format
             */
            [[nodiscard]] stl::optional<str_t> decoded_string() const noexcept {
                str_t d_queries{self.get_allocator()};
                if (!decode_uri_component(raw(), d_queries, QUERY_OR_FRAGMENT_NOT_PCT_ENCODED)) {
                    return stl::nullopt;
                }
                return d_queries;
            }

            template <typename MapType = istl::map<traits_type, str_t, str_t>>
            [[nodiscard]] auto decoded() const noexcept {
                MapType res(alloc_holder_type::get_allocator());
                extract_queries_to(res);
                return res;
            }

            /**
             * Set queries
             */
            basic_uri& set(istl::StringViewifiable auto&& _queries) {
                auto m_query = istl::string_viewify(stl::forward<decltype(_queries)>(_queries));
                if (!is::query(m_query))
                    throw stl::invalid_argument("The specified string is not a valid query");

                str_t encoded_query(alloc_holder_type::get_allocator());
                if (ascii::starts_with(m_query, '?')) {
                    encoded_query.append('?');
                }
                encode_uri_component(m_query, encoded_query, QUERY_OR_FRAGMENT_NOT_PCT_ENCODED);

                self.parse_query();

                if (self.query_start != self.data.size()) {
                    // we don't have a query
                    if (self.fragment_start != self.data.size()) {
                        self.replace_value(self.fragment_start, 0, self.encoded_query);
                    } else {
                        self.parse_path();
                        if (self.authority_end == self.data.size()) {
                            // we don't even have authority_end
                            self.parse_scheme();
                            if (self.authority_start == self.data.size()) {
                                // there's no authority_start
                                if (self.scheme_end == self.data.size()) {
                                    // it's an empty string
                                    self.replace_value(0, 0, "///" + self.encoded_query);
                                } else {
                                    self.replace_value(self.scheme_end, 0, "/" + self.encoded_query);
                                }
                            } else {
                                self.replace_value(self.authority_start, 0, "/" + self.encoded_query);
                            }
                        } else {
                            // we have authority_end
                            if (self.data[self.authority_end] == '/') {
                                self.replace_value(self.authority_end + 1, 0, self.encoded_query);
                            } else {
                                self.replace_value(self.authority_end + 1, 0, "/" + self.encoded_query);
                            }
                        }
                    }
                } else {
                    // we have query
                    self.replace_value(self.query_start, self.fragment_start - self.query_start, self.encoded_query);
                }
                return self;
            }

            /**
             * Append queries from a container like std::map or std::multimap
             */
            template <typename MapIter>
            basic_uri& set_from(MapIter const& _queries_begin, MapIter const& _queries_end) {
                static_assert(is_mutable(), "You can't use this method on a non-modifiable uri struct.");
                using map_key_type = typename MapIter::first_type;
                using map_value_type = typename MapIter::second_type;
                static_assert(istl::StringViewifiable<map_key_type> && istl::StringViewifiable<map_value_type>,
                              "The specified map is not valid");

                stl::size_t reserved_size = 0;
                for (auto it = _queries_begin; it != _queries_end; ++it)
                    reserved_size += it->first->size() + it->second->size() + 2;
                str_t _query_data(alloc_holder_type::get_allocator());
                _query_data.reserve(reserved_size);

                for (auto it = _queries_begin; it != _queries_end; ++it) {
                    str_t name(alloc_holder_type::get_allocator());
                    str_t value(alloc_holder_type::get_allocator());
                    encode_uri_component(it->first, name, QUERY_OR_FRAGMENT_NOT_PCT_ENCODED);
                    encode_uri_component(it->second, value, QUERY_OR_FRAGMENT_NOT_PCT_ENCODED);
                    if (name.empty()) // when name is empty, we just don't care
                        continue;
                    _query_data.append(name);
                    if (!value.empty()) {
                        _query_data.append('=');
                        _query_data.append(value);
                    }
                    if (stl::next(it) != _queries_end) {
                        _query_data.append('&');
                    }
                }
                set(stl::move(_query_data));
                return self;
            }

            template <typename MapType>
            basic_uri& set_from(MapType const& _queries) {
                return set_from(_queries.begin(), _queries.end());
            }

            /**
             * @brief clear the query section of the URI
             */
            basic_uri& clear() noexcept {
                return set("");
            }

            /**
             * Get the query in as a map<string, string>
             * It's also in a decoded format
             */
            template <typename MapType = istl::map<traits_type, str_t, str_t>>
            basic_uri& extract_to(MapType &q_structured) const noexcept {
                using map_key_type = typename MapType::key_value;
                using map_value_type = typename MapType::mapped_type;
                static_assert(istl::String<map_key_type>,
                              "The specified container can't hold the query keys.");
                static_assert(istl::String<map_value_type>,
                              "The specified container can't hold the query values.");
                stl::size_t                         last_and_sep = 0;
                auto                                _query       = raw();
                do {
                    auto and_sep = _query.find('&', last_and_sep); // find the delimiter
                    auto eq_sep  = _query.find("=", last_and_sep, and_sep - last_and_sep);
                    auto name    = _query.substr(last_and_sep + 1, stl::min(eq_sep, and_sep));
                    last_and_sep = and_sep;
                    if (name.empty()) // a name should not be empty
                        continue;
                    str_t d_value(alloc_holder_type::get_allocator());
                    str_t d_name(alloc_holder_type::get_allocator());
                    if (and_sep != str_view_t::npos) { // we have a value as well
                        d_value = _query.substr(eq_sep + 1, and_sep);
                    }
                    if (!decode_uri_component(name, d_name, QUERY_OR_FRAGMENT_NOT_PCT_ENCODED)) {
                        d_name = name; // just put the non-decoded string there
                    }
                    if (!d_name.empty()) {
                        map_value_type new_value(q_structured.get_allocator());
                        if (decode_uri_component(d_value, new_value, QUERY_OR_FRAGMENT_NOT_PCT_ENCODED)) {
                            q_structured[d_name] = stl::move(new_value);
                        } else {
                            q_structured[d_name] = stl::move(d_value); // just put the non-decoded value here
                        }
                    }
                } while (last_and_sep != str_view_t::npos);
                return *this;
            }

            basic_uri& append(istl::StringViewifiable auto&& key, istl::StringViewifiable auto &&value) noexcept {
                // todo
            }

        };

        [[no_unique_address]] queries_type queries{*this};

        /**
         * @brief checks if the uri path is normalized or not (contains relative
         * . or .. paths)
         * @return
         */
        [[nodiscard]] bool is_normalized() const noexcept {
            auto m_path = path_structured();
            return m_path.cend() != stl::find_if(m_path.cbegin(), m_path.cend(), [](auto const& p) {
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
                    target.set_queries(relative_uri.queries_raw());
                    target.normalize_path();
                } else {
                    target.host(host());
                    target.user_info(user_info());
                    target.port(port());
                    if (!relative_uri.has_path()) {
                        target.path(path());
                        if (relative_uri.has_queries()) {
                            target.set_queries(relative_uri.queries_raw());
                        } else {
                            target.set_queries(queries_raw());
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

    template <istl::CharType CharT = char>
    basic_uri(CharT const*) -> basic_uri<basic_std_traits<CharT>, false>;
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
