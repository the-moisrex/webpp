// Created by moisrex on 12/30/23.

#ifndef INET_PTON4_PARSER_HPP
#define INET_PTON4_PARSER_HPP

#include <cstdint>

enum struct inet_pton4_status {
    valid = 0,
    too_little_octets,    // not enough octets
    too_many_octets,      // found too many octets
    invalid_octet_range,  // at least one octet is not in range
    invalid_leading_zero, // the octet is starting with an invalid leading zero
    invalid_character     // found a non-standard character
};

namespace v1 {

    static constexpr inet_pton4_status inet_pton4(char const* src, char const* end, uint8_t* out) noexcept {
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
                    return inet_pton4_status::too_many_octets;
                }
                *++out    = 0;
                saw_digit = false;
            } else {
                return inet_pton4_status::invalid_character;
            }
        }
        if (octets < 4) {
            return inet_pton4_status::too_little_octets;
        }
        return inet_pton4_status::valid;
    }
} // namespace v1

namespace v2 {

    static constexpr inet_pton4_status inet_pton4(char const* src, char const* end, uint8_t* out) noexcept {
        bool saw_digit = false;
        int  octets    = 0;
        *out           = 0;
        while (src != end) {
            char const ch = *src++;
            switch (ch) {
                case '0':
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9': {
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
                    break;
                }
                case '.':
                    if (saw_digit) {
                        if (octets == 4) {
                            return inet_pton4_status::too_many_octets;
                        }
                        *++out    = 0;
                        saw_digit = false;
                        break;
                    }
                    [[fallthrough]];
                default: return inet_pton4_status::invalid_character;
            }
        }
        if (octets < 4) {
            return inet_pton4_status::too_little_octets;
        }
        return inet_pton4_status::valid;
    }
} // namespace v2

#endif // INET_PTON4_PARSER_HPP
