// Created by moisrex on 5/3/23.

#ifndef WEBPP_IP_INET_PTON_HPP
#define WEBPP_IP_INET_PTON_HPP

#include "../common/meta.hpp"
#include "../std/std.hpp"
#include "ip.hpp"

#include <cstdint>
#include <cstring>

namespace webpp {


    enum struct inet_pton4_status {
        valid = 0,
        too_little_octets,    // not enough octets
        too_many_octets,      // found too many octets
        invalid_octet_range,  // at least one octet is not in range
        invalid_leading_zero, // the octet is starting with an invalid leading zero
        invalid_character,    // found a non-standard character
        bad_ending,           // The ip ended badly
        invalid_prefix,       // The ip has and invalid prefix
    };

    enum struct inet_pton6_status {
        valid = 0,
        invalid_octet_range, // at least one octet is not in range
        invalid_colon_usage, // the ip is using colon where it shouldn't
        bad_ending,          // the ip ended badly
        invalid_character,   // found a non-standard character
        invalid_prefix,      // The ip has and invalid prefix
    };

    /**
     * Get the error message as a string view
     */
    static constexpr const char* to_string(inet_pton4_status status) noexcept {
        switch (status) {
            case inet_pton4_status::valid: return "Valid IPv4 address";
            case inet_pton4_status::too_little_octets:
                return "The IPv4 doesn't have enough octets; it should contain exactly 4 octets";
            case inet_pton4_status::invalid_octet_range:
                return "At least one of the octets is of an invalid range";
            case inet_pton4_status::too_many_octets:
                return "The IPv4 has too many octets; it should contain exactly 4 octets";
            case inet_pton4_status::invalid_leading_zero:
                return "The IPv4's octet started with a zero which is not valid";
            case inet_pton4_status::invalid_character: return "Invalid character found in the IPv4";
            case inet_pton4_status::bad_ending: return "IPv4 ended unexpectedly";
            case inet_pton4_status::invalid_prefix: return "IPv4 has an invalid prefix";
        }
        return "";
    }

    /**
     * Get the error message as a string view
     */
    static constexpr const char* to_string(inet_pton6_status status) noexcept {
        switch (status) {
            case inet_pton6_status::valid: return "Valid IPv6 address";
            case inet_pton6_status::invalid_octet_range:
                return "At least one of the octets is of an invalid range.";
            case inet_pton6_status::invalid_colon_usage: return "The colon is used in the wrong place";
            case inet_pton6_status::bad_ending: return "The IPv6 ended unexpectedly";
            case inet_pton6_status::invalid_character: return "Invalid character found in the IPv6";
            case inet_pton6_status::invalid_prefix: return "IPv4 has an invalid prefix";
        }
        return "";
    }


    // NOLINTBEGIN(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    // NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers)

    //
    /**
     * Return the value of CH as a hexademical digit, or -1 if it is a different type of character.
     */
    template <typename IntegerType = int>
    static constexpr IntegerType hex_digit_value(char ch) noexcept {
        if ('0' <= ch && ch <= '9')
            return ch - '0';
        if ('a' <= ch && ch <= 'f')
            return ch - 'a' + char{10};
        if ('A' <= ch && ch <= 'F')
            return ch - 'A' + char{10};
        return -1;
    }

    namespace details {
        static constexpr int parse_prefix(const char*& src, const char* src_endp) noexcept {
            int prefix; // NOLINT(cppcoreguidelines-init-variables)
            if (src == src_endp || *src < '0' || *src > '9') {
                return -1;
            } else {
                prefix = *src - '0';
            }
            ++src;
            if (src == src_endp) {
                return prefix;
            } else if (*src < '0' || *src > '9') {
                return -1;
            } else {
                if (prefix == 0) {
                    return -1;
                }
                prefix *= 10;
                prefix += *src - '0';
            }
            ++src;
            if (src == src_endp) {
                return prefix;
            } else if (*src < '0' || *src > '9') {
                return -1;
            } else {
                prefix *= 10;
                prefix += *src - '0';
            }
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
    static constexpr inet_pton4_status
    inet_pton4(const char*& src, const char* end, stl::uint8_t* out) noexcept {
        bool saw_digit = false;
        int  octets    = 0;
        *out           = 0;
        while (src != end) {
            char const ch = *src++;
            if (ch >= '0' && ch <= '9') {
                unsigned int const new_i = *out * 10u + static_cast<unsigned int>(ch - '0');
                if (saw_digit && *out == 0) {
                    return inet_pton4_status::invalid_leading_zero;
                }
                if (new_i > 255) {
                    return inet_pton4_status::invalid_octet_range;
                }
                *out = static_cast<uint8_t>(new_i);
                if (!saw_digit) {
                    if (++octets > 4) {
                        return inet_pton4_status::too_many_octets;
                    }
                    saw_digit = true;
                }
            } else if (ch == '.' && saw_digit) {
                if (octets == 4) {
                    return inet_pton4_status::bad_ending;
                }
                *++out    = 0;
                saw_digit = false;
            } else {
                --src;
                return inet_pton4_status::invalid_character;
            }
        }
        if (octets < 4) {
            return inet_pton4_status::too_little_octets;
        }
        return inet_pton4_status::valid;
    }

    /**
     * Parse IPv4 + prefix
     */
    static constexpr inet_pton4_status
    inet_pton4(const char*& src, const char* end, stl::uint8_t* out, stl::uint8_t& prefix) noexcept {
        auto const res = inet_pton4(src, end, out);
        if (res == inet_pton4_status::invalid_character && *src == '/') {
            ++src;
            int const prefix_tmp = details::parse_prefix(src, end);
            if (prefix_tmp == -1 || prefix_tmp > 32) {
                return inet_pton4_status::invalid_prefix;
            }
            prefix = static_cast<stl::uint8_t>(prefix_tmp);
            return inet_pton4_status::valid;
        }
        return res;
    }

    static constexpr inet_pton4_status
    inet_pton4(const char* const& inp_src, const char* src_endp, stl::uint8_t* out) noexcept {
        const char* src = inp_src;
        return inet_pton4(src, src_endp, out);
    }

    static constexpr inet_pton4_status inet_pton4(const char* const& inp_src,
                                                  const char*        src_endp,
                                                  stl::uint8_t*      out,
                                                  stl::uint8_t&      prefix) noexcept {
        const char* src = inp_src;
        return inet_pton4(src, src_endp, out, prefix);
    }


    /**
     * Convert IPv6 Presentation string into network order binary form.
     *
     * This implementation is very similar to the one in glibc, but our version has different side effects.
     *
     * @returns status of the parsing
     **/
    static constexpr inet_pton6_status
    inet_pton6(const char*& src, const char* src_endp, stl::uint8_t* out) noexcept {
        stl::uint8_t* colonp = nullptr;
        stl::uint8_t* endp   = out + ipv6_byte_count;

        // Handling Leading ::
        if (src == src_endp) {
            return inet_pton6_status::bad_ending;
        }
        if (*src == ':') {
            ++src;
            if (src == src_endp || *src != ':') {
                return inet_pton6_status::invalid_colon_usage;
            }
        }

        const char*  current_token = src;
        stl::size_t  hex_seen      = 0; // Number of hex digits since colon.
        unsigned int val           = 0;
        char         ch; // NOLINT(cppcoreguidelines-init-variables)
        while (src != src_endp) {
            ch              = *src++;
            int const digit = hex_digit_value(ch);
            if (digit >= 0) {
                if (hex_seen == 4) {
                    return inet_pton6_status::invalid_octet_range;
                }
                val <<= 4;
                val |= static_cast<unsigned int>(digit);
                if (val > 0xffff) {
                    return inet_pton6_status::invalid_octet_range; // todo: is this if stmt even possible?
                }
                ++hex_seen;
                continue;
            } else if (ch == ':') {
                current_token = src;
                if (hex_seen == 0) {
                    if (colonp) {
                        return inet_pton6_status::invalid_colon_usage;
                    }
                    colonp = out;
                    continue;
                } else if (src == src_endp) {
                    return inet_pton6_status::bad_ending;
                }
                if (out + uint16_byte_count > endp) {
                    return inet_pton6_status::invalid_octet_range;
                }
                *out++   = static_cast<stl::uint8_t>((val >> 8) & 0xff);
                *out++   = static_cast<stl::uint8_t>(val & 0xff);
                hex_seen = 0;
                val      = 0;
                continue;
            } else if (ch == '.' && (out + ipv4_byte_count) <= endp) {
                src = current_token;
                switch (inet_pton4(src, src_endp, out)) {
                    case inet_pton4_status::valid: {
                        out += ipv4_byte_count;
                        hex_seen = 0;
                        break;
                    }
                    case inet_pton4_status::bad_ending:
                    case inet_pton4_status::too_little_octets:
                    case inet_pton4_status::too_many_octets: return inet_pton6_status::bad_ending;
                    case inet_pton4_status::invalid_octet_range:
                        return inet_pton6_status::invalid_octet_range;
                    case inet_pton4_status::invalid_leading_zero:
                    case inet_pton4_status::invalid_character: return inet_pton6_status::invalid_character;
                    case inet_pton4_status::invalid_prefix: return inet_pton6_status::invalid_prefix;
                }
                break;              // '\0' was seen by inet_pton4.
            } else if (ch == '/') { // handling prefixes
                --src;
                break;
            }
            --src;
            return inet_pton6_status::invalid_character;
        }
        if (hex_seen > 0) {
            if (out + uint16_byte_count > endp) {
                return inet_pton6_status::invalid_octet_range;
            }
            *out++ = static_cast<stl::uint8_t>((val >> 8) & 0xff);
            *out++ = static_cast<stl::uint8_t>(val & 0xff);
        }
        if (colonp != nullptr) {
            // Replace :: with zeros.
            if (out == endp) {
                // :: would expand to a zero-width field.
                return inet_pton6_status::bad_ending;
            }
            auto const n = static_cast<stl::size_t>(out - colonp);
            stl::memmove(endp - n, colonp, n);
            stl::memset(colonp, 0, static_cast<stl::size_t>(endp - n - colonp));
            out = endp;
        }
        if (out != endp) {
            return inet_pton6_status::bad_ending;
        }
        if (ch == '/') {
            return inet_pton6_status::invalid_character;
        }
        return inet_pton6_status::valid;
    }



    /**
     * Parse a ipv6 + prefix
     */
    static constexpr inet_pton6_status
    inet_pton6(const char*& src, const char* end, stl::uint8_t* out, stl::uint8_t& prefix) noexcept {
        auto const res = inet_pton6(src, end, out);
        if (res == inet_pton6_status::invalid_character && *src == '/') {
            ++src;
            int const prefix_tmp = details::parse_prefix(src, end);
            if (prefix_tmp == -1 || prefix_tmp > 128) {
                return inet_pton6_status::invalid_prefix;
            }
            prefix = static_cast<stl::uint8_t>(prefix_tmp);
            return inet_pton6_status::valid;
        }
        return res;
    }


    static constexpr inet_pton6_status
    inet_pton6(const char* const& inp_src, const char* src_endp, stl::uint8_t* out) noexcept {
        const char* src = inp_src;
        return inet_pton6(src, src_endp, out);
    }

    static constexpr inet_pton6_status inet_pton6(const char* const& inp_src,
                                                  const char*        src_endp,
                                                  stl::uint8_t*      out,
                                                  stl::uint8_t&      prefix) noexcept {
        const char* src = inp_src;
        return inet_pton6(src, src_endp, out, prefix);
    }

    // NOLINTEND(cppcoreguidelines-avoid-magic-numbers)
    // NOLINTEND(cppcoreguidelines-pro-bounds-pointer-arithmetic)

} // namespace webpp

#endif // WEBPP_IP_INET_PTON_HPP
