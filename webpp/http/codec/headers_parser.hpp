#ifndef WEBPP_HTTP_HEADERS_PARSER_HPP
#define WEBPP_HTTP_HEADERS_PARSER_HPP

#include "../../std/string_view.hpp"
#include "tokens.hpp"

#include <cstdint>
#include <iterator>

namespace webpp::http {

    enum struct parsing_status : stl::uint_fast8_t {
        unknown,         // unknown status, we haven't started yet
        done,            // no error
        bad_eof,         // we reached the end of buffer where we shouldn't have
        unexpected_char, // Unexpected Character found
        empty_name       // Empty field names are not allowed in HTTP
    };

    /**
     * HTTP Headers Parser Iterator;
     *
     * This is a forward iterator that parses a buffer and gives these details on each iteration:
     *   - Header Name (Start and End positions)
     *   - Header Value (Start and End positions)
     *
     * Features of this iterator:
     *   - constexpr friendly
     *   - noexcept
     *   - no-allocation
     *   - doesn't parse the values
     *   - doesn't merge header field values
     *
     **/
    struct headers_parser_iterator {
        using value_type        = char const*;
        using difference_type   = stl::size_t;
        using reference         = value_type&;
        using const_reference   = value_type const&;
        using pointer           = char const**;
        using const_pointer     = char const* const*;
        using iterator_category = stl::forward_iterator_tag;
        using iterator_concept  = stl::forward_iterator_tag;


      private:
        using size_type = stl::size_t;

        value_type     buf;
        value_type     buf_end;
        value_type     name_start   = nullptr;
        size_type      name_size    = 0;
        value_type     value_start  = nullptr;
        size_type      value_size   = 0;
        parsing_status status_value = parsing_status::unknown;


      public:
        constexpr headers_parser_iterator(value_type inp_buf, value_type inp_end) noexcept
          : buf{inp_buf},
            buf_end{inp_end} {}
        constexpr headers_parser_iterator(headers_parser_iterator const&)                = default;
        constexpr headers_parser_iterator(headers_parser_iterator&&) noexcept            = default;
        constexpr headers_parser_iterator& operator=(headers_parser_iterator const&)     = default;
        constexpr headers_parser_iterator& operator=(headers_parser_iterator&&) noexcept = default;
        constexpr ~headers_parser_iterator() noexcept                                    = default;

        /**
         * Check if the name is equal to the specified string view
         **/
        template <typename StrViewT>
        constexpr bool operator==(StrViewT str) noexcept {
            return str == name_start;
        }

        /**
         * Get the Field Name
         **/
        template <typename StrViewT = stl::string_view>
        constexpr StrViewT name() const noexcept {
            return {name_start, name_size};
        }

        /**
         * Get the Field Value
         **/
        template <typename StrViewT = stl::string_view>
        constexpr StrViewT value() const noexcept {
            return {value_start, value_size};
        }

        // Forward iterator requirements; Get the current position of the buffer
        constexpr reference operator*() noexcept {
            return buf;
        }

        constexpr const_reference operator*() const noexcept {
            return buf;
        }

        // Forward iterator requirements; Get the current position of the buffer
        constexpr pointer operator->() noexcept {
            return &buf;
        }

        constexpr const_pointer operator->() const noexcept {
            return &buf;
        }

        constexpr headers_parser_iterator& operator+=(difference_type val) noexcept {
            for (difference_type i = 0; i != val; ++i)
                operator++();
            return *this;
        }

        /**
         * Parse the next value
         **/
        constexpr headers_parser_iterator& operator++() noexcept {
#define WEBPP_CHECK_EOF()                   \
    if (buf == buf_end) {                   \
        set_error(parsing_status::bad_eof); \
        return *this;                       \
    }

#define WEBPP_EXPECT_CHAR(ch)                       \
    if (*buf++ != (ch)) {                           \
        set_error(parsing_status::unexpected_char); \
        return *this;                               \
    }


            WEBPP_CHECK_EOF();
            if (*buf == '\015') {
                ++buf;
                WEBPP_EXPECT_CHAR('\012');
                break;
            } else if (*buf == '\012') {
                ++buf;
                break;
            }
            if (*num_headers == max_headers) {
                status_value = -1;
                buf = buf_end = nullptr;
                return *this;
            }
            if (!(*num_headers != 0 && (*buf == ' ' || *buf == '\t'))) {
                /* parsing name, but do not discard SP before colon, see
                 * http://www.mozilla.org/security/announce/2006/mfsa2006-33.html */
                name_start                                  = buf;
                static constexpr char alignas(16) ranges1[] = "\x00 "  // control chars and up to SP
                                                              "\"\""   // 0x22
                                                              "()"     // 0x28,0x29
                                                              ",,"     // 0x2c
                                                              "//"     // 0x2f
                                                              ":@"     // 0x3a-0x40
                                                              "[]"     // 0x5b-0x5d
                                                              "{\377"; // 0x7b-0xff
                int found;
                buf = findchar_fast(buf, buf_end, ranges1, sizeof(ranges1) - 1, &found);
                if (!found) {
                    WEBPP_CHECK_EOF();
                }
                for (;;) {
                    if (*buf == ':') {
                        break;
                    } else if (!is_http_token(*buf)) {
                        set_error(parsing_status::unexpected_char);
                        return *this;
                    }
                    ++buf;
                    WEBPP_CHECK_EOF();
                }

                name_size = buf - name_start;
                if (name_size == 0) {
                    set_error(parsing_status::empty_name);
                    return *this;
                }
                ++buf;
                for (;; ++buf) {
                    WEBPP_CHECK_EOF();
                    if (!(*buf == ' ' || *buf == '\t')) {
                        break;
                    }
                }
            } else {
                name_start = nullptr;
                name_size  = 0;
            }
            const char* value;
            size_t      value_len;
            if ((buf = get_token_to_eol(buf, buf_end, &value, &value_len, ret)) == nullptr) {
                buf = buf_end = nullptr;
                return *this;
            }
            /* remove trailing SPs and HTABs */
            const char* value_size_ptr = value + value_len;
            for (; value_size_ptr != value; --value_size) {
                const char c = *(value_size_ptr - 1);
                if (!(c == ' ' || c == '\t')) {
                    break;
                }
            }
            value_start = value;
            value_size  = value_size_ptr - value;

            return *this;

#undef WEBPP_CHECK_EOF
#undef WEBPP_EXPECT_CHAR
        }

        constexpr headers_parser_iterator operator++(int) const noexcept {
            return headers_parser_iterator{*this}.operator++();
        }


        [[nodiscard]] constexpr parsing_status status() const noexcept {
            return status_value;
        }

        /**
         * Put the parsing error message to the specified `out`put string.
         **/
        template <typename StrT>
        constexpr void error_string(StrT& out) const {
            switch (status_value) {
                using enum parsing_status;
                case unknown: {
                    out += "Headers are not evaluated yet.";
                    break;
                }
                case bad_eof: {
                    out += "Didn't expect an EOF here.";
                    print_header(out);
                    break;
                }
                case unexpected_char: {
                    out += "Didn't expect character '";
                    out += *buf;
                    out += "' at this position.";
                    print_header(out);
                    break;
                }
                case empty_name: {
                    out += "Empty HTTP Header Field Names are not allowed.";
                    print_header(out);
                }
                case done: {
                    // no error
                    break;
                }
            }
        }

      private:
        constexpr void set_error(parsing_status err) noexcept {
            status_value = err;
            buf_end      = buf;
        }

        // This member function calculates how much of the buffer is necessary for debugging,
        // and puts that much of the headers to the specified output string.
        template <typename StrT>
        constexpr void print_header(StrT& out) const {
            // todo
        }
    };

} // namespace webpp::http

#endif
