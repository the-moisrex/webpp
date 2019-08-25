#ifndef URI_H
#define URI_H

#include "../std/string_view.h"
#include "charset.h"
#include <memory>
#include <string>
#include <vector>

namespace webpp {

    /**
     * Most URIs will never change in their life time (at least in webpp
     * project) and they mostly used to get details of the URL we have as a
     * string; so I decided that probabely half of the calculations can be done
     * at compile time; so fo that point, I reimplemented the URI class with
     * constexpr and string_view in mind.
     *
     */
    template <typename StringType>
    class uri_t {
      private:
        StringType data;

      public:
        constexpr uri_t() noexcept = default;
        ~uri_t() noexcept = default;

        // parse from string
        constexpr uri_t(std::string_view const& u) noexcept = default;

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
        constexpr bool has_scheme() const noexcept { return scheme() == ""; }

        /**
         * @brief scheme
         * @return get scheme
         */
        constexpr std::string_view scheme() const noexcept {
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
                    return "";
                if (!_scheme.substr(1).find_first_not_of(
                        SCHEME_NOT_FIRST.string_view()))
                    return "";
                return _scheme;
            }
            return "";
        }

        uri_t& scheme(std::string_view const& _scheme) noexcept;

        constexpr std::string_view user_info() const noexcept;
        uri_t& user_info(std::string_view const&) noexcept;

        /**
         * @brief clears the user info if exists
         * @return
         */
        uri_t& clear_user_info() noexcept;

        constexpr bool has_host() const noexcept;
        constexpr std::string_view host() const noexcept;
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

    using const_uri = uri_t<std::string_view>;
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
