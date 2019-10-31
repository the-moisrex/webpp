#ifndef WEBPP_CGI_H
#define WEBPP_CGI_H

#include "../http/body.h"
#include "../http/header.h"
#include "../router.h"
#include "../server.h"
#include "../std/string_view.h"
#include "basic_interface.h"
#include <iostream>
#include <string>

namespace webpp {

    struct cgi : public basic_interface {

        /**
         * Read the body of the string
         * @param data
         * @param length
         * @return
         */
        static std::streamsize read(char* data,
                                    std::streamsize length) noexcept {
            std::cin.read(data, length);
            return std::cin.gcount();
        }

        static void write(std::ostream& stream) noexcept {
            // I think o-stream is not readable so we cannot do this:
            // https://stackoverflow.com/questions/15629886/how-to-write-ostringstream-directly-to-cout
            std::cout
                << stream.rdbuf(); // TODO: test this, I don't trust myself :)
        }

        /**
         * Send data to the user
         * @param data
         * @param length
         */
        static void write(char const* data, std::streamsize length) noexcept {
            std::cout.write(data, length);
        }

        /**
         * Get the environment value safely
         * @param key
         * @return
         */
        static std::string_view env(char const* const key) noexcept {
            if (auto value = getenv(key))
                return value;
            return {};
        }
    };

    template <>
    class server<cgi> {};

    template <>
    class request_t<cgi> : basic_request_t {

        /**
         * @brief Get the method
         */
        [[nodiscard]] std::string_view method() const noexcept {
            return cgi::env("REQUEST_METHOD");
        }

        /**
         * @brief returns the request scheme (http/https/...)
         */
        [[nodiscard]] std::string_view request_scheme() const noexcept {
            return cgi::env("REQUEST_SCHEME");
        }

        /**
         * @brief returns something like "HTTP/1.1" or ...
         */
        [[nodiscard]] std::string_view server_protocol() const noexcept {
            return cgi::env("SERVER_PROTOCOL");
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
         * @brief get the server name
         */
        [[nodiscard]] std::string_view server_name() const noexcept {
            return cgi::env("SERVER_NAME");
        }

        /**
         * @brief get the request uri
         */
        [[nodiscard]] std::string_view request_uri() const noexcept {
            return cgi::env("REQUEST_URI");
        }

        /**
         * @brief get a single header
         * @param name
         */
        [[nodiscard]] std::string_view
        header(std::string_view const& name) const noexcept {
            throw std::logic_error(
                "This method should be specialized by all the interfaces");
        }

        /**
         * @brief get all of the headers as a string_view
         * @details this method in CGI interfaces will have to do more than
         * in the our own version because data has already been parsed by the
         * CGI server; and we have to recreate it based on the environment
         * variables.
         */
        [[nodiscard]] std::string_view headers() const noexcept {
            throw std::logic_error(
                "This method should be specialized by all the interfaces");
        }

        /**
         * @brief get the whole body as a string_view
         * @details this method will return raw string values of the body of
         * the request and will not parse it. Parsing it is another methods'
         * problem that might even use this function as the source.
         */
        [[nodiscard]] std::string_view body() const noexcept {
            throw std::logic_error(
                "This method should be specialized by all the interfaces");
        }
    };

} // namespace webpp

#endif // WEBPP_CGI_H
