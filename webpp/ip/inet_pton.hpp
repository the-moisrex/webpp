// Created by moisrex on 5/3/23.

#ifndef WEBPP_IP_INET_PTON_HPP
#define WEBPP_IP_INET_PTON_HPP

#include "../std/string_view.hpp"
#include "../strings/hex.hpp"
#include "ip.hpp"

#include <cstdint>
#include <cstring>

namespace webpp {


    /**
     * Get the error message as a string view
     */
    static constexpr stl::string_view to_string(inet_pton4_status const status) noexcept {
        switch (status) {
            using enum inet_pton4_status;
            case valid: return {"Valid IPv4 address"};
            case valid_special:
                return {"Valid IPv4 address, also found the specified special character at the end"};
            case too_little_octets:
                return {"The IPv4 doesn't have enough octets; it should contain exactly 4 octets"};
            case invalid_octet_range: return {"At least one of the IPv4 octets is of an invalid range"};
            case too_many_octets: return {"The IPv4 has too many octets; it should contain exactly 4 octets"};
            case invalid_leading_zero: return {"The IPv4's octet started with a zero which is not valid"};
            case invalid_character: return {"Invalid character found in the IPv4"};
            case bad_ending: return {"IPv4 ended unexpectedly"};
            case invalid_prefix: return {"IPv4 has an invalid prefix"};
        }
        stl::unreachable();
    }

    /**
     * Get the error message as a string view
     */
    static constexpr stl::string_view to_string(inet_pton6_status const status) noexcept {
        switch (status) {
            using enum inet_pton6_status;
            case valid: return {"Valid IPv6 address"};
            case valid_special:
                return {"Valid IPv6 address, also found the specified special character at the end"};
            case invalid_octet_range: return {"At least one of the IPv6 octets is of an invalid range."};
            case invalid_colon_usage: return {"The colon is used in the wrong place in IPv6"};
            case bad_ending: return {"The IPv6 ended unexpectedly"};
            case invalid_character: return {"Invalid character found in the IPv6"};
            case invalid_prefix: return {"IPv6 has an invalid prefix"};
        }
        stl::unreachable();
    }

    // NOLINTBEGIN(*-easily-swappable-parameters)
    // NOLINTBEGIN(*-pro-bounds-pointer-arithmetic)
    // NOLINTBEGIN(*-magic-numbers)

    namespace details {
        template <typename Iter = char const*, typename CIter = Iter>
        static constexpr int parse_prefix(Iter& src, CIter src_endp) noexcept {
            if (src == src_endp || *src < '0' || *src > '9') {
                return -1;
            }
            int prefix = *src - '0';
            ++src;
            if (src == src_endp) {
                return prefix;
            }
            if (*src < '0' || *src > '9') {
                return -1;
            }

            if (prefix == 0) {
                return -1;
            }
            prefix *= 10;
            prefix += *src - '0';

            ++src;
            if (src == src_endp) {
                return prefix;
            }
            if (*src < '0' || *src > '9') {
                return -1;
            }

            prefix *= 10;
            prefix += *src - '0';

            ++src;
            if (src != src_endp) {
                return -1;
            }
            return prefix;
        }
    } // namespace details

    /**
     * Converts the string representation of an IPv4 into it's network binary format.
     * The output is changed to invalid value when the input is not correct.
     *
     * This implementation is very similar to the one in glibc, but our version has different side effects.
     *
     * @returns status of the parsing
     **/
    template <typename Iter = char const*, typename CIter = Iter, istl::CharType CharT = char>
    static constexpr inet_pton4_status
    inet_pton4(Iter& src, CIter end, stl::uint8_t* out, CharT special_character = '/') noexcept {
        using enum inet_pton4_status;

        bool saw_digit = false;
        int  octets    = 0;
        *out           = 0;
        while (src != end) {
            auto const cur_char = *src++;
            if (cur_char >= '0' && cur_char <= '9') {
                unsigned int const new_i = *out * 10U + static_cast<unsigned int>(cur_char - '0');
                if (saw_digit && *out == 0) {
                    return invalid_leading_zero;
                }
                if (new_i > 255) {
                    return invalid_octet_range;
                }
                *out = static_cast<uint8_t>(new_i);
                if (!saw_digit) {
                    if (++octets > 4) {
                        return too_many_octets;
                    }
                    saw_digit = true;
                }
            } else if (cur_char == '.' && saw_digit) {
                if (octets == 4) {
                    return bad_ending;
                }
                *++out    = 0;
                saw_digit = false;
            } else {
                --src;
                if (octets == 4 && cur_char == special_character) {
                    return valid_special;
                }
                return invalid_character;
            }
        }
        if (octets != 4) {
            return too_little_octets;
        }
        return valid;
    }

    /**
     * Parse IPv4 + prefix
     */
    template <typename Iter = char const*, typename CIter = Iter, istl::CharType CharT = char>
    static constexpr inet_pton4_status inet_pton4(
      Iter&         src,
      CIter         end,
      stl::uint8_t* out,
      stl::uint8_t& prefix,
      CharT         prefix_character = '/') noexcept {
        using enum inet_pton4_status;
        auto const res = inet_pton4(src, end, out, prefix_character);
        if (res == valid_special && *src == prefix_character) {
            ++src;
            int const prefix_tmp = details::parse_prefix(src, end);
            if (prefix_tmp == -1 || prefix_tmp > 32) {
                return invalid_prefix;
            }
            prefix = static_cast<stl::uint8_t>(prefix_tmp);
            return res;
        }
        return res;
    }

    /**
     * Convert IPv6 Presentation string into network order binary form.
     *
     * This implementation is very similar to the one in glibc, but our version has different side effects.
     *
     * @returns status of the parsing
     **/
    template <typename Iter = char const*, typename CIter = Iter, istl::CharType CharT = char>
    static constexpr inet_pton6_status
    inet_pton6(Iter& src, CIter src_endp, stl::uint8_t* out, CharT special_character = '/') noexcept {
        using enum inet_pton6_status;

        using char_type = istl::char_type_of_t<stl::iterator_traits<Iter>>;

        stl::uint8_t*             colon_ptr = nullptr;
        stl::uint8_t const* const beg       = out;
        stl::uint8_t*             endp      = out + ipv6_byte_count;

        // Handling Leading ::
        if (src == src_endp) {
            return bad_ending;
        }
        if (*src == ':') {
            ++src;
            if (src == src_endp || *src != ':') {
                return invalid_colon_usage;
            }
        }

        auto         current_token = src;
        stl::size_t  hex_seen      = 0; // Number of hex digits since colon.
        unsigned int val           = 0;
        char_type    cur_char;          // NOLINT(*-init-variables)
        while (src != src_endp) {
            cur_char        = *src++;
            int const digit = ascii::hex_digit_safe(cur_char);
            if (digit >= 0) {
                if (hex_seen == 4) {
                    return invalid_octet_range;
                }
                val <<= 4U;
                val  |= static_cast<unsigned int>(digit);
                if (val > 0xFFFF) {
                    return invalid_octet_range; // todo: is this if stmt even possible?
                }
                ++hex_seen;
                continue;
            }
            if (cur_char == ':') {
                current_token = src;
                if (hex_seen == 0) {
                    if (colon_ptr != nullptr) {
                        return invalid_colon_usage;
                    }
                    colon_ptr = out;
                    continue;
                }
                if (src == src_endp) {
                    return bad_ending;
                }
                if (out + uint16_byte_count > endp) {
                    return invalid_octet_range;
                }
                *out++   = static_cast<stl::uint8_t>((val >> 8U) & 0xFFU);
                *out++   = static_cast<stl::uint8_t>(val & 0xFFU);
                hex_seen = 0;
                val      = 0;
                continue;
            }
            if (cur_char == '.' &&                   // <-- possible ipv4 dot character
                (out + ipv4_byte_count) <= endp &&   // <-- have enough octets of the ipv6 left for ipv4, and
                (out != beg || colon_ptr != nullptr) // <-- we're not at the beginning of the string
            )
            {
                src = current_token;
                switch (inet_pton4(src, src_endp, out)) {
                    case inet_pton4_status::valid_special: cur_char = *src; [[fallthrough]];
                    case inet_pton4_status::valid: {
                        out      += ipv4_byte_count;
                        hex_seen  = 0;
                        break;
                    }
                    case inet_pton4_status::bad_ending:
                    case inet_pton4_status::too_little_octets:
                    case inet_pton4_status::invalid_leading_zero:
                    case inet_pton4_status::too_many_octets: return bad_ending;
                    case inet_pton4_status::invalid_octet_range: return invalid_octet_range;
                    case inet_pton4_status::invalid_character: return invalid_character;
                    case inet_pton4_status::invalid_prefix: return invalid_prefix;
                }
                break; // '\0' or special character was seen by inet_pton4.
            }
            if (cur_char == special_character) {
                // handling prefixes ('/'), or the closing char of a host IPv6 (']')
                --src;
                break;
            }
            --src;
            return invalid_character;
        }
        if (hex_seen > 0) {
            if (out + uint16_byte_count > endp) {
                return invalid_octet_range;
            }
            *out++ = static_cast<stl::uint8_t>((val >> 8U) & 0xFFU);
            *out++ = static_cast<stl::uint8_t>(val & 0xFFU);
        }
        if (colon_ptr != nullptr) {
            // Replace :: with zeros.
            if (out == endp) {
                // :: would expand to a zero-width field.
                return bad_ending;
            }

            // constexpr-friendly equivalent of:
            // auto const n      = static_cast<stl::size_t>(out - colonp);
            // stl::memmove(endp - n, colonp, n);
            // stl::memset(colonp, 0, static_cast<stl::size_t>(endp - n - colonp));

            // auto count = endp - out;
            // auto p = out + count;
            // for (; out != colonp - 1; ) {
            //     *--p = *--out;
            // }
            // for (; count-- != 0; *colonp++ = 0)
            //     ;

            // another constexpr-friendly way of doing the same thing:
            auto* right_ptr = endp;
            for (; out != colon_ptr;) {
                *--right_ptr = *--out;
            }
            for (; colon_ptr != right_ptr; *colon_ptr++ = 0) {
            }
            out = endp;
        }
        if (out != endp) {
            return bad_ending;
        }
        if (cur_char == special_character) {
            return valid_special;
        }
        return valid;
    }

    /**
     * Parse a ipv6 + prefix
     */
    template <typename Iter = char const*, typename CIter = Iter, istl::CharType CharT = char>
    [[nodiscard]] static constexpr inet_pton6_status inet_pton6(
      Iter&         src,
      CIter         end,
      stl::uint8_t* out,
      stl::uint8_t& prefix,
      CharT         prefix_character = '/') noexcept {
        using enum inet_pton6_status;
        auto const res = inet_pton6(src, end, out);
        if (res == valid_special && *src == prefix_character) {
            ++src;
            int const prefix_tmp = details::parse_prefix(src, end);
            if (prefix_tmp == -1 || prefix_tmp > 128) {
                return invalid_prefix;
            }
            prefix = static_cast<stl::uint8_t>(prefix_tmp);
            return res;
        }
        return res;
    }

    // NOLINTEND(*-magic-numbers)
    // NOLINTEND(*-pro-bounds-pointer-arithmetic)
    // NOLINTEND(*-easily-swappable-parameters)

} // namespace webpp

#endif // WEBPP_IP_INET_PTON_HPP
