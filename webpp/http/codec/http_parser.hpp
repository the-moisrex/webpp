#ifndef WEBPP_HTTP_PARSER_HPP
#define WEBPP_HTTP_PARSER_HPP

#include "./tokens.hpp"

#include <cstdint>
#include <string_view>

namespace webpp::http {


    // State machine enum for parsing the request line
    enum struct http_parsing_state : std::uint8_t {
        unparsed,        // parsing has not yet begun

        ok,              // Ok for now
        ok_request_line, // OK: request line is now fully parsed

        need_more_data,  // We can't parse with this amount of input
        invalid_method,  // GET/POST/HEAD/... are valid
        invalid_target,
        invalid_version, // HTTP version is not valid
        invalid_crlf     // \r\n is not currect
    };

    [[nodiscard]] static constexpr std::string_view to_string(http_parsing_state state) noexcept {
        using enum http_parsing_state;
        switch (state) {
            case unparsed: return {"Not Parsed"};
            case ok: return {"Ok"};
            case ok_request_line: return {"Request Line OK"};
            case need_more_data: return {"Need more input"};
            case invalid_method: return {"Invalid method"};
            case invalid_target: return {"Invalid Target"};
            case invalid_version: return {"Invalid HTTP Version"};
            case invalid_crlf: return {"Invalid CRLF"};
            default: break;
        }
        return {"Unknown State"};
    }

    struct parsed_request_line {
        http_parsing_state state{http_parsing_state::need_more_data};
        std::string_view   method;
        std::string_view   target; // The raw unparsed URL/URI
        std::string_view   version;
    };

    // Helper for fast target character validation (visible ASCII excluding space)
    [[nodiscard]] static constexpr bool is_valid_target_char(char c) noexcept {
        auto const uc = static_cast<unsigned char>(c);
        return uc > 0x20 && uc < 0x7F;
    }

    /**
     * Parses the HTTP request line per RFC 9112.
     * Updates `begin` to point immediately after the CRLF on success.
     *
     * todo: check maximum length (max_request_line_length)
     */
    static constexpr void parse_request_line(char const*& begin, char const* end, parsed_request_line& req) noexcept {
        using enum http_parsing_state;
        // NOLINTBEGIN(*-pointer-arithmetic)
        char const* pos = begin;

        // 1. Parse Method
        char const* method_start = pos;
        for (; pos != end && *pos != ' '; ++pos) {
            if (!is_http_token(*pos)) [[unlikely]] {
                req.state = invalid_method;
                return;
            }
        }

        if (pos == end) [[unlikely]] {
            req.state = need_more_data;
            return;
        }
        if (pos == method_start) [[unlikely]] {
            req.state = invalid_method;
            return;
        }

        req.method = std::string_view(method_start, static_cast<std::size_t>(pos - method_start));
        ++pos; // Skip SP

        // 2. Parse Target (URL) - Basic validation only
        char const* target_start = pos;
        while (pos != end && *pos != ' ') {
            if (!is_valid_target_char(*pos)) {
                req.state = invalid_target;
                return;
            }
            ++pos;
        }

        if (pos == end) [[unlikely]] {
            req.state = need_more_data;
            return;
        }
        if (pos == target_start) [[unlikely]] {
            req.state = invalid_target;
            return;
        }

        req.target = std::string_view(target_start, static_cast<std::size_t>(pos - target_start));
        ++pos; // Skip SP

        // 3. Parse HTTP Version
        char const* version_start = pos;
        while (pos != end && *pos != '\r') {
            ++pos;
        }

        if (pos == end) [[unlikely]] {
            req.state = need_more_data;
            return;
        }

        req.version = std::string_view(version_start, static_cast<std::size_t>(pos - version_start));

        // Minimal standard validation for the version prefix
        if (!req.version.starts_with("HTTP/")) [[unlikely]] {
            req.state = invalid_version;
            return;
        }

        // 4. Validate CRLF
        if (pos + 1 >= end) [[unlikely]] {
            req.state = need_more_data;
            return;
        }

        if (*(pos + 1) != '\n') [[unlikely]] {
            req.state = invalid_crlf;
            return;
        }

        req.state = ok_request_line;

        // Transactional advance: only update the input pointer on complete success
        begin = pos + 2;
        // NOLINTEND(*-pointer-arithmetic)
    }

} // namespace webpp::http


#endif // WEBPP_HTTP_PARSER_HPP
