// Created by moisrex on 10/24/20.

#ifndef WEBPP_CGI_REQUEST_HPP
#define WEBPP_CGI_REQUEST_HPP

#include "../../std/string_view.hpp"
#include "../../traits/traits_concepts.hpp"
#include "./protocol_concepts.hpp"


namespace webpp {

    /**
     * Specializing the request_t<cgi> methods so the user can use them.
     * The request_t class is one of the important classes which that means It's
     * going to be used a lot so, we will do everything (even if it means we
     * have to copy and paste lots of codes to make it happen) to make sure
     * the user is able to use this class properly and easily.
     */
    template <Traits TraitsType, typename /* fixme: RequestExtensionList */ REL, /* Protocol */ typename ProtoType>
    struct cgi_request : public REL, public enable_traits<TraitsType> {
        using traits_type            = stl::remove_cvref_t<TraitsType>;
        using protocol_type          = stl::remove_cvref_t<ProtoType>;
        using request_extension_list = REL;
        using allocator_type   = typename traits_type::template allocator<typename traits_type::char_type>;
        using application_type = typename protocol_type::application_type;
        using logger_type      = typename traits_type::logger_type;
        using logger_ref       = typename logger_type::logger_ref;
        using etraits          = enable_traits<traits_type>;


        cgi_request(auto&&...etraits_args) noexcept
          : etraits(stl::forward<decltype(etraits_args)>(etraits_args)...) {}

        /**
         * @brief get the server's software
         * @details Name and version of the information server software
         * answering the request (and running the gateway). Format: name/version.
         * @example SERVER_SOFTWARE=Apache/2.4.41 (Unix) OpenSSL/1.1.1d
         */
        [[nodiscard]] stl::string_view server_software() const noexcept {
            return protocol_type::env("SERVER_SOFTWARE");
        }

        /**
         * @brief get the server name
         * @details Server's hostname, DNS alias, or IP address as it appears in
         * self-referencing URLs.
         * @example SERVER_NAME=localhost
         */
        [[nodiscard]] stl::string_view server_name() const noexcept {
            return protocol_type::env("SERVER_NAME");
        }

        /**
         * @brief get the gateway interface environment variable
         * @details CGI specification revision with which this server complies.
         * Format: CGI/revision.
         * @example GATEWAY_INTERFACE=CGI/1.1
         */
        [[nodiscard]] stl::string_view gateway_interface() const noexcept {
            return protocol_type::env("GATEWAY_INTERFACE");
        }

        /**
         * @brief get the server protocol
         * @details Name and revision of the information protocol this request
         * came in with. Format: protocol/revision.
         * @example SERVER_PROTOCOL=HTTP/1.1
         */
        [[nodiscard]] stl::string_view server_protocol() const noexcept {
            return protocol_type::env("SERVER_PROTOCOL");
        }

        /**
         * @brief get the port that the server is listening on
         * @details Port number to which the request was sent.
         */
        [[nodiscard]] stl::string_view server_port() const noexcept {
            return protocol_type::env("SERVER_PORT");
        }

        /**
         * @brief Get the method
         * @details Method with which the request was made. For HTTP, this is Get, Head, Post, and so on.
         */
        [[nodiscard]] stl::string_view request_method() const noexcept {
            return protocol_type::env("REQUEST_METHOD");
        }

        /**
         * @brief get the path info
         * @details Extra path information, as given by the client. Scripts can
         * be accessed by their virtual pathname, followed by extra information
         * at the end of this path. The extra information is sent as PATH_INFO.
         * @example PATH_INFO=/hello/world
         */
        [[nodiscard]] stl::string_view path_info() const noexcept {
            return protocol_type::env("PATH_INFO");
        }

        /**
         * @brief get the path translated
         * @details Translated version of PATH_INFO after any
         * virtual-to-physical mapping.
         * @example PATH_TRANSLATED=/srv/http/hello/world
         */
        [[nodiscard]] stl::string_view path_translated() const noexcept {
            return protocol_type::env("PATH_TRANSLATED");
        }

        /**
         * @brief get the script name
         * @details Virtual path to the script that is executing; used for
         * self-referencing URLs.
         * @example SCRIPT_NAME=/cgi-bin/one.cgi
         */
        [[nodiscard]] stl::string_view script_name() const noexcept {
            return protocol_type::env("SCRIPT_NAME");
        }

        /**
         * @brief get the query string
         * @details Query information that follows the ? in the URL that
         * referenced this script.
         */
        [[nodiscard]] stl::string_view query_string() const noexcept {
            return protocol_type::env("QUERY_STRING");
        }

        /**
         * @brief get the remote host
         * @details Hostname making the request. If the server does not have
         * this information, it sets REMOTE_ADDR and does not set REMOTE_HOST.
         */
        [[nodiscard]] stl::string_view remote_host() const noexcept {
            return protocol_type::env("REMOTE_HOST");
        }

        /**
         * @brief get the ip address of the user
         * @details IP address of the remote host making the request.
         */
        [[nodiscard]] stl::string_view remote_addr() const noexcept {
            return protocol_type::env("REMOTE_ADDR");
        }

        /**
         * @brief get the auth type
         * @details If the server supports user authentication, and the script
         * is protected, the protocol-specific authentication method used to
         * validate the user.
         */
        [[nodiscard]] stl::string_view auth_type() const noexcept {
            return protocol_type::env("AUTH_TYPE");
        }

        /**
         * @brief get the remote user or auth user value (both should be the
         * same)
         * @details If the server supports user authentication, and the script
         * is protected, the username the user has authenticated as. (Also
         * available as AUTH_USER.)
         */
        [[nodiscard]] stl::string_view remote_user() const noexcept {
            if (auto a = protocol_type::env("REMOTE_USER"); !a.empty())
                return a;
            return protocol_type::env("AUTH_USER");
        }

        /**
         * @brief get the remote user or auth user value (both should be the
         * same)
         * @details If the server supports user authentication, and the script
         * is protected, the username the user has authenticated as. (Also
         * available as AUTH_USER.)
         */
        [[nodiscard]] stl::string_view auth_user() const noexcept {
            if (auto a = protocol_type::env("AUTH_USER"); !a.empty())
                return a;
            return protocol_type::env("REMOTE_USER");
        }

        /**
         * @brief get the remote ident
         * @details If the HTTP server supports RFC 931 identification, this
         * variable is set to the remote username retrieved from the server. Use
         * this variable for logging only.
         */
        [[nodiscard]] stl::string_view remote_ident() const noexcept {
            return protocol_type::env("REMOTE_IDENT");
        }

        /**
         * @brief returns the request scheme (http/https/...)
         */
        [[nodiscard]] stl::string_view request_scheme() const noexcept {
            return protocol_type::env("REQUEST_SCHEME");
        }

        /**
         * @brief get the user's port number
         */
        [[nodiscard]] stl::string_view remote_port() const noexcept {
            return protocol_type::env("REMOTE_PORT");
        }

        /**
         * @brief get the ip address that the server is listening on
         */
        [[nodiscard]] stl::string_view server_addr() const noexcept {
            return protocol_type::env("SERVER_ADDR");
        }

        /**
         * @brief get the request uri
         */
        [[nodiscard]] stl::string_view request_uri() const noexcept {
            return protocol_type::env("REQUEST_URI");
        }

        /**
         * @brief get the content_type
         * @details For queries that have attached information, such as HTTP
         * POST and PUT, this is the content type of the data.
         */
        [[nodiscard]] stl::string_view content_type() const noexcept {
            return protocol_type::env("CONTENT_LENGTH");
        }

        /**
         * @brief get the content length
         * @details Length of the content as given by the client.
         */
        [[nodiscard]] stl::string_view content_length() const noexcept {
            return protocol_type::env("CONTENT_LENGTH");
        }

        /**
         * @brief get the document root environment value
         * @details The root directory of your server
         */
        [[nodiscard]] stl::string_view document_root() const noexcept {
            return protocol_type::env("DOCUMENT_ROOT");
        }

        /**
         * @brief get the https environment value
         * @return "on" if the user used HTTPS protocol
         */
        [[nodiscard]] stl::string_view https() const noexcept {
            return protocol_type::env("HTTPS");
        }

        /**
         * @brief get the server admin environment value
         * @return probabely the administrator's email address
         */
        [[nodiscard]] stl::string_view server_admin() const noexcept {
            return protocol_type::env("SERVER_ADMIN");
        }

        /**
         * @brief get the path environment variable
         * @details The system path your server is running under
         */
        [[nodiscard]] stl::string_view path() const noexcept {
            return protocol_type::env("PATH");
        }

        /**
         * @brief get the script_filename of the environment variables
         * @details The full pathname of the current CGI
         */
        [[nodiscard]] stl::string_view script_filename() const noexcept {
            return protocol_type::env("SCRIPT_FILENAME");
        }

        /**
         * @brief get a single header
         * @param name
         */
        [[nodiscard]] stl::string_view header(stl::string_view const& name) const noexcept {
            return protocol_type::header(stl::string(name));
        }

        /**
         * @brief get all of the headers as a string_view
         * @details this method in CGI interfaces will have to do more than
         * in the our own version because data has already been parsed by the
         * CGI server; and we have to recreate it based on the environment
         * variables.
         */
        [[nodiscard]] stl::string_view headers() const noexcept {
            return protocol_type::headers();
        }

        /**
         * @brief get the whole body as a string_view
         * @details this method will return raw string values of the body of
         * the request and will not parse it. Parsing it is another methods'
         * problem that might even use this function as the source.
         */
        [[nodiscard]] stl::string_view body() const noexcept {
            return protocol_type::body();
        }
    };

}

#endif // WEBPP_CGI_REQUEST_HPP
