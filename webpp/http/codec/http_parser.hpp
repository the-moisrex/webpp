#ifndef WEBPP_HTTP_PARSER_HPP
#define WEBPP_HTTP_PARSER_HPP

#include "./common.hpp"
#include "./tokens.hpp"

#include <cstdint>
#include <string_view>

// NOLINTBEGIN(*-pointer-arithmetic)
namespace webpp::http {


    /// State machine enum for parsing the request line
    enum struct http_parsing_state : std::uint8_t {
        unparsed,        // parsing has not yet begun

        ok,              // Ok for now
        ok_request_line, // OK: request line is now fully parsed
        ok_headers_end,  // OK: we finished with headers (reached \r\n\r\n)

        need_more_data,  // We can't parse with this amount of input
        invalid_method,  // GET/POST/HEAD/... are valid
        invalid_target,
        invalid_version, // HTTP version is not valid
        invalid_crlf,    // \r\n is not currect

        too_many_headers,
        empty_header_name,
        invalid_char,          // invalid character found
        possible_line_folding, // Line folding is deprecated feature of HTTP
    };

    [[nodiscard]] static constexpr std::string_view to_string(http_parsing_state const state) noexcept {
        using enum http_parsing_state;
        switch (state) {
            case unparsed: return {"Not Parsed"};
            case ok: return {"Ok"};
            case ok_request_line: return {"Request line OK"};
            case ok_headers_end: return {"Headers are OK"};
            case need_more_data: return {"Need more input"};
            case invalid_method: return {"Invalid method"};
            case invalid_target: return {"Invalid target"};
            case invalid_version: return {"Invalid HTTP version"};
            case invalid_crlf: return {"Invalid CRLF"};
            case too_many_headers: return {"Too many headers provided"};
            case empty_header_name: return {"Empty header name"};
            case invalid_char: return {"Invalid character found"};
            case possible_line_folding: return {"Possible deprecated line folding found in headers"};
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
    static constexpr void parse_request_line(char const *&begin, char const *end, parsed_request_line &req) noexcept {
        using enum http_parsing_state;
        char const *pos = begin;

        // 1. Parse Method
        char const *method_start = pos;
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
        char const *target_start = pos;
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
        char const *version_start = pos;
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
    }

    /**
     * Parses an HTTP token until it hits `next_char`.
     */
    [[nodiscard]] static constexpr http_parsing_state
    parse_token(char const *&buf, char const *buf_end, stl::string_view &token, char next_char) noexcept {
        using enum http_parsing_state;

        char const *const buf_start = buf;

        // Scans forward to find the first character that is NOT a valid HTTP token.
        buf = token_charmap.find_first_not_in(buf, buf_end);

        // If we hit the end of the buffer before finding the delimiter,
        // or if the first non-token character is not the expected delimiter, fail.
        if (buf == buf_end || *buf != next_char) [[unlikely]] {
            return buf == buf_end ? need_more_data : invalid_char;
        }

        token = {buf_start, static_cast<std::size_t>(buf - buf_start)};

        return ok;
    }

    /**
     * Consumes a token up to the End-Of-Line (EOL: \n or \r\n).
     *
     * Validates that the token only contains printable characters and the horizontal tab (HT).
     * Any other control character, or an incomplete/missing EOL, results in an error.
     *
     * @param buf Start iterator/pointer of the buffer.
     * @param buf_end End iterator/pointer of the buffer.
     * @param out_token Output parameter for the extracted string_view.
     */
    [[nodiscard]] static constexpr http_parsing_state
    token_to_eol(char const *&buf, char const *const buf_end, std::string_view &out_token) noexcept {
        using enum http_parsing_state;
        auto const *token_start = buf;

        // We skip printable ASCII (>= 0x20), extended chars (>= 0x80), and HT (0x09).
        // We stop at control characters (< 0x20) and DEL (0x7F).
        while (buf != buf_end) {
            auto const uc = static_cast<unsigned char>(*buf);
            if ((uc < 0x20 && uc != 0x09) || uc == 0x7F) {
                break;
            }
            ++buf;
        }

        if (buf == buf_end) [[unlikely]] {
            return invalid_crlf; // Unexpected EOF
        }

        // Handle \r\n (CRLF)
        if (*buf == '\r') {
            auto const *next = buf;
            ++next;
            if (next != buf_end && *next == '\n') {
                out_token = {&(*token_start), static_cast<std::size_t>(buf - token_start)};
                ++next;
                buf = next;
                return ok; // Return pointer after \n
            }
        }
        // Handle \n (LF)
        else if (*buf == '\n')
        {
            out_token = {&(*token_start), static_cast<std::size_t>(buf - token_start)};
            ++buf;
            return ok; // Return pointer after \n
        }

        // Encountered an invalid control character before EOL
        [[unlikely]] { return invalid_char; }
    }

    /**
     * Parse one single header line
     * @returns `ok` if parsed fine, `ok_headers` of reached the end, other errors otherwise.
     */
    [[nodiscard]] static constexpr http_parsing_state
    parse_header(char const *&buf, char const *buf_end, stl::string_view &name, stl::string_view &value) noexcept {
        using enum http_parsing_state;
        auto const *buf_start = buf;

        if (buf == buf_end) [[unlikely]] {
            return need_more_data;
        }
        if (*buf == '\r') {
            ++buf;
            if (buf == buf_end) [[unlikely]] {
                buf = buf_start;
                return need_more_data;
            }
            if (*buf++ != '\n') [[unlikely]] {
                return invalid_crlf;
            }
            // Reached the last header
            return ok_headers_end;
        }
        if (*buf == '\n') [[unlikely]] {
            ++buf;
            // We'll assume it's the last header nontheless
            return ok_headers_end;
        }
        if (*buf != ' ' && *buf != '\t') [[unlikely]] {
            // A lot of older HTTP parsers still parse line folding for historical reasons. We choose not to.
            name = {}; // clear the name
            return possible_line_folding;
        }

        // Parsing header name, but we do not discard SP before colon. It introduces inconsistency between parsers,
        // and thus allowing header smuggling.
        // http://www.mozilla.org/security/announce/2006/mfsa2006-33.html
        if (auto const res = parse_token(buf, buf_end, name, ':'); res != ok) [[unlikely]] {
            if (res == need_more_data) {
                buf = buf_start;
            }
            return res;
        }
        if (name.empty()) [[unlikely]] {
            return empty_header_name;
        }

        // ltrim LWS:
        ++buf;
        if (*buf == ' ' || *buf == '\t') [[likely]] {
            ++buf;
        }
        for (;; ++buf) [[unlikely]] {
            if (buf == buf_end) [[unlikely]] {
                buf = buf_start;
                return need_more_data;
            }
            if (*buf != ' ' && *buf != '\t') {
                break;
            }
        }

        if (auto const res = token_to_eol(buf, buf_end, value); res != ok) [[unlikely]] {
            return res;
        }

        // Remove trailing SPs and HTABs
        rtrim_lws(value);
        return ok;
    }

    /**
     * Parses all headers until the end of the headers section is reached (\r\n\r\n).
     * Attention: this does not parse the request line
     */
    template <typename HeaderCallback>
    [[nodiscard]] static constexpr http_parsing_state
    parse_headers(char const *&buf, char const *buf_end, HeaderCallback const &callback)
      noexcept(noexcept(callback(std::string_view{}, std::string_view{}))) {
        using enum http_parsing_state;

        while (buf != buf_end) {
            std::string_view name;
            std::string_view value;

            // Use the existing single-header parser
            http_parsing_state const res = parse_header(buf, buf_end, name, value);

            if (res == ok) [[likely]] {
                // Header parsed successfully, pass it to the callback
                callback(name, value);
            } else {
                // Returns ok_headers_end if we hit the \r\n\r\n boundary,
                // or an error/need_more_data state otherwise.
                return res;
            }
        }

        return need_more_data;
    }



} // namespace webpp::http

// NOLINTEND(*-pointer-arithmetic)

#endif // WEBPP_HTTP_PARSER_HPP
