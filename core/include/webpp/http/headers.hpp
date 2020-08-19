#ifndef WEBPP_HTTP_HEADERS_H
#define WEBPP_HTTP_HEADERS_H

#include "../std/std.hpp"

#include <cstdint>

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
     */
    template <Traits TraitsType, typename EList>
    struct header_field : public EList {
        using traits_type      = TraitsType;
        using string_type      = typename traits_type::string_type;
        using string_view_type = typename traits_type::string_view_type;
        using allocator_type   = typename string_type::allocator_type;
        using alloc_type       = allocator_type const&;

        string_type name;
        string_type value;

        //        constexpr response_header_field(string_type&& _name, string_type&& _value)
        //          : name{stl::move(_name)},
        //            value{stl::move(_value)} {}
        //
        //        constexpr response_header_field(string_view_type _name, string_view_type _value, alloc_type
        //        alloc)
        //          : name{_name, alloc},
        //            value{_value, alloc} {}
        //
        //        constexpr response_header_field(string_view_type _name, string_type&& _value, alloc_type
        //        alloc)
        //          : name{_name, alloc},
        //            value{stl::move(_value)} {}
        //
        //        constexpr response_header_field(string_type&& _name, string_view_type _value, alloc_type
        //        alloc)
        //          : name{stl::move(_name)},
        //            value{_value, alloc} {}
        //

        /**
         * Check if the specified name is the same as the header name
         * It's not a good idea to compare the name directly; the header name is
         * case-insensitive.
         */
        constexpr bool is_name(istl::ConvertibleToStringView auto&& str) const noexcept {
            return to_lower_copy<traits_type>(name) ==
                   to_lower_copy<traits_type>(istl::to_string_view(stl::forward<decltype(str)>(str)));
        }

        constexpr bool operator==(istl::ConvertibleToStringView auto&& str) const noexcept {
            return is_name(stl::forward<decltype(str)>(str));
        }

        constexpr bool operator!=(istl::ConvertibleToStringView auto&& str) const noexcept {
            return !operator==(stl::forward<decltype(str)>(str));
        }

        friend constexpr bool operator==(istl::ConvertibleToStringView auto&&   str,
                                         header_field<TraitsType, EList> const& field) noexcept {
            return field == istl::to_string_view(str);
        }

        friend constexpr bool operator!=(istl::ConvertibleToStringView auto&&   str,
                                         header_field<TraitsType, EList> const& field) noexcept {
            return field != istl::to_string_view(str);
        }
    };



    /**
     * hash function of std::unordered_set<webpp::basic_cookie>
     * Even though we're not using this, we put it here for when/if we changed our mind and wanted to use it
     * later or even if someone else wanted this
     */
    template <typename FieldType>
    struct header_field_hash {
        using field_type = FieldType;

        template <class T>
        void hash_combine(stl::size_t& seed, const T& v) noexcept {
            stl::hash<T> hasher;
            seed ^= hasher(v) + 0x9e3779b9 + (seed << 6u) + (seed >> 2u);
        }

        using result_type = stl::size_t;

        result_type operator()(field_type const& c) const noexcept {
            // change the "same_as" method too if you ever touch this function
            result_type seed = 0;
            hash_combine(seed, c.name);
            hash_combine(seed, c.value);
            return seed;
        }
    };

    template <typename FieldType>
    struct header_field_equals {
        using field_type = FieldType;

        [[nodiscard]] constexpr bool operator()(const field_type& lhs, const field_type& rhs) const noexcept {
            return lhs.name == rhs.name;
        }
    };




} // namespace webpp

#endif // WEBPP_HTTP_HEADERS_H
