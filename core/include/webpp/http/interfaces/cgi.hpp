#ifndef WEBPP_CGI_H
#define WEBPP_CGI_H

#include "../../std/string_view.hpp"
#include "../../traits/std_traits.hpp"
#include "../../utils/casts.hpp"
#include "../../utils/strings.hpp"
#include "../application.hpp"
#include "../request.hpp"
#include "../response.hpp"
#include "../routes/router.hpp"

#include <cctype>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <sstream>

// TODO: use GetEnvironmentVariableA for Windows operating system
#include <unistd.h> // for environ

namespace webpp {

    /**
     * Specializing the request_t<cgi> methods so the user can use them.
     * The request_t class is one of the important classes which that means It's
     * going to be used a lot so, we will do everything (even if it means we
     * have to copy and paste lots of codes to make it happen) to make sure
     * the user is able to use this class properly and easily.
     */
    template <Traits TraitsType, typename /* fixme: RequestExtensionList */ REL, Interface IfaceType>
    struct cgi_request : public REL, public enable_traits<TraitsType> {
        using traits_type            = TraitsType;
        using interface_type         = IfaceType;
        using request_extension_list = REL;
        using allocator_type   = typename traits_type::template allocator<typename traits_type::char_type>;
        using application_type = typename interface_type::application_type;
        using logger_type      = typename traits_type::logger_type;
        using logger_ref       = typename logger_type::logger_ref;
        using etraits          = enable_traits<traits_type>;


        template <typename AllocType>
        cgi_request(logger_ref logger = logger_type{}, auto const& alloc = AllocType{}) noexcept
          : etraits(logger, alloc) {}

        /**
         * @brief get the server's software
         * @details Name and version of the information server software
         * answering the request (and running the gateway). Format:
         * name/version.
         * @example SERVER_SOFTWARE=Apache/2.4.41 (Unix) OpenSSL/1.1.1d
         */
        [[nodiscard]] stl::string_view server_software() const noexcept {
            return interface_type::env("SERVER_SOFTWARE");
        }

        /**
         * @brief get the server name
         * @details Server's hostname, DNS alias, or IP address as it appears in
         * self-referencing URLs.
         * @example SERVER_NAME=localhost
         */
        [[nodiscard]] stl::string_view server_name() const noexcept {
            return interface_type::env("SERVER_NAME");
        }

        /**
         * @brief get the gateway interface environment variable
         * @details CGI specification revision with which this server complies.
         * Format: CGI/revision.
         * @example GATEWAY_INTERFACE=CGI/1.1
         */
        [[nodiscard]] stl::string_view gateway_interface() const noexcept {
            return interface_type::env("GATEWAY_INTERFACE");
        }

        /**
         * @brief get the server protocol
         * @details Name and revision of the information protocol this request
         * came in with. Format: protocol/revision.
         * @example SERVER_PROTOCOL=HTTP/1.1
         */
        [[nodiscard]] stl::string_view server_protocol() const noexcept {
            return interface_type::env("SERVER_PROTOCOL");
        }

        /**
         * @brief get the port that the server is listening on
         * @details Port number to which the request was sent.
         */
        [[nodiscard]] stl::string_view server_port() const noexcept {
            return interface_type::env("SERVER_PORT");
        }

        /**
         * @brief Get the method
         * @details Method with which the request was made. For HTTP, this is
         * Get, Head, Post, and so on.
         */
        [[nodiscard]] stl::string_view request_method() const noexcept {
            return interface_type::env("REQUEST_METHOD");
        }

        /**
         * @brief get the path info
         * @details Extra path information, as given by the client. Scripts can
         * be accessed by their virtual pathname, followed by extra information
         * at the end of this path. The extra information is sent as PATH_INFO.
         * @example PATH_INFO=/hello/world
         */
        [[nodiscard]] stl::string_view path_info() const noexcept {
            return interface_type::env("PATH_INFO");
        }

        /**
         * @brief get the path translated
         * @details Translated version of PATH_INFO after any
         * virtual-to-physical mapping.
         * @example PATH_TRANSLATED=/srv/http/hello/world
         */
        [[nodiscard]] stl::string_view path_translated() const noexcept {
            return interface_type::env("PATH_TRANSLATED");
        }

        /**
         * @brief get the script name
         * @details Virtual path to the script that is executing; used for
         * self-referencing URLs.
         * @example SCRIPT_NAME=/cgi-bin/one.cgi
         */
        [[nodiscard]] stl::string_view script_name() const noexcept {
            return interface_type::env("SCRIPT_NAME");
        }

        /**
         * @brief get the query string
         * @details Query information that follows the ? in the URL that
         * referenced this script.
         */
        [[nodiscard]] stl::string_view query_string() const noexcept {
            return interface_type::env("QUERY_STRING");
        }

        /**
         * @brief get the remote host
         * @details Hostname making the request. If the server does not have
         * this information, it sets REMOTE_ADDR and does not set REMOTE_HOST.
         */
        [[nodiscard]] stl::string_view remote_host() const noexcept {
            return interface_type::env("REMOTE_HOST");
        }

        /**
         * @brief get the ip address of the user
         * @details IP address of the remote host making the request.
         */
        [[nodiscard]] stl::string_view remote_addr() const noexcept {
            return interface_type::env("REMOTE_ADDR");
        }

        /**
         * @brief get the auth type
         * @details If the server supports user authentication, and the script
         * is protected, the protocol-specific authentication method used to
         * validate the user.
         */
        [[nodiscard]] stl::string_view auth_type() const noexcept {
            return interface_type::env("AUTH_TYPE");
        }

        /**
         * @brief get the remote user or auth user value (both should be the
         * same)
         * @details If the server supports user authentication, and the script
         * is protected, the username the user has authenticated as. (Also
         * available as AUTH_USER.)
         */
        [[nodiscard]] stl::string_view remote_user() const noexcept {
            if (auto a = interface_type::env("REMOTE_USER"); !a.empty())
                return a;
            return interface_type::env("AUTH_USER");
        }

        /**
         * @brief get the remote user or auth user value (both should be the
         * same)
         * @details If the server supports user authentication, and the script
         * is protected, the username the user has authenticated as. (Also
         * available as AUTH_USER.)
         */
        [[nodiscard]] stl::string_view auth_user() const noexcept {
            if (auto a = interface_type::env("AUTH_USER"); !a.empty())
                return a;
            return interface_type::env("REMOTE_USER");
        }

        /**
         * @brief get the remote ident
         * @details If the HTTP server supports RFC 931 identification, this
         * variable is set to the remote username retrieved from the server. Use
         * this variable for logging only.
         */
        [[nodiscard]] stl::string_view remote_ident() const noexcept {
            return interface_type::env("REMOTE_IDENT");
        }

        /**
         * @brief returns the request scheme (http/https/...)
         */
        [[nodiscard]] stl::string_view request_scheme() const noexcept {
            return interface_type::env("REQUEST_SCHEME");
        }

        /**
         * @brief get the user's port number
         */
        [[nodiscard]] stl::string_view remote_port() const noexcept {
            return interface_type::env("REMOTE_PORT");
        }

        /**
         * @brief get the ip address that the server is listening on
         */
        [[nodiscard]] stl::string_view server_addr() const noexcept {
            return interface_type::env("SERVER_ADDR");
        }

        /**
         * @brief get the request uri
         */
        [[nodiscard]] stl::string_view request_uri() const noexcept {
            return interface_type::env("REQUEST_URI");
        }

        /**
         * @brief get the content_type
         * @details For queries that have attached information, such as HTTP
         * POST and PUT, this is the content type of the data.
         */
        [[nodiscard]] stl::string_view content_type() const noexcept {
            return interface_type::env("CONTENT_LENGTH");
        }

        /**
         * @brief get the content length
         * @details Length of the content as given by the client.
         */
        [[nodiscard]] stl::string_view content_length() const noexcept {
            return interface_type::env("CONTENT_LENGTH");
        }

        /**
         * @brief get the document root environment value
         * @details The root directory of your server
         */
        [[nodiscard]] stl::string_view document_root() const noexcept {
            return interface_type::env("DOCUMENT_ROOT");
        }

        /**
         * @brief get the https environment value
         * @return "on" if the user used HTTPS protocol
         */
        [[nodiscard]] stl::string_view https() const noexcept {
            return interface_type::env("HTTPS");
        }

        /**
         * @brief get the server admin environment value
         * @return probabely the administrator's email address
         */
        [[nodiscard]] stl::string_view server_admin() const noexcept {
            return interface_type::env("SERVER_ADMIN");
        }

        /**
         * @brief get the path environment variable
         * @details The system path your server is running under
         */
        [[nodiscard]] stl::string_view path() const noexcept {
            return interface_type::env("PATH");
        }

        /**
         * @brief get the script_filename of the environment variables
         * @details The full pathname of the current CGI
         */
        [[nodiscard]] stl::string_view script_filename() const noexcept {
            return interface_type::env("SCRIPT_FILENAME");
        }

        /**
         * @brief get a single header
         * @param name
         */
        [[nodiscard]] stl::string_view header(stl::string_view const& name) const noexcept {
            return interface_type::header(stl::string(name));
        }

        /**
         * @brief get all of the headers as a string_view
         * @details this method in CGI interfaces will have to do more than
         * in the our own version because data has already been parsed by the
         * CGI server; and we have to recreate it based on the environment
         * variables.
         */
        [[nodiscard]] stl::string_view headers() const noexcept {
            return interface_type::headers();
        }

        /**
         * @brief get the whole body as a string_view
         * @details this method will return raw string values of the body of
         * the request and will not parse it. Parsing it is another methods'
         * problem that might even use this function as the source.
         */
        [[nodiscard]] stl::string_view body() const noexcept {
            return interface_type::body();
        }
    };

    // todo: add interface extensions as well
    template <Traits TraitsType, Application App, ExtensionList EList = empty_extension_pack>
    struct cgi : public enable_traits<TraitsType> {
      public:
        using traits_type      = TraitsType;
        using application_type = App;
        using extension_list   = stl::remove_cvref_t<EList>;
        using interface_type   = cgi<traits_type, application_type, extension_list>;
        using str_view_type    = typename TraitsType::string_view_type;
        using str_type         = typename TraitsType::string_type;
        using ostream_t        = typename TraitsType::ostream_type;
        using request_type     = simple_request<traits_type, cgi_request, interface_type, extension_list>;
        using allocator_type   = typename request_type::allocator_type;
        using logger_type      = typename traits_type::logger_type;
        using logger_ref       = typename logger_type::logger_ref;
        using etraits          = enable_traits<traits_type>;
        using application_wrapper_type = application_wrapper<traits_type, application_type>;

      private:
        void ctor() noexcept {
            // I'm not using C here; so why should I pay for it!
            // And also the user should not use cin and cout. so ...
            stl::ios::sync_with_stdio(false);
        }

      public:
        application_wrapper_type app;

        template <typename AllocType>
        cgi(logger_ref logger = logger_type{}, AllocType const& alloc = AllocType{})
          : etraits{logger, alloc},
            app{logger, alloc} {
            ctor();
        };

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
                    auto content_length = to_uint<traits_type>(content_length_str);

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
            request_type req{etraits::get_allocator()};
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
