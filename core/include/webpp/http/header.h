#ifndef WEBPP_HTTP_HEADERS_H
#define WEBPP_HTTP_HEADERS_H

#include "../std/format.h"
#include "../std/string.h"
#include "../std/string_view.h"
#include "../std/unordered_set.h"
#include "../traits/traits_concepts.h"
#include "./common.h"
#include "./cookies/cookie.h"

#include <cstdint>
#include <iterator>
#include <sstream>
#include <type_traits>

namespace webpp {

    using status_code_type = stl::uint_fast16_t;

    constexpr auto status_reason_phrase(status_code_type status_code) noexcept {
        switch (status_code) {
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

    /**
     * This is the header class witch will contain the name, and the value of
     * one single field of a header.
     * @tparam TraitsType
     * @tparam HeaderType
     */
    template <Traits TraitsType, typename EList = empty_extension_pack>
    struct response_header_field : public EList {
        using traits_type      = TraitsType;
        using string_type      = typename traits_type::string_type;
        using string_view_type = typename traits_type::string_view_type;
        using allocator_type   = typename string_type::allocator_type;
        using alloc_type       = allocator_type const&;

        string_type name;
        string_type value;

        constexpr response_header_field(string_type&& _name, string_type&& _value)
          : name{stl::move(_name)},
            value{stl::move(_value)} {
        }

        constexpr response_header_field(string_view_type _name, string_view_type _value,
                                        alloc_type alloc = allocator_type{})
          : name{_name, alloc},
            value{_value, alloc} {
        }

        constexpr response_header_field(string_view_type _name, string_type&& _value,
                                        alloc_type alloc = allocator_type{})
          : name{_name, alloc},
            value{stl::move(_value)} {
        }

        constexpr response_header_field(string_type&& _name, string_view_type _value,
                                        alloc_type alloc = allocator_type{})
          : name{stl::move(_name)},
            value{_value, alloc} {
        }


        /**
         * Check if the specified name is the same as the header name
         * It's not a good idea to compare the name directly; the header name is
         * case-insensitive.
         */
        constexpr bool is_name(string_view_type const& str) const noexcept {
            return to_lower_copy<string_type::value_type, string_type::traits_type,
                                 string_type::allocator_type>(name) ==
                   to_lower_copy<string_view_type::value_type, string_view_type::traits_type,
                                 traits_type::allocator>(str);
        }
    };


    /**
     * Setting non-ascii characters in the value section of the headers should
     * result in transforming the value to the "Encoded-Word" syntax (RFC 2047).
     * For example:
     *   In the header:  "Subject: =?iso-8859-1?Q?=A1Hola,_se=F1or!?="
     *   Interpreted as: "Subject: ¡Hola, señor!"
     *
     */
    template <Traits TraitsType, typename HeaderEList = empty_extension_pack,
              typename HeaderFieldType = response_header_field<TraitsType>>
    class response_headers : public stl::unordered_multiset<TraitsType, HeaderFieldType>, public HeaderEList {

        using super = stl::unordered_multiset<TraitsType, HeaderFieldType>;

      public:
        using traits_type       = TraitsType;
        using string_type       = typename traits_type::string_type;
        using header_field_type = HeaderFieldType;

        using HeaderEList::HeaderEList;

        status_code_type status_code = 200u;


        auto str() const noexcept {
            // todo check performance
            // TODO: add support for other HTTP versions
            // res << "HTTP/1.1" << " " << status_code() << " " <<
            // status_reason_phrase(status_code()) << "\r\n";
            stl::size_t size = 1;
            for (auto const& [attr, val] : *this) {
                size += attr.size() + val.size() + 4;
            }
            string_type res{super::get_allocator()};
            res.reserve(size);
            for (auto const& [attr, val] : *this) {
                // todo: make sure value is secure and doesn't have any newlines
                stl::format_to(stl::back_insert_iterator<string_type>(res), "{}: {}\r\n", attr, val);
            }
            return res;
        }
    };


    struct response_header_field_descriptor {
        template <typename ExtensionType>
        struct has_related_extension_pack {
            static constexpr bool value = requires {
                typename ExtensionType::response_header_field_extensions;
            };
        };

        template <typename ExtensionType>
        using related_extension_pack_type = typename ExtensionType::response_header_field_extensions;

        template <typename ExtensionListType, typename TraitsType, typename EList>
        using mid_level_extensie_type = response_header_field<TraitsType, EList>;

        // empty final extensie
        template <typename ExtensionListType, typename TraitsType, typename EList>
        struct final_extensie_type final : public EList {
            using EList::EList;
        };
    };

    struct response_header_descriptor {
        template <typename ExtensionType>
        struct has_related_extension_pack {
            static constexpr bool value = requires {
                typename ExtensionType::response_headers_extensions;
            };
        };

        template <typename ExtensionType>
        using related_extension_pack_type = typename ExtensionType::response_headers_extensions;

        template <typename ExtensionListType, typename TraitsType, typename EList>
        using mid_level_extensie_type = response_headers<
          TraitsType, EList,
          typename ExtensionListType::template extensie_type<TraitsType, response_header_field_descriptor>>;

        // empty final extensie
        template <typename ExtensionListType, typename TraitsType, typename EList>
        struct final_extensie_type final : public EList {
            using EList::EList;
        };
    };



} // namespace webpp

#endif // WEBPP_HTTP_HEADERS_H
