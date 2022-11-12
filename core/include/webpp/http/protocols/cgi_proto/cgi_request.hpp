// Created by moisrex on 10/24/20.

#ifndef WEBPP_CGI_REQUEST_HPP
#define WEBPP_CGI_REQUEST_HPP

#include "../../../std/string_view.hpp"
#include "../../../traits/traits.hpp"
#include "../../http_concepts.hpp"
#include "../../request_headers.hpp"
#include "../../request_view.hpp"

// TODO: use GetEnvironmentVariableA for Windows operating system
#include <iostream> // for access to cin and cout
#include <unistd.h> // for environ

namespace webpp::http {

    template <typename CommonHTTPRequest>
    struct cgi_request final : public CommonHTTPRequest, details::request_view_interface {
        using common_http_request_type = CommonHTTPRequest;
        using traits_type              = typename common_http_request_type::traits_type;
        using server_type              = typename common_http_request_type::server_type;

      private:
        using super            = CommonHTTPRequest;
        using server_ref       = typename super::server_ref;
        using string_view_type = typename super::string_view_type;
        using string_type      = typename super::string_type;
        using char_type        = traits::char_type<traits_type>;

        string_type headers_holder;

        string_view_type put_header_name(string_view_type name) {
            headers_holder.append(name.data(), name.size());
            stl::replace(headers_holder.end() - name.size(), headers_holder.end(), '_', '-');
            return {headers_holder.data() + headers_holder.size() - name.size(), name.size()};
        }

        string_view_type put_header_value(string_view_type value) {
            headers_holder.append(value.data(), value.size());
            return {headers_holder.data() + headers_holder.size() - value.size(), value.size()};
        }

        void fill_headers() {
            static constexpr string_view_type HTTP_prefix = "HTTP_";
            for (auto it = ::environ; *it; it++) {
                string_view_type hdr{*it};
                if (ascii::starts_with(hdr, HTTP_prefix)) {
                    hdr.remove_prefix(HTTP_prefix.size());
                    auto const       equal_sign = hdr.find('=');
                    string_view_type name       = hdr.substr(0, equal_sign);
                    string_view_type value      = hdr.substr(equal_sign + 1);

                    name  = put_header_name(name);
                    value = put_header_value(value);
                    this->headers.emplace(name, value);
                }
            }
        }

      protected:
        using pstring_type = typename basic_request_view::string_type;

        template <typename T>
        [[nodiscard]] inline pstring_type pstringify(T&& str) const {
            return istl::stringify_of<pstring_type>(stl::forward<T>(str),
                                                    alloc::general_alloc_for<pstring_type>(*this));
        }

        // get the dynamic request object
        inline basic_request_view const& dreq() const noexcept {
            return static_cast<basic_request_view const&>(*this);
        }

        [[nodiscard]] pstring_type get_method() const override {
            return pstringify(this->method());
        }

        [[nodiscard]] pstring_type get_uri() const override {
            return pstringify(this->uri());
        }

        [[nodiscard]] http::version get_version() const noexcept override {
            return this->version();
        }

      public:
        cgi_request(server_ref svr)
          : super{svr},
            headers_holder{alloc::general_alloc_for<string_type>(*this)} {
            fill_headers();
        }

        cgi_request(cgi_request const&)                = default;
        cgi_request(cgi_request&&) noexcept            = default;
        cgi_request& operator=(cgi_request const&)     = default;
        cgi_request& operator=(cgi_request&&) noexcept = default;
        ~cgi_request() final {}

        /**
         * Get the environment value safely
         */
        [[nodiscard]] inline string_view_type env(char const* key) const noexcept {
            return server_type::env(key);
        }



        /**
         * @brief get the server's software
         * @details Name and version of the information server software
         * answering the request (and running the gateway). Format: name/version.
         * @example SERVER_SOFTWARE=Apache/2.4.41 (Unix) OpenSSL/1.1.1d
         */
        [[nodiscard]] string_view_type server_software() const noexcept {
            return env("SERVER_SOFTWARE");
        }

        /**
         * @brief get the server name
         * @details Server's hostname, DNS alias, or IP address as it appears in
         * self-referencing URLs.
         * @example SERVER_NAME=localhost
         */
        [[nodiscard]] string_view_type server_name() const noexcept {
            return env("SERVER_NAME");
        }

        /**
         * @brief get the gateway interface environment variable
         * @details CGI specification revision with which this server complies.
         * Format: CGI/revision.
         * @example GATEWAY_INTERFACE=CGI/1.1
         */
        [[nodiscard]] string_view_type gateway_interface() const noexcept {
            return env("GATEWAY_INTERFACE");
        }

        /**
         * @brief get the server protocol
         * @details Name and revision of the information protocol this request
         * came in with. Format: protocol/revision.
         * @example SERVER_PROTOCOL=HTTP/1.1
         */
        [[nodiscard]] string_view_type server_protocol() const noexcept {
            return env("SERVER_PROTOCOL");
        }

        /**
         * @brief Get the HTTP version of the request
         * If the server didn't specify any protocol, then this method has no way of knowing thus
         * it'll return an unknown version.
         */
        [[nodiscard]] http::version version() const noexcept {
            auto const protocol = server_protocol();
            return http::version::from_server_protocol(protocol);
        }

        /**
         * @brief get the port that the server is listening on
         * @details Port number to which the request was sent.
         */
        [[nodiscard]] string_view_type server_port() const noexcept {
            return env("SERVER_PORT");
        }

        /**
         * @brief Get the method
         * @details Method with which the request was made. For HTTP, this is Get, Head, Post, and so on.
         */
        [[nodiscard]] string_view_type method() const noexcept {
            return env("REQUEST_METHOD");
        }

        /**
         * @brief get the path info
         * @details Extra path information, as given by the client. Scripts can
         * be accessed by their virtual pathname, followed by extra information
         * at the end of this path. The extra information is sent as PATH_INFO.
         * @example PATH_INFO=/hello/world
         */
        [[nodiscard]] string_view_type path_info() const noexcept {
            return env("PATH_INFO");
        }

        /**
         * @brief get the path translated
         * @details Translated version of PATH_INFO after any
         * virtual-to-physical mapping.
         * @example PATH_TRANSLATED=/srv/http/hello/world
         */
        [[nodiscard]] string_view_type path_translated() const noexcept {
            return env("PATH_TRANSLATED");
        }

        /**
         * @brief get the script name
         * @details Virtual path to the script that is executing; used for
         * self-referencing URLs.
         * @example SCRIPT_NAME=/cgi-bin/one.cgi
         */
        [[nodiscard]] string_view_type script_name() const noexcept {
            return env("SCRIPT_NAME");
        }

        /**
         * @brief get the query string
         * @details Query information that follows the ? in the URL that
         * referenced this script.
         */
        [[nodiscard]] string_view_type query_string() const noexcept {
            return env("QUERY_STRING");
        }

        /**
         * @brief get the remote host
         * @details Hostname making the request. If the server does not have
         * this information, it sets REMOTE_ADDR and does not set REMOTE_HOST.
         */
        [[nodiscard]] string_view_type remote_host() const noexcept {
            return env("REMOTE_HOST");
        }

        /**
         * @brief get the ip address of the user
         * @details IP address of the remote host making the request.
         */
        [[nodiscard]] string_view_type remote_addr() const noexcept {
            return env("REMOTE_ADDR");
        }

        /**
         * @brief get the auth type
         * @details If the server supports user authentication, and the script
         * is protected, the protocol-specific authentication method used to
         * validate the user.
         */
        [[nodiscard]] string_view_type auth_type() const noexcept {
            return env("AUTH_TYPE");
        }

        /**
         * @brief get the remote user or auth user value (both should be the
         * same)
         * @details If the server supports user authentication, and the script
         * is protected, the username the user has authenticated as. (Also
         * available as AUTH_USER.)
         */
        [[nodiscard]] string_view_type remote_user() const noexcept {
            if (auto a = env("REMOTE_USER"); !a.empty())
                return a;
            return env("AUTH_USER");
        }

        /**
         * @brief get the remote user or auth user value (both should be the
         * same)
         * @details If the server supports user authentication, and the script
         * is protected, the username the user has authenticated as. (Also
         * available as AUTH_USER.)
         */
        [[nodiscard]] string_view_type auth_user() const noexcept {
            if (auto a = env("AUTH_USER"); !a.empty())
                return a;
            return env("REMOTE_USER");
        }

        /**
         * @brief get the remote ident
         * @details If the HTTP server supports RFC 931 identification, this
         * variable is set to the remote username retrieved from the server. Use
         * this variable for logging only.
         */
        [[nodiscard]] string_view_type remote_ident() const noexcept {
            return env("REMOTE_IDENT");
        }

        /**
         * @brief returns the request scheme (http/https/...)
         */
        [[nodiscard]] string_view_type request_scheme() const noexcept {
            return env("REQUEST_SCHEME");
        }

        /**
         * @brief get the user's port number
         */
        [[nodiscard]] string_view_type remote_port() const noexcept {
            return env("REMOTE_PORT");
        }

        /**
         * @brief get the ip address that the server is listening on
         */
        [[nodiscard]] string_view_type server_addr() const noexcept {
            return env("SERVER_ADDR");
        }

        /**
         * @brief get the request uri
         */
        [[nodiscard]] string_view_type uri() const noexcept {
            return env("REQUEST_URI");
        }

        /**
         * @brief get the content_type
         * @details For queries that have attached information, such as HTTP
         * POST and PUT, this is the content type of the data.
         */
        [[nodiscard]] string_view_type content_type() const noexcept {
            return env("CONTENT_LENGTH");
        }

        /**
         * @brief get the content length
         * @details Length of the content as given by the client.
         */
        [[nodiscard]] string_view_type content_length() const noexcept {
            return env("CONTENT_LENGTH");
        }

        /**
         * @brief get the document root environment value
         * @details The root directory of your server
         */
        [[nodiscard]] string_view_type document_root() const noexcept {
            return env("DOCUMENT_ROOT");
        }

        /**
         * @brief get the https environment value
         * @return "on" if the user used HTTPS protocol
         */
        [[nodiscard]] string_view_type https() const noexcept {
            return env("HTTPS");
        }

        /**
         * @brief get the server admin environment value
         * @return probably the administrator's email address
         */
        [[nodiscard]] string_view_type server_admin() const noexcept {
            return env("SERVER_ADMIN");
        }

        /**
         * @brief get the path environment variable
         * @details The system path your server is running under
         */
        [[nodiscard]] string_view_type path() const noexcept {
            return env("PATH");
        }

        /**
         * @brief get the script_filename of the environment variables
         * @details The full pathname of the current CGI
         */
        [[nodiscard]] string_view_type script_filename() const noexcept {
            return env("SCRIPT_FILENAME");
        }

        /**
         * @brief get a single header
         * @param name
         */
        [[nodiscard]] string_view_type get_header(string_view_type name) const noexcept {
            return header(string_type(name, this->alloc_pack.template general_allocator<char_type>()));
        }


        /**
         * Get a specific header by it's name
         */
        [[nodiscard]] string_view_type get_header(stl::string name) noexcept {
            // fixme: check if this is all we have to do or we have to do more too:
            stl::transform(name.begin(), name.end(), name.begin(), [](auto const& c) {
                if (c == '-')
                    return '_';
                return static_cast<char>(stl::toupper(c));
            });

            name.insert(0, "HTTP_");
            return env(name.c_str());
        }

        /**
         * @brief get all of the headers as a string_view
         * @details this method in CGI interfaces will have to do more than
         * in the our own version because data has already been parsed by the
         * CGI server; and we have to recreate it based on the environment
         * variables.
         */
        [[nodiscard]] string_view_type get_headers() noexcept {
            // we can do this only in CGI, we have to come up with new ways for long-running protocols:
            static string_type headers_cache;
            if (headers_cache.empty()) {
                // TODO: this code won't work on windows. Change when you are worried about windows
                for (auto it = ::environ; *it; it++) {
                    string_view_type h{*it};
                    if (starts_with(h, "HTTP_")) {
                        headers_cache.append(h.substr(5));
                        // FIXME: decide if you need to convert _ to - or not.
                    }
                }
            }
            return headers_cache;
        }

        /**
         * @brief get the whole body as a string_view
         * @details this method will return raw string values of the body of
         * the request and will not parse it. Parsing it is another methods'
         * problem that might even use this function as the source.
         */
        [[nodiscard]] string_view_type get_body() noexcept {
            // again, we can do this only in cgi protocol not in other interfaces:
            static string_type body_cache{this->alloc_pack.template general_allocator<char_type>()};
            if (body_cache.empty()) {
                if (auto content_length_str = env("CONTENT_LENGTH"); !content_length_str.empty()) {
                    // now we know how much content the user is going to send,
                    // so we just create a buffer with that size
                    auto content_length = to_uint(content_length_str);

                    body_cache.resize(content_length);
                    // todo: see if there's a better way to do this
                    stl::cin.rdbuf()->pubsetbuf(body_cache.data(), content_length);
                } else {
                    // we don't know how much the user is going to send. so we use a small size buffer:

                    // TODO: add something here
                }
            }
            return body_cache;
        }
    };

} // namespace webpp::http

#endif // WEBPP_CGI_REQUEST_HPP
