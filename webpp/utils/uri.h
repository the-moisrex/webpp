#ifndef WEBPP_URI_H
#define WEBPP_URI_H

#include "../std/string_view.h"
#include "../validators/validators.h"
#include "casts.h"
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

    namespace is {
        /**
         * @brief check if scheme is correct or not
         * @param _scheme
         * @return
         */
        constexpr bool scheme(std::string_view const& _scheme) noexcept {
            return ALPHA.contains(_scheme);
        };
    } // namespace is

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
                    res.push_back(static_cast<char>(decoded_char));
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

        /**
         * This is the character set corresponds to the "pchar" syntax
         * specified in RFC 3986 (https://tools.ietf.org/html/rfc3986),
         * leaving out "pct-encoded".
         */
        static constexpr auto PCHAR_NOT_PCT_ENCODED = webpp::charset(
            UNRESERVED, SUB_DELIMS, webpp::charset_t<2>{{':', '@'}});

        /**
         * This is the character set corresponds to the "query" syntax
         * and the "fragment" syntax
         * specified in RFC 3986 (https://tools.ietf.org/html/rfc3986),
         * leaving out "pct-encoded".
         */
        static constexpr auto QUERY_OR_FRAGMENT_NOT_PCT_ENCODED =
            webpp::charset(PCHAR_NOT_PCT_ENCODED,
                           webpp::charset_t<2>{{'/', '?'}});

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
             * This is the port number element of the URI.
             */
            T port;

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
        };

        using uri_data_t =
            std::variant<std::string_view, uri_segments<std::string_view>,
                         uri_segments<std::string>>;

      private:
        mutable uri_data_t data;

        void parse(std::size_t index) const noexcept {

            auto data_index = data.index();
            if (index == data_index)
                return;

            // holds string_view but we need uri_segment<string_view> or
            // uri_segment<string>
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

        template <typename ReturnType>
        std::optional<ReturnType> get_value(
            std::function<ReturnType(uri_segments<ReturnType> const&)> const&
                func) const noexcept {
            constexpr auto index =
                std::is_same<ReturnType, uri_segments<std::string_view>>::value
                    ? 1
                    : (std::is_same<ReturnType,
                                    uri_segments<std::string>>::value
                           ? 2
                           : 0);
            parse(index);
            if (std::holds_alternative<uri_segments<ReturnType>>(data)) {
                ReturnType res = func(std::get<uri_segments<ReturnType>>(data));
                return res.empty() ? std::nullopt
                                   : std::make_optional(std::move(res));
            }
            return std::nullopt;
        }

        void set_value(std::function<void(uri_segments<std::string>&)> const&
                           func) noexcept {
            parse(2);
            if (auto _data = std::get_if<uri_segments<std::string>>(&data)) {
                func(*_data);
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
        constexpr bool has_scheme() const noexcept {
            return scheme().has_value();
        }

        /**
         * @brief scheme
         * @return get scheme
         */
        constexpr std::optional<std::string_view> scheme() const noexcept {
            return get_value<std::string_view>(
                [](auto const& _data) { return _data.scheme; });
        }

        /**
         * @brief set scheme
         * @param _scheme
         * @throws logic_error if uri is const
         * @return
         */
        uri& scheme(std::string_view const& _scheme) {
            if (!is::scheme(_scheme))
                throw std::invalid_argument(
                    "The specified scheme is not valid");

            set_value([&](auto& _data) { _data.scheme = _scheme; });
            return *this;
        }

        /**
         * @brief clear scheme from uri
         * @return
         */
        uri& clear_scheme() noexcept {
            set_value([](auto& _data) { _data.scheme = ""; });
            return *this;
        }

        /**
         * @brief checks if the uri has user info or not
         */
        constexpr bool has_user_info() const noexcept {
            return user_info().has_value();
        }

        /**
         * @brief get the user info or an empty value
         */
        constexpr std::optional<std::string_view> user_info() const noexcept {
            return get_value<std::string_view>(
                [](auto const& _data) { return _data.user_info; });
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

        std::string user_info_decoded_str() const noexcept {
            return user_info_decoded().value_or("");
        }

        /**
         * @brief set the user info if it's possible
         */
        uri& user_info(std::string_view const& info) noexcept {
            auto encoded_info =
                encode_uri_component(info, USER_INFO_NOT_PCT_ENCODED);
            set_value([&](auto& _data) { _data.user_info = encoded_info; });
            return *this;
        }

        /**
         * @brief clears the user info if exists
         * @return
         */
        uri& clear_user_info() noexcept {
            set_value([](auto& _data) { _data.user_info = ""; });
            return *this;
        }

        /**
         * @brief this function will return a string_view of the host and it's
         * port if exists
         */
        //        constexpr std::string_view host_port_view() const noexcept {
        //            std::string_view _data = data; // host and the rest of uri
        //            if (auto authority_part = _data.find("//");
        //                authority_part != std::string_view::npos) {

        //                // remove // from the string
        //                _data.remove_prefix(authority_part + 2);

        //                if (auto _path = _data.find('/');
        //                    _path != std::string_view::npos) {
        //                    // removing the path from the string
        //                    _data.remove_suffix(_data.size() - _path);
        //                }

        //                if (auto _user_info_point = _data.find('@');
        //                    _user_info_point != std::string_view::npos) {
        //                    // we know where the user info is placed, so we
        //                    use that as
        //                    // a starting point for the rest of search
        //                    _data.remove_prefix(_user_info_point + 1);
        //                }

        //                return _data;
        //            }

        //            // it's a path and doesn't have a host
        //            return "";
        //        }

        /**
         * @brief return host as an optional<string_view>
         * @return optional<string_view>
         */
        constexpr std::optional<std::string_view> host_string() const noexcept {
            return get_value<std::string_view>(
                [](auto const& _data) { return _data.host; });
        }

        /**
         * @brief returns const_ipv4/const_ipv6/hostname; if the URI doesn't
         * include a valid ip/hostname you'll get an empty string. this method
         * will only return the hostname/ip if it's in the correct format and
         * doesn't include invalid symtax.
         * @return
         */
        constexpr std::optional<std::variant<ipv4, ipv6, std::string_view>>
        host() const noexcept {
            auto host_string_view = get_value<std::string_view>(
                [](auto const& _data) { return _data.host; });
            if (!host_string_view)
                return std::nullopt;
            if (is::ipv4(host_string_view.value()))
                return std::make_optional(ipv4(host_string_view.value()));
            if (is::ipv6(host_string_view.value()))
                return std::make_optional(ipv6(host_string_view.value()));
        }

        /**
         * @brief get the hostname/ipv4/ipv6 from the URI. if the URI doesn't
         * include a hostname/ip or its hostname/ip is not in a valid shape, it
         * will return an empty string
         * @return string/ipv4/ipv6
         * @default empty string
         */
        std::optional<std::variant<ipv4, ipv6, std::string>>
        host_decoded() const noexcept {
            const auto the_host = host();
            if (!the_host)
                return std::nullopt;

            if (std::holds_alternative<std::string>(the_host.value())) {
                auto string_host =
                    std::string(std::get<std::string_view>(the_host.value()));
                return decode_uri_component(string_host,
                                            REG_NAME_NOT_PCT_ENCODED);
            }

            if (std::holds_alternative<ipv4>(the_host.value()))
                return std::make_optional(std::get<ipv4>(the_host.value()));

            return std::make_optional(std::get<ipv6>(the_host.value()));
        }

        /**
         * @brief get the decoded version of hostname/ip of the uri or an empty
         * string if the specified URI does not include a hostname/ip or its
         * hostname has the wrong character encodings.
         * @return string
         */
        std::string host_decoded_str() const noexcept {
            if (auto _host = get_value<std::string>(
                    [](auto const& _data) { return _data.host; })) {
                if (auto decoded = decode_uri_component(
                        _host.value(), REG_NAME_NOT_PCT_ENCODED))
                    return decoded.value();
            }
            return "";
        }

        /**
         * @brief this method will check if the hostname/ip exists in the uri or
         * not.
         * @return true if it find a hostname/ip in the uri
         */
        constexpr bool has_host() const noexcept {
            return host_string().has_value();
        }

        /**
         * @brief set the hostname/ip in the uri if possible
         */
        uri& host(std::string_view const& new_host) noexcept {
            set_value([&](auto& _data) {
                auto new_host_encoded =
                    encode_uri_component(new_host, REG_NAME_NOT_PCT_ENCODED);
                _data.host = new_host_encoded;
            });
            return *this;
        }

        /**
         * @brief clear host part from URI
         * @return
         */
        uri& clear_host() noexcept {
            set_value([](auto& _data) { _data.host = ""; });
            return *this;
        }

        /**
         * @brief port number of the uri;
         * @return port number
         * @default 80
         */
        constexpr uint16_t port() const noexcept {
            if (auto port_str = get_value<std::string_view>(
                    [](auto const& _data) { return _data.port; })) {
                return static_cast<uint16_t>(to_uint(port_str.value()));
            }
            return 80;
        }

        /**
         * @brief checks if the uri has explicitly specified a port number or
         * not
         * @return bool
         */
        constexpr bool has_port() const noexcept {
            return get_value<std::string_view>(
                       [](auto const& _data) { return _data.port; })
                .has_value();
        }

        /**
         * @brief set port
         * @param new_port
         * @return
         */
        uri& port(uint16_t new_port) noexcept {
            set_value(
                [&](auto& _data) { _data.port = std::to_string(new_port); });
            return *this;
        }

        /**
         * @brief clear the port number from the uri and defaults to 80
         * @return self
         */
        uri& clear_port() noexcept {
            set_value([&](auto& _data) { _data.port = ""; });
            return *this;
        }

        /**
         * @brief get path in non-decoded, string format
         * @return
         */
        constexpr std::optional<std::string_view> path() const noexcept {
            return get_value<std::string_view>(
                [](auto const& _data) { return _data.path; });
        }

        /**
         * @brief decoded path as a string
         * @return
         */
        std::optional<std::string> path_decoded() const noexcept {
            if (auto _path = path()) {
                return decode_uri_component(
                    _path.value(), charset(PCHAR_NOT_PCT_ENCODED, '/'));
            }
            return std::nullopt;
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
        constexpr Container path_structured() const noexcept {
            if (auto path_str = path()) {
                Container container;
                auto _path = path_str.value();
                std::size_t slash_start = 0;
                do {
                    slash_start = _path.find('/');
                    container.push_back(_path.substr(0, slash_start));
                    _path.remove_prefix(slash_start + 1);
                } while (!_path.empty());
                return container;
            }
            return {}; // empty path
        }

        /**
         * @brief this one will return a container containing decoded strings of
         * the path.
         * @attention do not use string_view or any alternatives for this method
         * as this method should own its data.
         */
        template <typename Container = std::vector<std::string>>
        Container path_structured_decoded() const noexcept {
            Container container;
            for (auto const& slug : path_structured()) {
                container.push_back(
                    decode_uri_component(slug, PCHAR_NOT_PCT_ENCODED));
            }
            return container;
        }

        /**
         * @brief set the path for the uri
         * @param _path
         * @return
         */
        uri& path(std::string_view const& _path) noexcept {
            set_value([&](auto& _data) {
                auto encoded_path = encode_uri_component(
                    _path, charset(PCHAR_NOT_PCT_ENCODED, '/'));
                _data.path = encoded_path;
            });
            return *this;
        }

        /**
         * @brief set path
         */
        template <typename Container>
        uri& path(const Container& _path) noexcept {
            std::ostringstream joined_path;
            copy(_path.cbegin(), _path.cend() - 1,
                 std::ostream_iterator<std::string>(joined_path, "/"));
            joined_path << *_path.crbegin();
            return path(joined_path);
        }

        /**
         * @brief clear path from the URI
         * @return
         */
        uri& clear_path() noexcept {
            set_value([](auto& _data) { _data.path = ""; });
            return *this;
        }

        /**
         * @brief checks if the path is an absolute path or relative path
         * @return
         */
        constexpr bool is_absolute() const noexcept {
            return path().value_or("/").starts_with('/');
        }

        /**
         * @brief checks if the path is a relative path or an absolute one
         * @return
         */
        constexpr bool is_relative() const noexcept { return !is_absolute(); }

        /**
         * @brief checks if the uri has query or not
         * @return
         */
        constexpr bool has_query() const noexcept {
            return query().has_value();
        }

        constexpr std::optional<std::string_view> query() const noexcept {
            return get_value<std::string_view>(
                [](auto const& _data) { return _data.query; });
        }

        /**
         * @brief set query
         * @param _query
         * @return
         */
        uri& query(std::string_view const& _query) {
            if (!is::query(_query))
                throw std::invalid_argument(
                    "The specified string is not a valid query");

            set_value([&](auto& _data) {
                _data.query = encode_uri_component(
                    _query, QUERY_OR_FRAGMENT_NOT_PCT_ENCODED);
            });
            return *this;
        }

        uri& query(
            std::initializer_list<std::string_view> const& _queries) noexcept {
            // TODO
        }

        /**
         * @brief clear the query section of the URI
         * @return
         */
        uri& clear_query() noexcept {
            set_value([](auto& _data) { _data.query = ""; });
            return *this;
        }

        auto query_structured() const noexcept {
            // TODO
        }

        auto query_structured_decoded() const noexcept {
            // TODO
        }

        /**
         * @brief checks if the uri path is normalized or not (contains relative
         * . or .. paths)
         * @return
         */
        bool is_normalized() const noexcept {
            auto _path = path_structured();
            for (auto const& p : _path)
                if (p == "." || p == "..")
                    return true;
            return false;
        }

        /**
         * @details This method applies the "remove_dot_segments" routine talked
         * about in RFC 3986 (https://tools.ietf.org/html/rfc3986) to the path
         * segments of the URI, in order to normalize the path
         * (apply and remove "." and ".." segments).
         */
        uri& normalize_path() noexcept { return *this; }

        /**
         * @brief get fragment
         */
        constexpr std::optional<std::string_view> fragment() const noexcept {
            return get_value<std::string_view>(
                [](auto const& _data) { return _data.fragment; });
        }

        /**
         * @brief get fragment in string format
         * @return
         */
        constexpr std::string_view fragment_str() const noexcept {
            return fragment().value_or("");
        }

        /**
         * @brief an indication of whether the URI has fragment or not.
         * @return
         */
        constexpr bool has_fragment() const noexcept {
            return fragment().has_value();
        }

        /**
         * @brief clear the fragment part of the uri
         * @return
         */
        uri& clear_fragment() noexcept {
            set_value([](auto& _data) { _data.fragment = ""; });
            return *this;
        }

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
