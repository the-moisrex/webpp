#ifndef WEBPP_PROTOCOLS_CGI_HPP
#define WEBPP_PROTOCOLS_CGI_HPP

#include "../convert/casts.hpp"
#include "../http/protocol/common_http_protocol.hpp"
#include "../http/request.hpp"
#include "../http/request_body.hpp"
#include "../http/response.hpp"
#include "../std/string_view.hpp"
#include "../traits/default_traits.hpp"
#include "../traits/enable_traits.hpp"
#include "./cgi_request.hpp"
#include "./cgi_request_body_communicator.hpp"

#include <iostream>

namespace webpp::http {

    template <Application App, Traits TraitsType = default_traits>
    struct cgi : common_http_protocol<TraitsType, App> {
        using traits_type               = TraitsType;
        using etraits                   = enable_owner_traits<traits_type>;
        using application_type          = App;
        using protocol_type             = cgi;
        using string_view_type          = traits::string_view<traits_type>;
        using char_type                 = traits::char_type<traits_type>;
        using string_type               = traits::string<traits_type>;
        using general_allocator_type    = traits::allocator_type_of<traits_type, char_type>;
        using common_protocol_type      = common_http_protocol<TraitsType, App>;
        using app_wrapper_type          = typename common_protocol_type::app_wrapper_type;
        using request_body_communicator = cgi_proto::cgi_request_body_communicator<protocol_type>;

        using fields_provider      = header_fields_provider<header_field_of<traits_type>>;
        using request_headers_type = request_headers<fields_provider>;
        using request_body_type    = request_body<traits_type, request_body_communicator>;
        using request_type         = simple_request<cgi_request, request_headers_type, request_body_type>;
        using response_type        = simple_response<traits_type>;


        static_assert(HTTPRequest<request_type>,
                      "Web++ Internal Bug: request_type is not a match for Request concept.");

        static_assert(ApplicationAcceptingRequest<app_wrapper_type, request_type>,
                      "Your application type can't be called with a request type of our choosing or "
                      "its response is not of a valid response type.");

        static_assert(AllocatorHolder<common_protocol_type>, "No allocator is available?");

      private:
        using super = common_http_protocol<TraitsType, App>;

        void ctor() noexcept {
            // I'm not using C here; so why should I pay for it!
            // And also the user should not use cin and cout. so ...
            stl::ios::sync_with_stdio(false);

            // The next line is generally not required because in this case the removal of synchronization is
            // generally enough to get the big speedup, but the next line removes the synchronization between
            // the C++ input and output buffers which gives a slight more speedup in most cases.
            stl::cin.tie(nullptr);
        }

      public:
        template <typename... Args>
        cgi(Args&&... args) : super{stl::forward<Args>(args)...} {
            ctor();
        }

        /**
         * Read the body of the string
         */
        static stl::streamsize read(char* data, stl::streamsize length) {
            stl::cin.read(data, length);
            return stl::cin.gcount();
        }

        static stl::streamsize read(char* data) {
            // A good article for fast reads:
            // https://ayandas.me/blog-tut/2019/04/06/speeding-up-iostreams-in-c++.html

            stl::streamsize read_count = 0;
            for (;;) {
                stl::cin.read(data, default_buffer_size);

                stl::streamsize const buf_read_size = stl::cin.gcount();
                if (buf_read_size == 0) {
                    break;
                }
                read_count += buf_read_size;
            }
            return read_count;
        }

        /**
         * Send the stream to the user
         * @param stream
         */
        static void write(auto& stream) noexcept {
            // TODO: check if you need to ignore the input or not

            // I think o-stream is not readable, so we cannot do this:
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
        [[nodiscard]] static inline stl::string_view env(char const* key) noexcept {
            if (auto const value = getenv(key)) {
                return value;
            }
            return {};
        }

      private:
        template <typename BodyType>
        inline void write_text(BodyType& body) {
            write(body.data(), static_cast<stl::streamsize>(body.size()));
        }

        template <typename BodyType>
        inline void write_cstream(BodyType& body) {
            using body_type         = stl::remove_cvref_t<BodyType>;
            using cstream_byte_type = typename body_type::byte_type;

            // NOLINTBEGIN(cppcoreguidelines-pro-type-reinterpret-cast)
            stl::array<char_type, default_buffer_size> buf;
            while (stl::streamsize read_size = body.read(reinterpret_cast<cstream_byte_type*>(buf.data()),
                                                         static_cast<stl::streamsize>(buf.size())))
            {
                write(buf.data(), read_size);
            }
            // NOLINTEND(cppcoreguidelines-pro-type-reinterpret-cast)
        }

        template <typename BodyType>
        inline void write_response_body(BodyType& body) {
            using body_type = stl::remove_cvref_t<BodyType>;
            if constexpr (UnifiedBodyReader<body_type>) {
                switch (body.which_communicator()) {
                    using enum communicator_type;
                    case nothing: {
                        break;
                    }
                    case cstream_based: {
                        write_cstream(body);
                        break;
                    }
                    case stream_based: {
                        write(body);
                        break;
                    }
                    case text_based: {
                        write_text(body);
                        break;
                    }
                }
            } else if constexpr (TextBasedBodyReader<body_type>) {
                write_text(body);
            } else if constexpr (CStreamBasedBodyReader<body_type>) {
                write_cstream(body);
            } else if constexpr (StreamBasedBodyReader<body_type>) {
                write(body);
            } else {
                static_assert_false(body_type,
                                    "We don't know how to write the response body to output"
                                    " in CGI protocol. Application returns a wrong "
                                    "response type which contains unknown body.");
            }
        }

      public:
        int operator()() noexcept {
            try {
                HTTPResponse auto res = this->app(request_type{*this});
                res.calculate_default_headers();
                string_type header_str{general_alloc_for<string_type>(*this)};
                res.headers.string_to(header_str);

                // From RFC: https://tools.ietf.org/html/rfc3875
                // Send status code:
                // Status         = "Status:" status-code SP reason-phrase NL
                // status-code    = "200" | "302" | "400" | "501" | extension-code
                // extension-code = 3digit
                // reason-phrase  = *TEXT

                // todo: give the user the ability to change the status phrase
                auto status_line = object::make_local<string_type>(*this);
                fmt::format_to(stl::back_inserter(status_line),
                               "Status: {} {}\r\n",
                               res.headers.status_code_integer(),
                               http::status_code_reason_phrase(res.headers.status_code()));
                write(status_line.data(), static_cast<stl::streamsize>(status_line.size()));

                write(header_str.data(), static_cast<stl::streamsize>(header_str.size()));
                write("\r\n", 2L);
                write_response_body(res.body);
                return EXIT_SUCCESS;
            } catch (stl::exception const& ex) {
                this->logger.error("CGI", "Fatal exception is thrown.", ex);
                return EXIT_FAILURE;
            } catch (...) {
                this->logger.error("CGI", "Fatal and unknown exception is thrown.");
                return EXIT_FAILURE;
            }
        }
    };

    template <typename App>
    cgi(App&&) -> cgi<App, default_traits>;

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


} // namespace webpp::http

#endif // WEBPP_PROTOCOLS_CGI_HPP
