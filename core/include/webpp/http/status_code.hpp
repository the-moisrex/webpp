// Created by moisrex on 10/7/20.

#ifndef WEBPP_STATUS_CODE_HPP
#define WEBPP_STATUS_CODE_HPP

#include "../std/std.hpp"

#include <cstdint>

namespace webpp {

    using status_code_type = stl::uint_fast16_t;

    enum struct status_code : status_code_type {
        no_status                       = 0,
        switching_protocols             = 101,
        ok                              = 200,
        created                         = 201,
        accepted                        = 202,
        no_content                      = 204,
        partial_content                 = 206,
        multiple_choices                = 300,
        moved_permanently               = 301,
        found                           = 302,
        see_other                       = 303,
        not_modified                    = 304,
        moved_temporarily               = 307,
        bad_request                     = 400,
        unauthorized                    = 401,
        forbidden                       = 403,
        not_found                       = 404,
        request_entity_too_large        = 413,
        requested_range_not_satisfiable = 416,
        internal_server_error           = 500,
        not_implemented                 = 501,
        bad_gateway                     = 502,
        service_unavailable             = 503,
        version_not_supported           = 505
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

    constexpr auto status_code_reason_phrase(status_code code) noexcept {
        return status_code_reason_phrase(static_cast<status_code_type>(code));
    }


} // namespace webpp

#endif // WEBPP_STATUS_CODE_HPP
