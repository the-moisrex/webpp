#ifndef WEBPP_CGI_H
#define WEBPP_CGI_H

#include "../server.h"
#include "basic_interface.h"
#include <string_view>

namespace webpp {

    struct cgi : public basic_interface {

        /**
         * Read the body of the string
         * @param data
         * @param length
         * @return
         */
        static std::streamsize read(char* data,
                                    std::streamsize length) noexcept;

        /**
         * Send the stream to the user
         * @param stream
         */
        static void write(std::ostream& stream) noexcept;

        /**
         * Send data to the user
         * @param data
         * @param length
         */
        static void write(char const* data, std::streamsize length) noexcept;

        /**
         * Get the environment value safely
         * @param key
         * @return
         */
        static std::string_view env(char const* key) noexcept;
    };

    template <>
    class server<cgi> {};

    template <>
    class request_t<cgi> : basic_request_t {

        /**
         * @brief get the server's software
         * @details Name and version of the information server software
         * answering the request (and running the gateway). Format:
         * name/version.
         * @example SERVER_SOFTWARE=Apache/2.4.41 (Unix) OpenSSL/1.1.1d
         */
        [[nodiscard]] std::string_view server_software() const noexcept {
            return cgi::env("SERVER_SOFTWARE");
        }

        /**
         * @brief get the server name
         * @details Server's hostname, DNS alias, or IP address as it appears in
         * self-referencing URLs.
         * @example SERVER_NAME=localhost
         */
        [[nodiscard]] std::string_view server_name() const noexcept {
            return cgi::env("SERVER_NAME");
        }

        /**
         * @brief get the gateway interface environment variable
         * @details CGI specification revision with which this server complies.
         * Format: CGI/revision.
         * @example GATEWAY_INTERFACE=CGI/1.1
         */
        [[nodiscard]] std::string_view gateway_interface() const noexcept {
            return cgi::env("GATEWAY_INTERFACE");
        }

        /**
         * @brief get the server protocol
         * @details Name and revision of the information protocol this request
         * came in with. Format: protocol/revision.
         * @example SERVER_PROTOCOL=HTTP/1.1
         */
        [[nodiscard]] std::string_view server_protocol() const noexcept {
            return cgi::env("SERVER_PROTOCOL");
        }

        /**
         * @brief get the port that the server is listening on
         * @details Port number to which the request was sent.
         */
        [[nodiscard]] std::string_view server_port() const noexcept {
            return cgi::env("SERVER_PORT");
        }

        /**
         * @brief Get the method
         * @details Method with which the request was made. For HTTP, this is
         * Get, Head, Post, and so on.
         */
        [[nodiscard]] std::string_view request_method() const noexcept {
            return cgi::env("REQUEST_METHOD");
        }

        /**
         * @brief get the path info
         * @details Extra path information, as given by the client. Scripts can
         * be accessed by their virtual pathname, followed by extra information
         * at the end of this path. The extra information is sent as PATH_INFO.
         * @example PATH_INFO=/hello/world
         */
        [[nodiscard]] std::string_view path_info() const noexcept {
            return cgi::env("PATH_INFO");
        }

        /**
         * @brief get the path translated
         * @details Translated version of PATH_INFO after any
         * virtual-to-physical mapping.
         * @example PATH_TRANSLATED=/srv/http/hello/world
         */
        [[nodiscard]] std::string_view path_translated() const noexcept {
            return cgi::env("PATH_TRANSLATED");
        }

        /**
         * @brief get the script name
         * @details Virtual path to the script that is executing; used for
         * self-referencing URLs.
         * @example SCRIPT_NAME=/cgi-bin/one.cgi
         */
        [[nodiscard]] std::string_view script_name() const noexcept {
            return cgi::env("SCRIPT_NAME");
        }

        /**
         * @brief get the query string
         * @details Query information that follows the ? in the URL that
         * referenced this script.
         */
        [[nodiscard]] std::string_view query_string() const noexcept {
            return cgi::env("QUERY_STRING");
        }

        /**
         * @brief get the remote host
         * @details Hostname making the request. If the server does not have
         * this information, it sets REMOTE_ADDR and does not set REMOTE_HOST.
         */
        [[nodiscard]] std::string_view remote_host() const noexcept {
            return cgi::env("REMOTE_HOST");
        }

        /**
         * @brief get the ip address of the user
         * @details IP address of the remote host making the request.
         */
        [[nodiscard]] std::string_view remote_addr() const noexcept {
            return cgi::env("REMOTE_ADDR");
        }

        /**
         * @brief get the auth type
         * @details If the server supports user authentication, and the script
         * is protected, the protocol-specific authentication method used to
         * validate the user.
         */
        [[nodiscard]] std::string_view auth_type() const noexcept {
            return cgi::env("AUTH_TYPE");
        }

        /**
         * @brief get the remote user or auth user value (both should be the
         * same)
         * @details If the server supports user authentication, and the script
         * is protected, the username the user has authenticated as. (Also
         * available as AUTH_USER.)
         */
        [[nodiscard]] std::string_view remote_user() const noexcept {
            if (auto a = cgi::env("REMOTE_USER"); !a.empty())
                return a;
            return cgi::env("AUTH_USER");
        }

        /**
         * @brief get the remote user or auth user value (both should be the
         * same)
         * @details If the server supports user authentication, and the script
         * is protected, the username the user has authenticated as. (Also
         * available as AUTH_USER.)
         */
        [[nodiscard]] std::string_view auth_user() const noexcept {
            if (auto a = cgi::env("AUTH_USER"); !a.empty())
                return a;
            return cgi::env("REMOTE_USER");
        }

        /**
         * @brief get the remote ident
         * @details If the HTTP server supports RFC 931 identification, this
         * variable is set to the remote username retrieved from the server. Use
         * this variable for logging only.
         */
        [[nodiscard]] std::string_view remote_ident() const noexcept {
            return cgi::env("REMOTE_IDENT");
        }

        /**
         * @brief returns the request scheme (http/https/...)
         */
        [[nodiscard]] std::string_view request_scheme() const noexcept {
            return cgi::env("REQUEST_SCHEME");
        }

        /**
         * @brief get the user's port number
         */
        [[nodiscard]] std::string_view remote_port() const noexcept {
            return cgi::env("REMOTE_PORT");
        }

        /**
         * @brief get the ip address that the server is listening on
         */
        [[nodiscard]] std::string_view server_addr() const noexcept {
            return cgi::env("SERVER_ADDR");
        }

        /**
         * @brief get the request uri
         */
        [[nodiscard]] std::string_view request_uri() const noexcept {
            return cgi::env("REQUEST_URI");
        }

        /**
         * @brief get the content_type
         * @details For queries that have attached information, such as HTTP
         * POST and PUT, this is the content type of the data.
         */
        [[nodiscard]] std::string_view content_type() const noexcept {
            return cgi::env("CONTENT_LENGTH");
        }

        /**
         * @brief get the content length
         * @details Length of the content as given by the client.
         */
        [[nodiscard]] std::string_view content_length() const noexcept {
            return cgi::env("CONTENT_LENGTH");
        }

        /**
         * @brief get the document root environment value
         */
        [[nodiscard]] std::string_view document_root() const noexcept {
            return cgi::env("DOCUMENT_ROOT");
        }

        /**
         * @brief get the https environment value
         * @return "on" if the user used HTTPS protocol
         */
        [[nodiscard]] std::string_view https() const noexcept {
            return cgi::env("HTTPS");
        }

        /**
         * @brief get a single header
         * @param name
         */
        [[nodiscard]] std::string_view
        header(std::string_view const& name) const noexcept {
            // TODO
        }

        /**
         * @brief get all of the headers as a string_view
         * @details this method in CGI interfaces will have to do more than
         * in the our own version because data has already been parsed by the
         * CGI server; and we have to recreate it based on the environment
         * variables.
         */
        [[nodiscard]] std::string_view headers() const noexcept {
            // todo
        }

        /**
         * @brief get the whole body as a string_view
         * @details this method will return raw string values of the body of
         * the request and will not parse it. Parsing it is another methods'
         * problem that might even use this function as the source.
         */
        [[nodiscard]] std::string_view body() const noexcept {
            // todo
        }
    };

} // namespace webpp

#endif // WEBPP_CGI_H
