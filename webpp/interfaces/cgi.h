#ifndef WEBPP_CGI_H
#define WEBPP_CGI_H

#include "../http/body.h"
#include "../http/header.h"
#include "../router.h"
#include "../server.h"
#include "../std/string_view.h"
#include "basic_interface.h"
#include <string>

namespace webpp {

    class cgi : public basic_interface {
      public:
        cgi() = default;
        //        char const* header(std::string h) const noexcept;
        //        header_type headers() noexcept;
        //        body_type body() noexcept;
        //        char const* server_addr() const noexcept;
        //        int server_port() const noexcept;
        //        char const* remote_addr() const noexcept;
        //        int remote_port() const noexcept;
        //        char const* server_name() const noexcept;
        //        char const* scheme() const noexcept;
        //        char const* server_protcol() const noexcept;
        //        char const* method() const noexcept;
        //        char const* request_uri() const noexcept;

        //        char const* env(std::string_view const& name) const noexcept;
        std::streamsize read(char* data, std::streamsize length) const;
        void write(std::ostream& stream);
        void write(char const* data, std::streamsize length);

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

        [[nodiscard]] std::string_view method() const noexcept {
            return cgi::env("REQUEST_METHOD");
        }

        /**
         * @brief returns the request scheme (http/https/...)
         */
        [[nodiscard]] std::string_view scheme() const noexcept {
            throw std::logic_error(
                "This method should be specialized by all the interfaces");
        }

        /**
         * @brief returns something like "HTTP/1.1" or ...
         */
        [[nodiscard]] std::string_view server_protocol() const noexcept {
            throw std::logic_error(
                "This method should be specialized by all the interfaces");
        }

        [[nodiscard]] std::string_view query() const noexcept {
            throw std::logic_error(
                "This method should be specialized by all the interfaces");
        }

        [[nodiscard]] std::string_view remote_port() const noexcept {
            throw std::logic_error(
                "This method should be specialized by all the interfaces");
        }

        [[nodiscard]] std::string_view server_port() const noexcept {
            throw std::logic_error(
                "This method should be specialized by all the interfaces");
        }

        [[nodiscard]] std::string_view server_addr() const noexcept {
            throw std::logic_error(
                "This method should be specialized by all the interfaces");
        }

        [[nodiscard]] std::string_view remote_addr() const noexcept {
            throw std::logic_error(
                "This method should be specialized by all the interfaces");
        }

        [[nodiscard]] std::string_view server_name() const noexcept {
            throw std::logic_error(
                "This method should be specialized by all the interfaces");
        }

        [[nodiscard]] std::string_view request_uri() const noexcept {
            throw std::logic_error(
                "This method should be specialized by all the interfaces");
        }

        [[nodiscard]] std::string_view
        header(std::string_view const& name) const noexcept {
            throw std::logic_error(
                "This method should be specialized by all the interfaces");
        }

        [[nodiscard]] std::string_view headers() const noexcept {
            throw std::logic_error(
                "This method should be specialized by all the interfaces");
        }

        [[nodiscard]] std::string_view body() const noexcept {
            throw std::logic_error(
                "This method should be specialized by all the interfaces");
        }
    };

} // namespace webpp

#endif // WEBPP_CGI_H
