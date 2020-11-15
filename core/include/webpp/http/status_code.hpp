// Created by moisrex on 10/7/20.

#ifndef WEBPP_STATUS_CODE_HPP
#define WEBPP_STATUS_CODE_HPP

#include "../std/std.hpp"

#include <cstdint>

namespace webpp::http {

    using status_code_type = stl::uint_fast16_t;

    enum struct status_code_category : stl::uint_fast16_t {
        unknown       = 0,
        informational = 100,
        successful    = 200,
        redirection   = 300,
        client_error  = 400,
        server_error  = 500
    };

    enum struct status_code : status_code_type {
        /** An unknown status-code.

            This value indicates that the value for the status code
            is not in the list of commonly recognized status codes.
            Callers interested in the exactly value should use the
            interface which provides the raw integer.
        */
        unknown = 0,

        continue_                           = 100,

        /** Switching Protocols

            This status indicates that a request to switch to a new
            protocol was accepted and applied by the server. A successful
            response to a WebSocket Upgrade HTTP request will have this
            code.
        */
        switching_protocols                 = 101,

        processing                          = 102,

        ok                                  = 200,
        created                             = 201,
        accepted                            = 202,
        non_authoritative_information       = 203,
        no_content                          = 204,
        reset_content                       = 205,
        partial_content                     = 206,
        multi_status                        = 207,
        already_reported                    = 208,
        im_used                             = 226,

        multiple_choices                    = 300,
        moved_permanently                   = 301,
        found                               = 302,
        see_other                           = 303,
        not_modified                        = 304,
        use_proxy                           = 305,
        temporary_redirect                  = 307,
        permanent_redirect                  = 308,

        bad_request                         = 400,
        unauthorized                        = 401,
        payment_required                    = 402,
        forbidden                           = 403,
        not_found                           = 404,
        method_not_allowed                  = 405,
        not_acceptable                      = 406,
        proxy_authentication_required       = 407,
        request_timeout                     = 408,
        conflict                            = 409,
        gone                                = 410,
        length_required                     = 411,
        precondition_failed                 = 412,
        payload_too_large                   = 413,
        uri_too_long                        = 414,
        unsupported_media_type              = 415,
        range_not_satisfiable               = 416,
        expectation_failed                  = 417,
        misdirected_request                 = 421,
        unprocessable_entity                = 422,
        locked                              = 423,
        failed_dependency                   = 424,
        upgrade_required                    = 426,
        precondition_required               = 428,
        too_many_requests                   = 429,
        request_header_fields_too_large     = 431,
        connection_closed_without_response  = 444,
        unavailable_for_legal_reasons       = 451,
        client_closed_request               = 499,

        internal_server_error               = 500,
        not_implemented                     = 501,
        bad_gateway                         = 502,
        service_unavailable                 = 503,
        gateway_timeout                     = 504,
        http_version_not_supported          = 505,
        variant_also_negotiates             = 506,
        insufficient_storage                = 507,
        loop_detected                       = 508,
        not_extended                        = 510,
        network_authentication_required     = 511,
        network_connect_timeout_error       = 599
    };


    constexpr auto status_code_reason_phrase(status_code_type code) noexcept {
        switch (code) {
            case 100: return "Continue";
            case 101: return "Switching Protocols";
            case 102: // WebDav
                return "Processing";
            case 103: return "Early Hints";

            case 200: return "OK";
            case 201: return "Created";
            case 202: return "Accepted";
            case 203: return "Non-Authoritative Information";
            case 204: return "No Content";
            case 205: return "Reset Content";
            case 206: return "Partial Content";
            case 207: // WebDav
                return "Multi-Status";
            case 208: // WebDav
                return "Already Reported";
            case 226: return "IM Used";

            case 300: return "Multiple Choices";
            case 301: return "Moved Permanently";
            case 302: // Previously named "Moved temporarily"
                return "Found";
            case 303: return "See Other";
            case 304: return "Not Modified";
            case 305: return "Use Proxy";
            case 306: return "Switch Proxy";
            case 307: return "Temporary Redirect";
            case 308: return "Permanent Redirect";

            case 400: return "Bad Request";
            case 401: return "Unauthorized";
            case 402: return "Payment Required";
            case 403: return "Forbidden";
            case 404: // Joke Not Found :)
                return "Not Found";
            case 405: return "Method Not Allowed";
            case 406: return "Not Acceptable";
            case 407: return "Proxy Authentication Required";
            case 408: return "Request Timeout";
            case 409: return "Conflict";
            case 410: return "Gone";
            case 411: return "Length Required";
            case 412: return "Precondition Failed";
            case 413: return "Payload Too Large";
            case 414: return "URI Too Long";
            case 415: return "Unsupported Media Type";
            case 416: return "Range Not Satisfiable";
            case 417: return "Expectation Failed";
            case 418: // We should make jokes about this one!
                return "I'm a teapot";
            case 421: return "Misdirected Request";
            case 422: // WebDav
                return "Unprocessable Entity";
            case 423: // WebDav
                return "Locked";
            case 424: // WebDav
                return "Failed Dependency";
            case 425: return "Too Early";
            case 426: return "Upgrade Required";
            case 428: return "Precondition Required";
            case 429: return "Too Many Requests";
            case 431: return "Request Header Fields Too Large";
            case 451: return "Unavailable For Legal Reasons";

            case 500: return "Internal Server Error";
            case 501: return "Not Implemented";
            case 502: return "Bad Gateway";
            case 503: return "Service Unavailable";
            case 504: return "Gateway Timeout";
            case 505: return "HTTP Version Not Supported";
            case 506: return "Variant Also Negotiates";
            case 507: // WebDav
                return "Insufficient Storage";
            case 508: // WebDav
                return "Loop Detected";
            case 510: return "Not Extended";
            case 511:
                return "Network Authentication Required";

                // Unofficial codes:
                // case 103:
                // return "Checkpoint";
                // case 218: // Apache web server
                // return "This is fine";
                // case 419: // Laravel Framework
                // return "Page Expired";
                // case 420: // Spring Framework: Method Failure | Twitter:
                // Enhance Your Calm return "Method Failure"; case 430: //
                // Shopify return "Request Header Fields Too Large"; case 450:
                // // Microsoft return "Blocked by Windows Parental Controls";
                // case 498: // Esri
                // return "Invalid Token";
                // case 499: // Esri
                // return "Token Required";
                // case 509: // Apache Web Server/cPanel
                // return "Bandwidth Limit Exceeded";
                // case 526:
                // return "Invalid SSL Certificate";
                // case 529:
                // return "Site is overloaded";
                // case 530:
                // return "Site is frozen";
                // case 598:
                // return "Network read timeout error";
                //
                // IIS
                // case 440:
                // return "Login Time-out";
                // case 449:
                // return "Retry With";
                // case 451:
                //   return "Redirect";

            default: return "";
        }
    }

    static constexpr auto status_code_reason_phrase(status_code code) noexcept {
        return status_code_reason_phrase(static_cast<status_code_type>(code));
    }

    static constexpr status_code_category get_status_code_category(status_code_type code) noexcept {
        if (code >= 100 && code < 200) {
            return status_code_category::informational;
        } else if (code >= 200 && code < 300) {
            return status_code_category::successful;
        } else if (code >= 300 && code < 400) {
            return status_code_category::client_error;
        } else if (code >= 400 && code < 500) {
            return status_code_category::server_error;
        }
        return status_code_category::unknown;
    }


} // namespace webpp

#endif // WEBPP_STATUS_CODE_HPP
