#ifndef WEBPP_CGI_H
#define WEBPP_CGI_H

#include "../../convert/casts.hpp"
#include "../../std/string_view.hpp"
#include "../../strings/to_case.hpp"
#include "../../traits/std_traits.hpp"
#include "../application.hpp"
#include "../request.hpp"
#include "../response.hpp"
#include "./cgi_request.hpp"

#include <cctype>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <sstream>

// TODO: use GetEnvironmentVariableA for Windows operating system
#include <unistd.h> // for environ

namespace webpp {

    // todo: add protocol extensions as well
    template <Traits TraitsType, Application App, ExtensionList EList = empty_extension_pack>
    struct cgi : public enable_traits<TraitsType> {
      public:
        using traits_type      = TraitsType;
        using application_type = App;
        using extension_list   = stl::remove_cvref_t<EList>;
        using protocol_type            = cgi<traits_type, application_type, extension_list>;
        using str_view_type    = typename TraitsType::string_view_type;
        using str_type         = typename TraitsType::string_type;
        using ostream_t        = typename TraitsType::ostream_type;
        using request_type     = simple_request<traits_type, cgi_request, protocol_type, extension_list>;
        using allocator_type   = typename request_type::allocator_type;
        using logger_type      = typename traits_type::logger_type;
        using logger_ref       = typename logger_type::logger_ref;
        using etraits          = enable_traits<traits_type>;
        using app_wrapper_type         = http_app_wrapper<traits_type, application_type>;

      private:
        void ctor() noexcept {
            // I'm not using C here; so why should I pay for it!
            // And also the user should not use cin and cout. so ...
            stl::ios::sync_with_stdio(false);
        }

      public:
        app_wrapper_type app;

        template <typename AllocType = allocator_type>
        cgi(logger_ref logger = logger_type{}, AllocType const& alloc = AllocType{})
          : etraits{logger, alloc},
            app{logger, alloc} {
            ctor();
        }

        /**
         * Read the body of the string
         */
        static stl::streamsize read(char* data, stl::streamsize length) noexcept {
            stl::cin.read(data, length);
            return stl::cin.gcount();
        }

        /**
         * Send the stream to the user
         * @param stream
         */
        static void write(ostream_t& stream) noexcept {
            // TODO: check if you need to ignore the input or not

            // I think o-stream is not readable so we cannot do this:
            // https://stackoverflow.com/questions/15629886/how-to-write-ostringstream-directly-to-cout
            stl::cout << stream.rdbuf(); // TODO: test this, I don't trust myself :)
        }

        /**
         * Send data to the user
         */
        static void write(char const* data, stl::streamsize length) noexcept {
            stl::cout.write(data, length);
        }

        /**
         * Get the environment value safely
         */
        [[nodiscard]] static stl::string_view env(char const* key) noexcept {
            if (auto value = getenv(key))
                return value;
            return {};
        }

        /**
         * Get a specific header by it's name
         */
        [[nodiscard]] static str_view_type header(stl::string name) noexcept {
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
         * Get a list of headers as a string
         */
        [[nodiscard]] static str_view_type headers() noexcept {
            // we can do this only in CGI, we have to come up with new ways for long-running protocols:
            static str_type headers_cache;
            if (headers_cache.empty()) {
                // TODO: this code won't work on windows. Change when you are worried about windows
                for (auto it = ::environ; *it; it++) {
                    str_view_type h{*it};
                    if (starts_with(h, "HTTP_")) {
                        headers_cache.append(h.substr(5));
                        // FIXME: decide if you need to convert _ to - or not.
                    }
                }
            }
            return headers_cache;
        }

        /**
         * Get the full body as a string_view
         */
        [[nodiscard]] static str_view_type body() noexcept {
            // again, we can do this only in cgi protocol not in other interfaces:
            static str_type body_cache;
            if (body_cache.empty()) {
                if (auto content_length_str = env("CONTENT_LENGTH"); !content_length_str.empty()) {
                    // now we know how much content the user is going to send
                    // so we just create a buffer with that size
                    auto content_length = to_uint(content_length_str);

                    char* buffer = new char[content_length];
                    stl::cin.rdbuf()->pubsetbuf(buffer, sizeof(buffer));
                } else {
                    // we don't know how much the user is going to send. so we use a small size buffer:

                    // TODO: add something here
                }
            }
            return body_cache;
        }


        void operator()() noexcept {
            auto req = etraits::template instantiate<request_type>();
            auto         res = app(req);
            res.calculate_default_headers();
            auto header_str = res.headers.str();
            auto str        = res.body.str();

            // From RFC: https://tools.ietf.org/html/rfc3875
            // Send status code:
            // Status         = "Status:" status-code SP reason-phrase NL
            // status-code    = "200" | "302" | "400" | "501" | extension-code
            // extension-code = 3digit
            // reason-phrase  = *TEXT

            // todo: use <format> or {fmt}
            // todo: give the user the ability to change the status phrase
            stl::stringstream status_line;
            status_line << "Status: " << res.headers.status_code << " "
                        << status_reason_phrase(res.headers.status_code) << "\r\n";

            auto _status_line_str = status_line.str();
            write(_status_line_str.data(), _status_line_str.size());

            write(header_str.data(), header_str.size());
            write("\r\n", 2);
            write(str.data(), str.size());
        }
    };

    // fixme: implement these too:
    //    AUTH_PASSWORD
    //    AUTH_TYPE
    //    AUTH_USER
    //    CERT_COOKIE
    //    CERT_FLAGS
    //    CERT_ISSUER
    //    CERT_KEYSIZE
    //    CERT_SECRETKEYSIZE
    //    CERT_SERIALNUMBER
    //    CERT_SERVER_ISSUER
    //    CERT_SERVER_SUBJECT
    //    CERT_SUBJECT
    //    CF_TEMPLATE_PATH
    //    CONTENT_LENGTH
    //    CONTENT_TYPE
    //    CONTEXT_PATH
    //    GATEWAY_INTERFACE
    //    HTTPS
    //    HTTPS_KEYSIZE
    //    HTTPS_SECRETKEYSIZE
    //    HTTPS_SERVER_ISSUER
    //    HTTPS_SERVER_SUBJECT
    //    HTTP_ACCEPT
    //    HTTP_ACCEPT_ENCODING
    //    HTTP_ACCEPT_LANGUAGE
    //    HTTP_CONNECTION
    //    HTTP_COOKIE
    //    HTTP_HOST
    //    HTTP_REFERER
    //    HTTP_USER_AGENT
    //    QUERY_STRING
    //    REMOTE_ADDR
    //    REMOTE_HOST
    //    REMOTE_USER
    //    REQUEST_METHOD
    //    SCRIPT_NAME
    //    SERVER_NAME
    //    SERVER_PORT
    //    SERVER_PORT_SECURE
    //    SERVER_PROTOCOL
    //    SERVER_SOFTWARE
    //    WEB_SERVER_API


    // void cgi::run(router_t<cgi>& _router) noexcept {
    //    auto self = std::make_shared<cgi>(this);
    //    webpp::request<webpp::cgi> req(self);
    //    auto res = _router.run(req);
    //    std::ios_base::sync_with_stdio(false); // TODO: write tests for this
    //    part for (auto const& [attr, value] : res.headers()) {
    //        std::cout << attr << ": " << value << "\r\n";
    //    }
    //    std::cout << "\r\n" << res.body();
    //}


} // namespace webpp

#endif // WEBPP_CGI_H
