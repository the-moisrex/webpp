#ifndef URI_H
#define URI_H

#include "../std/string_view.h"
#include "charset.h"
#include "strings.h"
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>
#include <functional>

namespace webpp {

    
    /**
     * @brief this function will decode parts of uri
     */
    template <std::size_t N>
    std::optional<std::string> decode_uri_component(std::string_view const& encoded_str,
                                        charset_t<N> const& allowed_chars) noexcept {
        /**
            * This is the character set containing just the upper-case
            * letters 'A' through 'F', used in upper-case hexadecimal.
            */
        constexpr auto HEX_UPPER = charset<'A', 'F'>();

        /**
            * This is the character set containing just the lower-case
            * letters 'a' through 'f', used in lower-case hexadecimal.
            */
        const auto HEX_LOWER = charset<'a', 'f'>();

        int digits_left = 2;
        char decoded_char = 0;
        bool decoding = false;
        std::string res;
        for (const auto c : encoded_str) {
            if (digits_left) {
                decoded_char <<= 4;
                if (DIGIT.contains(c)) {
                    decoded_char += c - '0';
                } else if (HEX_UPPER.contains(c)) {
                    decoded_char += c - 'A' + 10;
                } else if (HEX_LOWER.contains(c)) {
                    decoded_char += c - 'a' + 10;
                } else {
                    return std::nullopt; // not a encrypted well
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
     * @brief this function is supposed to be the same as decodeURIComponent in javascript
     */
    auto decode_uri(std::string_view const& encoded_str) noexcept {
        constexpr auto URI_ALLOWED_CHARACTERS = charset(
            ALPHA,
            DIGIT,
            charset_t<20>{ ';', ',', '/', '?', ':', '@', '&', '=', '+', '$', '-',
                '_', '.', '!', '~', '*', '\'', '(', ')', '#' }
        );
        return decode_uri_component<URI_ALLOWED_CHARACTERS.size()>(encoded_str, URI_ALLOWED_CHARACTERS);
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
            if (auto slashes_point = data.find("//"); std::string::npos != slashes_point) {
                data = std::string(_scheme.substr(0, _scheme.find(':'))) + ":" + data.substr(slashes_point);
            } else {
                throw std::invalid_argument("URI has an invalid syntax; thus we're unable to set the specified scheme.");
            }
        }

        constexpr std::pair<std::string::iterator, std::size_t> 
                user_info_span() const noexcept 
        {
            std::string_view _data = data;

            if (auto authority_start = _data.find("//");
                authority_start != std::string_view::npos) {
                authority_start +=
                    2; // we already know what those chars are (//)

                // finding path so we won't go out of scope:
                auto path_start = _data.find('/');
                if (path_start == std::string_view::npos)
                    path_start = _data.size();

                if (auto delim = _data.find("@", authority_start, path_start);
                    delim != std::string_view::npos &&
                    delim != authority_start) {
                    return std::make_pair(data.begin() + authority_start,
                                            delim - authority_start);
                }
            }
            return std::make_pair(data.end(), 0); // there's no user info in the uri
        }

        /**
         * @brief checks if the uri has user info or not
         */
        constexpr bool has_user_info() const noexcept {
            return user_info_span().first != data.end();
        }

        /**
         * @brief get the user info or an empty value
         */
        constexpr std::optional<std::string_view> user_info() const noexcept {
            auto points = user_info_span();
            if (points.first == data.end())
                return std::nullopt; // there is no user info in the uri
            return std::string_view(points.first.base(), points.second);
        }
        
        
        /**
         * @brief decode user_info and return it as a string
         */
        std::optional<std::string> user_info_decoded() const noexcept {
            
            auto info = user_info();
            if (!info)
                return std::nullopt;

            /**
             * This is the character set corresponds to the "unreserved" syntax
             * specified in RFC 3986 (https://tools.ietf.org/html/rfc3986).
             */
            constexpr auto UNRESERVED =
                charset(ALPHA, DIGIT, charset('-', '.', '_', '~'));

            /**
             * This is the character set corresponds to the "sub-delims" syntax
             * specified in RFC 3986 (https://tools.ietf.org/html/rfc3986).
             */
            constexpr auto SUB_DELIMS = webpp::charset(
                '!', '$', '&', '\'', '(', ')', '*', '+', ',', ';', '=');

            /**
             * This is the character set corresponds to the "userinfo" syntax
             * specified in RFC 3986 (https://tools.ietf.org/html/rfc3986),
             * leaving out "pct-encoded".
             */
            constexpr auto USER_INFO_NOT_PCT_ENCODED =
                webpp::charset(UNRESERVED, SUB_DELIMS, webpp::charset(':'));

            return decode_uri_component (info.value(), USER_INFO_NOT_PCT_ENCODED);
        }
        
        /**
         * @brief set the user info if it's possible
         */
        uri_t& user_info(std::string_view const& info) {
            check_modifiable();
            auto points = user_info_span();
            std::string_view _data = data;
            if (points.first == data.end()) {
                // there's no user info so we have to find the place and it ourselves
                
                if (auto slashes_point = _data.find("//"); slashes_point != std::string_view::npos) {
                    data = data.substr(0, slashes_point + 2) + uri_encode(info) + _data.substr(slashes_point + 2);
                } else {
                    throw std::invalid_argument("The specified URI is not in a correct shape so we're no able to add user info to it.");
                }
            } else {
                // we have already know where it is and we only have to replace it
                
                auto user_info_start = std::distance(data.begin(), points.first);
                auto user_info_end = std::distance(data.begin(), points.second);
                data = _data.substr(0, user_info_start) + info + _data.substr(user_info_end);
            }
        }

        /**
         * @brief clears the user info if exists
         * @return
         */
        uri_t& clear_user_info() noexcept {
            check_modifiable();
            auto points = user_info_span();
            if (points.first == data.end())
                return; // there's no user_info thus we don't need to change anything
            
            // removing the user_info from the data + the "@" after it
            data.erase(std::remove(points.first, points.first + points.second + 1), data.end());
            
        }

        constexpr std::pair<std::string::iterator, std::size_t> host_span() const noexcept {
            
        }
        
        /**
         * @brief this method will check if the hostname/ip exists in the uri or not.
         * @return true if it find a hostname/ip in the uri
         */
        constexpr bool has_host() const noexcept {
            return host_span().first != data.end();
        }
        constexpr std::optional<std::string_view> host() const noexcept;
        uri_t& host(std::string_view const&) noexcept;

        /**
         * @brief port number of the uri;
         * @return port number
         * @default 80
         */
        constexpr unsigned int port() const noexcept;
        constexpr bool has_port() const noexcept;
        uri_t& port(unsigned int) noexcept;

        /**
         * @brief clear the port number from the uri and defaults to 80
         * @return self
         */
        uri_t& clear_port() noexcept;

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
    };

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

#endif // URI_H
