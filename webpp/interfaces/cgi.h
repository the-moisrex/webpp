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
         * @example SERVER_SOFTWARE=Apache/2.4.41 (Unix) OpenSSL/1.1.1d
         */
        [[nodiscard]] std::string_view server_software() const noexcept {
            return cgi::env("SERVER_SOFTWARE");
        }

        /**
         * @brief get the server name
         * @example SERVER_NAME=localhost
         */
        [[nodiscard]] std::string_view server_name() const noexcept {
            return cgi::env("SERVER_NAME");
        }

        /**
         * @brief get the gateway interface environment variable
         * @example GATEWAY_INTERFACE=CGI/1.1
         * @return
         */
        [[nodiscard]] std::string_view gateway_interface() const noexcept {
            return cgi::env("GATEWAY_INTERFACE");
        }

        /**
         * @brief get the server protocol
         * @example SERVER_PROTOCOL=HTTP/1.1
         */
        [[nodiscard]] std::string_view server_protocol() const noexcept {
            return cgi::env("SERVER_PROTOCOL");
        }

        /**
         * @brief Get the method
         */
        [[nodiscard]] std::string_view request_method() const noexcept {
            return cgi::env("REQUEST_METHOD");
        }

        /**
         * @brief get the path info
         * @example PATH_INFO=/hello/world
         */
        [[nodiscard]] std::string_view path_info() const noexcept {
            return cgi::env("PATH_INFO");
        }

        /**
         * @brief returns the request scheme (http/https/...)
         */
        [[nodiscard]] std::string_view request_scheme() const noexcept {
            return cgi::env("REQUEST_SCHEME");
        }


        /**
         * @brief get the query string
         */
        [[nodiscard]] std::string_view query() const noexcept {
            return cgi::env("QUERY_STRING");
        }

        /**
         * @brief get the user's port number
         */
        [[nodiscard]] std::string_view remote_port() const noexcept {
            return cgi::env("REMOTE_PORT");
        }

        /**
         * @brief get the port that the server is listening on
         */
        [[nodiscard]] std::string_view server_port() const noexcept {
            return cgi::env("SERVER_PORT");
        }

        /**
         * @brief get the ip address that the server is listening on
         */
        [[nodiscard]] std::string_view server_addr() const noexcept {
            return cgi::env("SERVER_ADDR");
        }

        /**
         * @brief get the ip address of the user
         */
        [[nodiscard]] std::string_view remote_addr() const noexcept {
            return cgi::env("REMOTE_ADDR");
        }


        /**
         * @brief get the request uri
         */
        [[nodiscard]] std::string_view request_uri() const noexcept {
            return cgi::env("REQUEST_URI");
        }

        /**
         * @brief get the content_type
         */
        [[nodiscard]] std::string_view content_type() const noexcept {
            return cgi::env("CONTENT_LENGTH");
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
