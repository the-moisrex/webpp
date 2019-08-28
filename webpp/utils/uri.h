#ifndef WEBPP_URI_H
#define WEBPP_URI_H

#include "../std/string_view.h"
#include "../validators/validators.h"
#include "charset.h"
#include "strings.h"
#include <functional>
#include <memory>
#include <stdexcept>
#include <string>
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
        char decoded_char = 0;
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
                    return std::nullopt; // not encrypted well
                }
                --digits_left;

                if (digits_left == 0) {
                    decoding = false;
                    res.push_back(decoded_char);
                }
            } else if (c == '%') {
                decoding = true;

                // reseting:
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
    template <typename StringType>
    class uri_t {
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

      private:
        StringType data;

        void check_modifiable() {
            if constexpr (std::is_same<StringType, std::string_view>::value) {
                throw std::logic_error(
                    "You cannot change a string_view thus you will not be able "
                    "to run non-const methods of uri_t class");
            }
            if constexpr (std::is_const<StringType>::value) {
                throw std::logic_error(
                    "You cannot change a const string thus you are not able to "
                    "run non-const methods of uri_t class");
            }
        }

      public:
        constexpr uri_t() noexcept = default;
        ~uri_t() noexcept = default;

        /**
         * @brief parse from string, it will trim the spaces for generality too
         * @param string_view reperesentaion of a URI
         */
        constexpr uri_t(std::string_view const& u) noexcept
            : data(trim_copy(u)) {}

        constexpr uri_t(uri_t const& u) noexcept = default;
        constexpr uri_t(uri_t&& u) noexcept = default;

        // assignment operators
        constexpr void operator=(uri_t const& u) noexcept = default;
        constexpr void operator=(uri_t&& u) noexcept = default;

        constexpr bool operator==(const uri_t& u) const noexcept;
        constexpr bool operator!=(const uri_t& u) const noexcept;

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
        constexpr bool has_scheme() const noexcept { return scheme(); }

        /**
         * @brief scheme
         * @return get scheme
         */
        constexpr std::optional<std::string_view> scheme() const noexcept {
            /**
             * This is the character set corresponds to the second part
             * of the "scheme" syntax
             * specified in RFC 3986 (https://tools.ietf.org/html/rfc3986).
             */
            constexpr auto SCHEME_NOT_FIRST =
                charset(ALPHA, DIGIT, webpp::charset('+', '-', '.'));

            std::string_view _data =
                data; // to make sure we have string_view not a string or any
                      // other thing that we'r not going to work with in this
                      // method

            if (const auto schemeEnd = _data.find(':');
                schemeEnd != std::string_view::npos) {
                auto _scheme = _data.substr(0, schemeEnd);
                if (!ALPHA.contains(_scheme[0]))
                    return std::nullopt;
                if (!_scheme.substr(1).find_first_not_of(
                        SCHEME_NOT_FIRST.string_view()))
                    return std::nullopt;
                return _scheme;
            }
            return std::nullopt;
        }

        /**
         * @brief set scheme
         * @param _scheme
         * @throws logic_error if uri is const
         * @return
         */
        uri_t& scheme(std::string_view const& _scheme) {
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

            if (auto authority_start = _data.find("//");
                authority_start != std::string_view::npos) {

                // we already know what those chars are (//)
                _data.remove_prefix(authority_start + 2);

                // finding path so we won't go out of scope:
                if (auto path_start = _data.find('/');
                    path_start == std::string_view::npos)
                    _data.remove_suffix(_data.size() - path_start);

                if (auto delim = _data.find("@");
                    delim != std::string_view::npos) {
                    _data.remove_suffix(_data.size() - delim);
                    return _data;
                }
            }

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
        uri_t& user_info(std::string_view const& info) {
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
        }

        /**
         * @brief clears the user info if exists
         * @return
         */
        uri_t& clear_user_info() noexcept {
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

        constexpr std::string_view host_span() const noexcept {
            /**
             * These are the various states for the state machine
             * implemented below to correctly split up and validate the URI
             * substring containing the host and potentially a port number
             * as well.
             */
            enum class state_t {
                FIRST_CHARACTER,
                NOT_IP_LITERAL,
                PERCENT_ENCODED_CHARACTER,
                IP_LITERAL,
                IPV6_ADDRESS,
                IPV_FUTURE_NUMBER,
                IPV_FUTURE_BODY,
                GARBAGE_CHECK,
                PORT,
            };

            auto _data = host_port_view();

            // Next, parsing host and port from authority and path.
            std::string portString;
            state_t state = state_t::FIRST_CHARACTER;
            bool hostIsRegName = false;
            for (const auto& c : _data) {
                switch (state) {
                case state_t::FIRST_CHARACTER: {
                    if (c == '[') {
                        state = state_t::IP_LITERAL;
                        break;
                    } else {
                        state = state_t::NOT_IP_LITERAL;
                        hostIsRegName = true;
                    }
                }

                case state_t::NOT_IP_LITERAL: {
                    if (c == '%') {
                        pecDecoder = PercentEncodedCharacterDecoder();
                        state = state_t::PERCENT_ENCODED_CHARACTER;
                    } else if (c == ':') {
                        state = state_t::PORT;
                    } else {
                        if (REG_NAME_NOT_PCT_ENCODED.contains(c)) {
                            host.push_back(c);
                        } else {
                            return false;
                        }
                    }
                } break;

                case state_t::PERCENT_ENCODED_CHARACTER: {
                    if (!pecDecoder.NextEncodedCharacter(c)) {
                        return false;
                    }
                    if (pecDecoder.Done()) {
                        state = state_t::NOT_IP_LITERAL;
                        host.push_back((char)pecDecoder.GetDecodedCharacter());
                    }
                } break;

                case state_t::IP_LITERAL: {
                    if (c == 'v') {
                        host.push_back(c);
                        state = state_t::IPV_FUTURE_NUMBER;
                        break;
                    } else {
                        state = state_t::IPV6_ADDRESS;
                    }
                }

                case state_t::IPV6_ADDRESS: {
                    if (c == ']') {
                        if (!ValidateIpv6Address(host)) {
                            return false;
                        }
                        state = state_t::GARBAGE_CHECK;
                    } else {
                        host.push_back(c);
                    }
                } break;

                case state_t::IPV_FUTURE_NUMBER: {
                    if (c == '.') {
                        state = state_t::IPV_FUTURE_BODY;
                    } else if (!HEXDIG.contains(c)) {
                        return false;
                    }
                    host.push_back(c);
                } break;

                case state_t::IPV_FUTURE_BODY: {
                    if (c == ']') {
                        state = state_t::GARBAGE_CHECK;
                    } else if (!IPV_FUTURE_LAST_PART.Contains(c)) {
                        return false;
                    } else {
                        host.push_back(c);
                    }
                } break;

                case state_t::GARBAGE_CHECK: {
                    // illegal to have anything else, unless it's a colon,
                    // in which case it's a port delimiter
                    if (c == ':') {
                        state = state_t::PORT;
                    } else {
                        return false;
                    }
                } break;

                case state_t::PORT: {
                    portString.push_back(c);
                } break;
                }
            }
            if ((state != state_t::FIRST_CHARACTER) &&
                (state != state_t::NOT_IP_LITERAL) &&
                (state != state_t::GARBAGE_CHECK) && (state != state_t::PORT)) {
                // truncated or ended early
                return false;
            }
            if (hostIsRegName) {
                host = SystemAbstractions::ToLower(host);
            }
            if (portString.empty()) {
                hasPort = false;
            } else {
                intmax_t portAsInt;
                if (SystemAbstractions::ToInteger(portString, portAsInt) !=
                    SystemAbstractions::ToIntegerResult::Success) {
                    return false;
                }
                if ((portAsInt < 0) ||
                    (portAsInt >
                     (decltype(portAsInt))
                         std::numeric_limits<decltype(port)>::max())) {
                    return false;
                }
                port = static_cast<decltype(port)>(portAsInt);
                hasPort = true;
            }
        }

        /**
         * @brief this method will check if the hostname/ip exists in the uri or
         * not.
         * @return true if it find a hostname/ip in the uri
         */
        constexpr bool has_host() const noexcept {
            return host_port_view() != "";
        }

        /**
         * @brief return the hostname/ip if it exists in the uri.
         */
        constexpr std::optional<std::string_view> host() const noexcept {
            if (auto info = host_span(); info.first != data.end())
                return std::string_view(info.first.base(), info.second);
            return std::nullopt;
        }

        /**
         * @brief set the hostname/ip in the uri if possible
         */
        uri_t& host(std::string_view const&) noexcept;

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
        uri_t& port(unsigned int new_port) {
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
        }

        /**
         * @brief clear the port number from the uri and defaults to 80
         * @return self
         */
        uri_t& clear_port() noexcept {
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

        uri_t& path(std::string_view const&) noexcept;

        template <typename Container>
        uri_t& path(const Container&) noexcept;

        constexpr bool has_query() const noexcept;
        constexpr std::string_view query() const noexcept;
        uri_t& query(std::string_view const&) noexcept;
    }; // namespace webpp

    using const_uri = uri_t<const std::string_view>;
    using uri = uri_t<std::string>;

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
         * This method applies the "remove_dot_segments" routine talked about
         * in RFC 3986 (https://tools.ietf.org/html/rfc3986) to the path
         * segments of the URI, in order to normalize the path
         * (apply and remove "." and ".." segments).
         */
        void NormalizePath();

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
