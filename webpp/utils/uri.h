#ifndef WEBPP_URI_H
#define WEBPP_URI_H

#include "../std/string_view.h"
#include "../validators/validators.h"
#include "charset.h"
#include "ip.h"
#include "strings.h"
#include <functional>
#include <memory>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <variant>
#include <vector>

namespace webpp {

    /**
     * @brief this function will decode parts of uri
     * @details this function is almost the same as "decodeURIComponent" in
     * javascript
     */
    template <std::size_t N>
    std::optional<std::string>
    decode_uri_component(std::string_view const& encoded_str,
                         charset_t<N> const& allowed_chars) noexcept {

        int digits_left = 2;
        unsigned char decoded_char = 0;
        bool decoding = false;
        std::string res;
        for (const auto c : encoded_str) {
            if (digits_left) {
                decoded_char <<= 4;
                if (c >= '0' && c <= '9') { // DIGITS
                    decoded_char += c - '0';
                } else if (c >= 'A' && c <= 'F') { // UPPER_HEX
                    decoded_char += c - 'A' + 10;
                } else if (c >= 'a' && c <= 'f') { // LOWER_HEX
                    decoded_char += c - 'a' + 10;
                } else {
                    return std::nullopt; // not encoded well
                }
                --digits_left;

                if (digits_left == 0) {
                    decoding = false;
                    res.push_back(decoded_char);
                }
            } else if (c == '%') {
                decoding = true;

                // resetting:
                digits_left = 2;
                decoded_char = 0;
            } else {
                if (!allowed_chars.contains(c))
                    return std::nullopt; // bad chars
                res.push_back(c);
            }
        }
        return std::move(res);
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
    template <std::size_t N>
    std::string
    encode_uri_component(const std::string_view& element,
                         const webpp::charset_t<N>& allowedCharacters) {
        constexpr auto make_hex_digit = [](unsigned int value) {
            if (value < 10) {
                return static_cast<char>(value + '0');
            } else {
                return static_cast<char>(value - 10 + 'A');
            }
        };

        std::string encodedElement;
        for (auto c : element) {
            if (allowedCharacters.contains(c)) {
                encodedElement.push_back(c);
            } else {
                encodedElement.push_back('%');
                encodedElement.push_back(
                    make_hex_digit(static_cast<unsigned int>(c) >> 4));
                encodedElement.push_back(
                    make_hex_digit(static_cast<unsigned int>(c) & 0x0F));
            }
        }
        return encodedElement;
    }

    /**
     * Most URIs will never change in their life time (at least in webpp
     * project) and they mostly used to get details of the URL we have as a
     * string; so I decided that probabely half of the calculations can be done
     * at compile time; so fo that point, I reimplemented the URI class with
     * constexpr and string_view in mind.
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
     */
    class uri {
      public:
        /**
         * source:
         * https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/encodeURI
         */
        static constexpr auto ALLOWED_CHARACTERS_IN_URI = charset(
            ALPHA, DIGIT,
            charset_t<20>{';', ',', '/', '?', ':', '@', '&',  '=', '+', '$',
                          '-', '_', '.', '!', '~', '*', '\'', '(', ')', '#'});
        /**
         * This is the character set corresponds to the second part
         * of the "scheme" syntax
         * specified in RFC 3986 (https://tools.ietf.org/html/rfc3986).
         */
        static constexpr auto SCHEME_NOT_FIRST =
            charset(ALPHA, DIGIT, webpp::charset('+', '-', '.'));

        /**
         * This is the character set corresponds to the "unreserved" syntax
         * specified in RFC 3986 (https://tools.ietf.org/html/rfc3986).
         */
        static constexpr auto UNRESERVED =
            charset(ALPHA, DIGIT, charset('-', '.', '_', '~'));

        /**
         * This is the character set corresponds to the "sub-delims" syntax
         * specified in RFC 3986 (https://tools.ietf.org/html/rfc3986).
         */
        static constexpr auto SUB_DELIMS = webpp::charset(
            '!', '$', '&', '\'', '(', ')', '*', '+', ',', ';', '=');

        /**
         * This is the character set corresponds to the "userinfo" syntax
         * specified in RFC 3986 (https://tools.ietf.org/html/rfc3986),
         * leaving out "pct-encoded".
         */
        static constexpr auto USER_INFO_NOT_PCT_ENCODED =
            webpp::charset(UNRESERVED, SUB_DELIMS, webpp::charset(':'));

        /**
         * This is the character set corresponds to the last part of
         * the "IPvFuture" syntax
         * specified in RFC 3986 (https://tools.ietf.org/html/rfc3986).
         */
        static constexpr auto IPV_FUTURE_LAST_PART =
            webpp::charset(UNRESERVED, SUB_DELIMS, webpp::charset(':'));

        /**
         * This is the character set corresponds to the "reg-name" syntax
         * specified in RFC 3986 (https://tools.ietf.org/html/rfc3986),
         * leaving out "pct-encoded".
         */
        static constexpr auto REG_NAME_NOT_PCT_ENCODED =
            webpp::charset(UNRESERVED, SUB_DELIMS);

        template <typename T>
        struct uri_segments {

            /**
             * This is the "scheme" element of the URI.
             */
            T scheme;

            /**
             * This is the "UserInfo" element of the URI.
             */
            T user_info;

            /**
             * This is the "host" element of the URI.
             */
            T host;

            /**
             * This is the "path" element of the URI,
             * as a sequence of segments.
             */
            T path;

            /**
             * This is the "query" element of the URI,
             * if it has one.
             */
            T query;

            /**
             * This is the "fragment" element of the URI,
             * if it has one.
             */
            T fragment;

            /**
             * This is the port number element of the URI.
             */
            T port;
        };

        using uri_data_t = std::variant<std::string_view, uri_segments<std::string_view>, uri_segments<std::string>>;

      private:
        mutable uri_data_t data;

        void parse(int index) const noexcept {

            auto data_index = data.index();
            if (index == data_index)
                return;

            // holds string_view but we need uri_segment<string_view> or uri_segment<string>
            if (index >= 1 && data_index == 0) {
                auto _data = std::get<std::string_view>(data);

                uri_segments<std::string_view> segs{};

                std::size_t path_start = std::string_view::npos;
                std::size_t port_start = std::string_view::npos;

                // extracting scheme
                if (const auto schemeEnd = _data.find(':');
                    schemeEnd != std::string_view::npos) {
                    auto _scheme = _data.substr(0, schemeEnd);
                    if (ALPHA.contains(_scheme[0]) &&
                        _scheme.substr(1).find_first_not_of(
                            SCHEME_NOT_FIRST.string_view())) {
                        segs.scheme = _scheme;
                        _data.remove_prefix(schemeEnd);
                    }
                }

                if (auto authority_start = _data.find("//");
                    authority_start != std::string_view::npos) {

                    // finding path so we won't go out of scope:
                    if (path_start = _data.find('/', 2);
                        path_start == std::string_view::npos)
                        _data.remove_suffix(_data.size() - path_start);

                    // extracting user info
                    if (auto delim =
                            _data.find("@", authority_start + 2,
                                       path_start - authority_start + 2);
                        delim != std::string_view::npos) {

                        segs.user_info = _data.substr(authority_start + 2,
                                                      _data.size() - delim);
                        _data.remove_prefix(delim + 1);
                    }

                    // extracting host

                    /* IP future versions can be specified like this:
                     * (RFC 3986)
                     *
                     * IP-literal = "[" ( IPv6address / IPvFuture  ) "]"
                     * IPvFuture = "v" 1*HEXDIG "." 1*( unreserved / sub-delims
                     * / ":" )
                     */
                    if (_data.starts_with('[')) { // IP Literal
                        if (_data.size() > 2 &&
                            _data[1] == 'v') { // IPv Future Number
                            if (auto dot_delim = _data.find('.');
                                dot_delim != std::string_view::npos) {

                                auto ipvf_version = _data.substr(2, dot_delim);
                                if (HEXDIG.contains(ipvf_version)) {

                                    if (auto ipvf_end = _data.find(']');
                                        ipvf_end != std::string_view::npos) {
                                        auto ipvf = _data.substr(dot_delim + 1,
                                                                 ipvf_end);
                                        if (IPV_FUTURE_LAST_PART.contains(
                                                ipvf)) {
                                            segs.host = _data.substr(
                                                1,
                                                ipvf_end); // returning the ipvf
                                                           // and it's version
                                        }
                                    }
                                }
                            }
                        } else if (_data.size() >= 5) { // IPv6

                            if (auto ipv6_end = _data.find(']');
                                ipv6_end != std::string_view::npos) {

                                if (auto ipv6_view = _data.substr(1, ipv6_end);
                                    is::ipv6(ipv6_view)) {
                                    // TODO: probabely use std::variant<ipv6,
                                    // ipv4, string>
                                    segs.host = ipv6_view;
                                    _data.remove_prefix(ipv6_end + 1);
                                }
                            }
                        }
                    } else { // Not IP Literal
                        port_start = _data.find(":", 0, path_start);
                        auto port_or_hostname_start =
                            port_start != std::string_view::npos ? port_start
                                                                 : path_start;
                        auto hostname = _data.substr(0, port_or_hostname_start);
                        // we're not going to decode hostname here. We'll do
                        // this in another method because this function is
                        // constexpr and will only return const stuff

                        // TODO: we have our answer but we will check for the
                        // correctness of the hostname now
                        // FIXME: I think it's wrong
                        if (charset(REG_NAME_NOT_PCT_ENCODED, charset('%'))
                                .contains(hostname)) {
                            segs.host = hostname;
                            _data.remove_prefix(port_or_hostname_start);
                        }
                    }
                }

                // extracting port
                if (auto colon = _data.find(':');
                    colon != std::string_view::npos) {
                    auto port_end =
                        _data.find_first_not_of(DIGIT.string_view());
                    auto port = _data.substr(colon + 1, port_end);
                    if (is::digit(port)) {
                        segs.port = port;
                        _data.remove_prefix(port_end);
                    }
                }


                // extracting path
                if (_data.starts_with('/')) {
                    // it's the query_start actually
                    auto path_end = _data.find('?');
                    if (path_end == std::string_view::npos)
                        path_end = _data.find('#'); // it's hash start

                    segs.path = _data.substr(0, path_end);
                    _data.remove_prefix(path_end);
                }

                // extracting queries
                if (_data.starts_with('?')) {
                    auto hash_start = _data.find('#');
                    segs.query = _data.substr(1, hash_start);
                    _data.remove_prefix(hash_start);
                }

                // extracting hash segment
                if (_data.starts_with('#')) {
                    segs.fragment = _data;
                }


                data = segs;
            }


            // holds uri_segments<string_view> but we need uri_segment<string>
            if (index >= 2 && data_index == 1) {
                uri_segments<std::string> segs;
                auto _data = std::get<uri_segments<std::string_view>>(data);
                segs.scheme = _data.scheme;
                segs.host = _data.host;
                segs.fragment = _data.fragment;
                segs.path = _data.path;
                segs.user_info = _data.user_info;
                segs.port = _data.port;
                segs.query = _data.query;
                data = std::move(segs);
            }
        }


        template <typename ReturnType, typename D>
        std::optional<ReturnType> get_value(std::function<ReturnType(uri_segments<D> const&)> const &func) const noexcept {
            constexpr auto index = std::is_same<ReturnType, uri_segments<std::string_view>>::value ? 1 : (std::is_same<ReturnType, uri_segments<std::string>>::value ? 2 : 0);
            parse(index);
            if (std::holds_alternative<std::string_view>(data))
                return std::nullopt;
            if constexpr (std::is_same<ReturnType, std::string_view>::value && std::is_same<D, std::string>::value) {
                auto const res = func(data);
                return res.empty() ? std::nullopt : std::make_optional(std::move(res));
            }
        }

      public:
        constexpr uri() noexcept = default;
        ~uri() noexcept = default;

        /**
         * @brief parse from string, it will trim the spaces for generality too
         * @param string_view URI string
         */
        constexpr uri(std::string_view const& u) noexcept
            : data(trim_copy(u)) {}

        constexpr uri(uri const& u) noexcept = default;
        constexpr uri(uri&& u) noexcept = default;

        // assignment operators
        constexpr uri& operator=(uri const& u) noexcept = default;
        constexpr uri& operator=(uri&& u) noexcept = default;

        constexpr bool operator==(const uri& u) const noexcept;
        constexpr bool operator!=(const uri& u) const noexcept;


        /**
         * @brief this function is the same as "encodeURI" in javascript
         */
        std::string encoded_uri() noexcept {
            return encode_uri_component<ALLOWED_CHARACTERS_IN_URI.size()>(
                std::string_view(data), ALLOWED_CHARACTERS_IN_URI);
        }

        /**
         * @brief this function is the same as "decodeURI" in javascript
         * @return this function will return an optional<string> object. it will
         * be nullopt when the uri is not valid and has invalid characters
         */
        std::optional<std::string> decoded_uri() noexcept {
            return decode_uri_component<ALLOWED_CHARACTERS_IN_URI.size()>(
                std::string_view(data), ALLOWED_CHARACTERS_IN_URI);
        }

        /**
         * @brief check if the specified uri has a scheme or not
         */
        constexpr bool has_scheme() const noexcept { return scheme().has_value(); }

        /**
         * @brief scheme
         * @return get scheme
         */
        constexpr std::optional<std::string_view> scheme() const noexcept {
            return get_value<std::string_view, std::string_view>([](auto const & _data) {
                return _data.scheme;
            });
        }

        /**
         * @brief set scheme
         * @param _scheme
         * @throws logic_error if uri is const
         * @return
         */
        uri& scheme(std::string_view const& _scheme) {
            check_modifiable();
            if (auto slashes_point = data.find("//");
                std::string::npos != slashes_point) {
                data = std::string(_scheme.substr(0, _scheme.find(':'))) + ":" +
                       data.substr(slashes_point);
            } else {
                // TODO: you still could do something here other than throwing
                // stuff to the user's face
                throw std::invalid_argument(
                    "URI has an invalid syntax; thus we're unable to set the "
                    "specified scheme.");
            }
            return *this;
        }

        /**
         * @brief checks if the uri has user info or not
         */
        constexpr bool has_user_info() const noexcept { return user_info(); }

        /**
         * @brief get the user info or an empty value
         */
        constexpr std::optional<std::string_view> user_info() const noexcept {
            std::string_view _data = data;

            // there's no user info in the uri
            return std::nullopt;
        }

        /**
         * @brief decode user_info and return it as a string
         */
        std::optional<std::string> user_info_decoded() const noexcept {

            auto info = user_info();
            if (!info)
                return std::nullopt;

            return decode_uri_component(info.value(),
                                        USER_INFO_NOT_PCT_ENCODED);
        }

        /**
         * @brief set the user info if it's possible
         */
        uri& user_info(std::string_view const& info) {
            check_modifiable();
            std::string_view _data = data;
            if (auto _user_info = user_info()) {
                // we have already know where it is and we only have to replace
                // it

                auto user_info_start =
                    std::distance(data.cbegin().base(), _user_info.data());
                auto user_info_end =
                    std::distance(data.cbegin().base(), _user_info.data());
                data = _data.substr(0, user_info_start) + info +
                       _data.substr(user_info_end);
            } else {

                // there's no user info so we have to find the place and it
                // ourselves

                if (auto slashes_point = _data.find("//");
                    slashes_point != std::string_view::npos) {
                    data =
                        data.substr(0, slashes_point + 2) +
                        encode_uri_component(info, USER_INFO_NOT_PCT_ENCODED) +
                        "@" + _data.substr(slashes_point + 2);
                } else {
                    // TODO: you still can do some stuff here other than
                    // throwing stuff at the users' face
                    throw std::invalid_argument(
                        "The specified URI is not in a correct shape so we're "
                        "no able to add user info to it.");
                }
            }
            return *this;
        }

        /**
         * @brief clears the user info if exists
         * @return
         */
        uri& clear_user_info() noexcept {
            check_modifiable();
            if (auto _user_info = user_info()) {

                auto user_info_start =
                    std::distance(data.cbegin().base(), _user_info.data());
                auto user_info_end =
                    std::distance(data.cbegin().base(), _user_info.data());

                // removing the user_info from the data + the "@" after it
                data.erase(std::remove(data.begin() + user_info_start,
                                       data.begin() + user_info_end + 1),
                           data.end());
            }
        }

        /**
         * @brief this function will return a string_view of the host and it's
         * port if exists
         */
        constexpr std::string_view host_port_view() const noexcept {
            std::string_view _data = data; // host and the rest of uri
            if (auto authority_part = _data.find("//");
                authority_part != std::string_view::npos) {

                // remove // from the string
                _data.remove_prefix(authority_part + 2);

                if (auto _path = _data.find('/');
                    _path != std::string_view::npos) {
                    // removing the path from the string
                    _data.remove_suffix(_data.size() - _path);
                }

                if (auto _user_info_point = _data.find('@');
                    _user_info_point != std::string_view::npos) {
                    // we know where the user info is placed, so we use that as
                    // a starting point for the rest of search
                    _data.remove_prefix(_user_info_point + 1);
                }

                return _data;
            }

            // it's a path and doesn't have a host
            return "";
        }

        /**
         * @brief returns const_ipv4/const_ipv6/hostname; if the URI doesn't
         * include a valid ip/hostname you'll get an empty string. this method
         * will only return the hostname/ip if it's in the correct format and
         * doesn't include invalid symtax.
         * @return
         */
        constexpr std::variant<const_ipv4, const_ipv6, std::string_view>
        host() const noexcept {
            auto _data = host_port_view();

            if (_data == "")
                return "";
        }

        /**
         * @brief get the hostname/ipv4/ipv6 from the URI. if the URI doesn't
         * include a hostname/ip or its hostname/ip is not in a valid shape, it
         * will return an empty string
         * @return string/ipv4/ipv6
         * @default empty string
         */
        std::variant<ipv4, ipv6, std::string> host_decoded() const noexcept {
            const auto the_host = host();
            return std::visit([](auto&& the_host) -> decltype(auto) {
                using the_host_t = std::decay_t<decltype(the_host)>;
                if constexpr (std::is_same<the_host_t,
                                           std::string_view>::value) {
                    auto hostname_decoded = decode_uri_component(
                        the_host, REG_NAME_NOT_PCT_ENCODED);
                    return hostname_decoded.value_or("");

                } else if constexpr (std::is_same<the_host_t,
                                                  const_ipv4>::value) {
                    // convert to non-const ipv4
                    return ipv4(the_host);
                } else if constexpr (std::is_same<the_host_t,
                                                  const_ipv6>::value) {
                    // convert to non-const ipv6
                    return ipv6(the_host);
                } else {
                    return "";
                }
            });
        }

        /**
         * @brief get the decoded version of hostname/ip of the uri or an empty
         * string if the specified URI does not include a hostname/ip or its
         * hostname has the wrong character encodings.
         * @return string
         */
        std::string host_decoded_str() const noexcept {
            const auto the_host = host();
            return visit([](auto&& the_host) -> decltype(auto) {
                using the_host_t = std::decay_t<decltype(the_host)>;
                if constexpr (std::is_same<the_host_t, std::string>(the_host)) {
                    if (auto hostname_decoded = decode_uri_component(
                            the_host, REG_NAME_NOT_PCT_ENCODED)) {
                        return hostname_decoded;
                    }
                    return ""; // not a valid host
                } else if constexpr (std::is_same<the_host_t, const_ipv4>(
                                         the_host)) {
                    return the_host.str();
                } else if constexpr (std::is_same<the_host_t, const_ipv6>(
                                         the_host)) {
                    return the_host.str();
                } else {
                    return ""; // there's no host?!
                }
            });
        }

        /**
         * @brief this method will check if the hostname/ip exists in the uri or
         * not.
         * @return true if it find a hostname/ip in the uri
         */
        constexpr bool has_host() const noexcept {
            if (auto h = host(); std::holds_alternative<std::string_view>(h)) {
                return h == "";
            }
            return true;
        }

        /**
         * @brief set the hostname/ip in the uri if possible
         */
        uri& host(std::string_view const& new_host) noexcept {

            auto new_host_encoded =
                encode_uri_component(new_host, REG_NAME_NOT_PCT_ENCODED);

            std::size_t host_start, host_end;

            if (auto _host = host_port_view(); _host != "") {
                // we know where it is, let's just replace it
                host_start = std::distance(data.cbegin().base(), _host.data());
                auto _port_start = _host.rfind(':');
                host_end = std::distance(
                    data.cbegin().base(),
                    _host.data() + (_port_start == std::string_view::npos
                                        ? 0
                                        : _port_start));

            } else {
                // check if we have double slash scheme

                std::string_view _data = data;
                auto const _path = path();
                auto path_start =
                    _path != ""
                        ? std::string_view::npos
                        : std::distance(data.cbegin().base(), _path.data());
                if (auto _s = _data.find("//", 0, path_start);
                    _s != std::string_view::npos) {
                    host_start = host_end = _s + 2;
                } else {
                    // there's no double slashes, so we insert one
                    data = "//" + data;
                    host_start = host_end = 2;
                }
            }
            data = data.substr(0, host_start) + new_host_encoded +
                   data.substr(host_end);
        }

        /**
         * @brief port number of the uri;
         * @return port number
         * @default 80
         */
        constexpr unsigned int port() const noexcept {
            if (auto host_port = host_port_view(); host_port != "") {
                if (auto colon = host_port.rfind(':');
                    colon != std::string_view::npos) {
                    auto port = host_port.substr(colon + 1);
                    if (is::digit(port))
                        return atoi(port);
                }
            }
            return 80;
        }

        constexpr bool has_port() const noexcept {
            if (auto host_port = host_port_view(); host_port != "") {
                if (auto colon = host_port.rfind(':');
                    colon != std::string_view::npos) {
                    return is::digit(host_port.substr(colon + 1));
                }
            }
            return false;
        }
        uri& port(unsigned int new_port) {
            if (auto host_port = host_port_view(); host_port != "") {
                if (auto colon = host_port.rfind(':');
                    colon != std::string_view::npos) {
                    // now we just have to replace the port number
                    auto port_start = std::distance(data.cbegin().base(),
                                                    host_port.cbegin().base()) +
                                      colon + 1;
                    auto port_end = std::distance(data.cbegin().base(),
                                                  host_port.cend().base());
                    data = data.substr(0, port_start + 1) +
                           std::to_string(new_port) + data.substr(port_end);
                } else {
                    // now we have to create the port ourselves
                    auto port_start = std::distance(data.cbegin().base(),
                                                    host_port.cend().base());
                    data = data.substr(0, port_start) + ':' +
                           std::to_string(new_port) + data.substr(port_start);
                }
            } else {
                // TODO: I think we still can do something here other than
                // throwing stuff at the users' face
                throw std::invalid_argument(
                    "The URI doesn't seem to have a host; so adding port "
                    "number is not possible.");
            }
            return *this;
        }

        /**
         * @brief clear the port number from the uri and defaults to 80
         * @return self
         */
        uri& clear_port() noexcept {
            if (auto host_port = host_port_view(); host_port != "") {
                if (auto colon = host_port.rfind(':');
                    colon != std::string_view::npos) {
                    // now we just have to replace the port number
                    auto port_start = std::distance(data.cbegin().base(),
                                                    host_port.cbegin().base()) +
                                      colon;
                    auto port_end = std::distance(data.cbegin().base(),
                                                  host_port.cend().base());
                    data =
                        data.substr(0, port_start + 1) + data.substr(port_end);
                }
            }
            return *this;
        }

        /**
         * @brief get the path as the specified type
         * @details this method will returns a vector/list/... of
         * string/string_views
         * this method does not just response to the fact that Container should
         * be an std container, but if string/string_view is presented as a
         * container, it will reutrn the whole path.
         */
        template <typename Container = std::vector<std::string_view>>
        constexpr Container path() const noexcept;

        uri& path(std::string_view const&) noexcept;

        template <typename Container>
        uri& path(const Container&) noexcept;

        constexpr bool has_query() const noexcept;
        constexpr std::string_view query() const noexcept;
        uri& query(std::string_view const&) noexcept;

        /**
         * @details This method applies the "remove_dot_segments" routine talked
         * about in RFC 3986 (https://tools.ietf.org/html/rfc3986) to the path
         * segments of the URI, in order to normalize the path
         * (apply and remove "." and ".." segments).
         */
        uri& normalize_path() { return *this; }

    }; // namespace webpp

    /**
     * This class represents a Uniform Resource Identifier (URI),
     * as defined in RFC 3986 (https://tools.ietf.org/html/rfc3986).
     */
    class Uri {
        // Lifecycle management
      public:
        ~Uri() noexcept;
        Uri(const Uri& other);
        Uri(Uri&&) noexcept;
        Uri& operator=(const Uri& other);
        Uri& operator=(Uri&&) noexcept;

        // Public methods
      public:
        /**
         * This is the default constructor.
         */
        Uri();

        /**
         * This is the equality comparison operator for the class.
         *
         * @param[in] other
         *     This is the other URI to which to compare this URI.
         *
         * @return
         *     An indication of whether or not the two URIs are
         *     equal is returned.
         */
        bool operator==(const Uri& other) const;

        /**
         * This is the inequality comparison operator for the class.
         *
         * @param[in] other
         *     This is the other URI to which to compare this URI.
         *
         * @return
         *     An indication of whether or not the two URIs are
         *     not equal is returned.
         */
        bool operator!=(const Uri& other) const;

        /**
         * This method builds the URI from the elements parsed
         * from the given string rendering of a URI.
         *
         * @param[in] uriString
         *     This is the string rendering of the URI to parse.
         *
         * @return
         *     An indication of whether or not the URI was
         *     parsed successfully is returned.
         */
        bool ParseFromString(const std::string& uriString);

        /**
         * This method returns the "scheme" element of the URI.
         *
         * @return
         *     The "scheme" element of the URI is returned.
         *
         * @retval ""
         *     This is returned if there is no "scheme" element in the URI.
         */
        std::string GetScheme() const;

        /**
         * This method returns the "UserInfo" element of the URI.
         *
         * @return
         *     The "UserInfo" element of the URI is returned.
         *
         * @retval ""
         *     This is returned if there is no "UserInfo" element in the URI.
         */
        std::string GetUserInfo() const;

        /**
         * This method returns the "host" element of the URI.
         *
         * @return
         *     The "host" element of the URI is returned.
         *
         * @retval ""
         *     This is returned if there is no "host" element in the URI.
         */
        std::string GetHost() const;

        /**
         * This method returns the "path" element of the URI,
         * as a sequence of segments.
         *
         * @note
         *     If the first segment of the path is an empty string,
         *     then the URI has an absolute path.
         *
         * @return
         *     The "path" element of the URI is returned
         *     as a sequence of segments.
         */
        std::vector<std::string> GetPath() const;

        /**
         * This method returns an indication of whether or not the
         * URI includes a port number.
         *
         * @return
         *     An indication of whether or not the
         *     URI includes a port number is returned.
         */
        bool HasPort() const;

        /**
         * This method returns the port number element of the URI,
         * if it has one.
         *
         * @return
         *     The port number element of the URI is returned.
         *
         * @note
         *     The returned port number is only valid if the
         *     HasPort method returns true.
         */
        uint16_t GetPort() const;

        /**
         * This method returns an indication of whether or not
         * the URI is a relative reference.
         *
         * @return
         *     An indication of whether or not the URI is a
         *     relative reference is returned.
         */
        bool IsRelativeReference() const;

        /**
         * This method returns an indication of whether or not
         * the URI contains a relative path.
         *
         * @return
         *     An indication of whether or not the URI contains a
         *     relative path is returned.
         */
        bool ContainsRelativePath() const;

        /**
         * This method returns an indication of whether or not the
         * URI includes a query.
         *
         * @return
         *     An indication of whether or not the
         *     URI includes a query is returned.
         */
        bool HasQuery() const;

        /**
         * This method returns the "query" element of the URI,
         * if it has one.
         *
         * @return
         *     The "query" element of the URI is returned.
         *
         * @retval ""
         *     This is returned if there is no "query" element in the URI.
         */
        std::string GetQuery() const;

        /**
         * This method returns an indication of whether or not the
         * URI includes a fragment.
         *
         * @return
         *     An indication of whether or not the
         *     URI includes a fragment is returned.
         */
        bool HasFragment() const;

        /**
         * This method returns the "fragment" element of the URI,
         * if it has one.
         *
         * @return
         *     The "fragment" element of the URI is returned.
         *
         * @retval ""
         *     This is returned if there is no "fragment" element in the URI.
         */
        std::string GetFragment() const;

        /**
         * This method resolves the given relative reference, based on the given
         * base URI, returning the resolved target URI.
         *
         * @param[in] relativeReference
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
        Uri Resolve(const Uri& relativeReference) const;

        /**
         * This method sets the scheme element of the URI.
         *
         * @param[in] scheme
         *     This is the scheme to set for the URI.
         */
        void SetScheme(const std::string& scheme);

        /**
         * This method sets the userinfo element of the URI.
         *
         * @param[in] userinfo
         *     This is the userinfo to set for the URI.
         */
        void SetUserInfo(const std::string& userinfo);

        /**
         * This method sets the host element of the URI.
         *
         * @param[in] host
         *     This is the host to set for the URI.
         */
        void SetHost(const std::string& host);

        /**
         * This method sets the port element of the URI.
         *
         * @param[in] port
         *     This is the port to set for the URI.
         */
        void SetPort(uint16_t port);

        /**
         * This method removes the port element from the URI.
         */
        void ClearPort();

        /**
         * This method sets the path element of the URI.
         *
         * @param[in] path
         *     This is the sequence of segments to use to form the path
         *     to set for the URI.
         *
         *     An empty string segment can be used at the front to
         *     indicate an absolute path (as opposed to a relative one).
         *
         *     An empty string segment can be used at the back to
         *     make sure the path ends in a delimiter (forward slash)
         *     when printed out or when combined with another URI
         *     via the Resolve() method.
         */
        void SetPath(const std::vector<std::string>& path);

        /**
         * This method removes the query element from the URI.
         */
        void ClearQuery();

        /**
         * This method sets the query element of the URI.
         *
         * @param[in] query
         *     This is the query to set for the URI.
         */
        void SetQuery(const std::string& query);

        /**
         * This method removes the fragment element from the URI.
         */
        void ClearFragment();

        /**
         * This method sets the fragment element of the URI.
         *
         * @param[in] fragment
         *     This is the fragment to set for the URI.
         */
        void SetFragment(const std::string& fragment);

        /**
         * This method constructs and returns the string
         * rendering of the URI, according to the rules
         * in RFC 3986 (https://tools.ietf.org/html/rfc3986).
         *
         * @return
         *     The string rendering of the URI is returned.
         */
        std::string GenerateString() const;
    };

} // namespace webpp

#endif // WEBPP_URI_H
