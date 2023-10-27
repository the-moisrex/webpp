// Created by moisrex on 10/24/23.

#ifndef WEBPP_STRING_HEX_HPP
#define WEBPP_STRING_HEX_HPP

#include "../common/meta.hpp"

#include <cstdint>

namespace webpp::ascii {

    // NOLINTBEGIN(*-magic-numbers)
    // NOLINTBEGIN(*-avoid-c-arrays)
    namespace details {
        template <typename CharT = char>
        constexpr static CharT hex_to_binary_table[] = {
          0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 0, 0, 0, 0, 0, 0, 10, 11, 12, 13, 14, 15, 0,  0,  0,  0, 0,
          0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0,  0,  10, 11, 12, 13, 14, 15};

        template <typename CharT = char>
        static constexpr const CharT* hex_chars = "0123456789abcdef";


        template <typename CharT = char>
        static constexpr CharT percent_hex_table[1024] = "%00\0%01\0%02\0%03\0%04\0%05\0%06\0%07\0"
                                                         "%08\0%09\0%0A\0%0B\0%0C\0%0D\0%0E\0%0F\0"
                                                         "%10\0%11\0%12\0%13\0%14\0%15\0%16\0%17\0"
                                                         "%18\0%19\0%1A\0%1B\0%1C\0%1D\0%1E\0%1F\0"
                                                         "%20\0%21\0%22\0%23\0%24\0%25\0%26\0%27\0"
                                                         "%28\0%29\0%2A\0%2B\0%2C\0%2D\0%2E\0%2F\0"
                                                         "%30\0%31\0%32\0%33\0%34\0%35\0%36\0%37\0"
                                                         "%38\0%39\0%3A\0%3B\0%3C\0%3D\0%3E\0%3F\0"
                                                         "%40\0%41\0%42\0%43\0%44\0%45\0%46\0%47\0"
                                                         "%48\0%49\0%4A\0%4B\0%4C\0%4D\0%4E\0%4F\0"
                                                         "%50\0%51\0%52\0%53\0%54\0%55\0%56\0%57\0"
                                                         "%58\0%59\0%5A\0%5B\0%5C\0%5D\0%5E\0%5F\0"
                                                         "%60\0%61\0%62\0%63\0%64\0%65\0%66\0%67\0"
                                                         "%68\0%69\0%6A\0%6B\0%6C\0%6D\0%6E\0%6F\0"
                                                         "%70\0%71\0%72\0%73\0%74\0%75\0%76\0%77\0"
                                                         "%78\0%79\0%7A\0%7B\0%7C\0%7D\0%7E\0%7F\0"
                                                         "%80\0%81\0%82\0%83\0%84\0%85\0%86\0%87\0"
                                                         "%88\0%89\0%8A\0%8B\0%8C\0%8D\0%8E\0%8F\0"
                                                         "%90\0%91\0%92\0%93\0%94\0%95\0%96\0%97\0"
                                                         "%98\0%99\0%9A\0%9B\0%9C\0%9D\0%9E\0%9F\0"
                                                         "%A0\0%A1\0%A2\0%A3\0%A4\0%A5\0%A6\0%A7\0"
                                                         "%A8\0%A9\0%AA\0%AB\0%AC\0%AD\0%AE\0%AF\0"
                                                         "%B0\0%B1\0%B2\0%B3\0%B4\0%B5\0%B6\0%B7\0"
                                                         "%B8\0%B9\0%BA\0%BB\0%BC\0%BD\0%BE\0%BF\0"
                                                         "%C0\0%C1\0%C2\0%C3\0%C4\0%C5\0%C6\0%C7\0"
                                                         "%C8\0%C9\0%CA\0%CB\0%CC\0%CD\0%CE\0%CF\0"
                                                         "%D0\0%D1\0%D2\0%D3\0%D4\0%D5\0%D6\0%D7\0"
                                                         "%D8\0%D9\0%DA\0%DB\0%DC\0%DD\0%DE\0%DF\0"
                                                         "%E0\0%E1\0%E2\0%E3\0%E4\0%E5\0%E6\0%E7\0"
                                                         "%E8\0%E9\0%EA\0%EB\0%EC\0%ED\0%EE\0%EF\0"
                                                         "%F0\0%F1\0%F2\0%F3\0%F4\0%F5\0%F6\0%F7\0"
                                                         "%F8\0%F9\0%FA\0%FB\0%FC\0%FD\0%FE\0%FF";
    } // namespace details

    /// convert hex to it's integral value
    /// Attention: use this utility where you KNOW the input is valid hex character,
    /// otherwise use hex_digit_value
    template <typename ResType = unsigned, typename CharT = char>
    [[nodiscard]] static constexpr ResType hex_to_binary(CharT c) noexcept {
        webpp_assume(c >= '0' && c <= 'f');
        return details::hex_to_binary_table<ResType>[static_cast<unsigned>(c - '0')];
    }

    /**
     * Return the value of CH as a hexademical digit, or -1 if it is a different type of character.
     * Almost the same as hex_to_binary
     */
    template <typename IntegerType = int, typename CharT = char>
    [[nodiscard]] static constexpr IntegerType
    hex_digit_value(CharT ch, IntegerType default_value = static_cast<IntegerType>(-1)) noexcept {
        if ('0' <= ch && ch <= '9')
            return static_cast<IntegerType>(ch - '0');
        if ('a' <= ch && ch <= 'f')
            return static_cast<IntegerType>(ch - 'a' + static_cast<CharT>(10));
        if ('A' <= ch && ch <= 'F')
            return static_cast<IntegerType>(ch - 'A' + static_cast<CharT>(10));
        return default_value;
    }


    template <typename CharT = char>
    [[nodiscard]] static constexpr bool is_lowercase_hex(CharT c) noexcept {
        return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f');
    }

    template <typename CharT = char>
    [[nodiscard]] static constexpr bool is_uppercase_hex(CharT c) noexcept {
        return (c >= '0' && c <= '9') || (c >= 'A' && c <= 'F');
    }

    template <typename CharT = char>
    [[nodiscard]] static constexpr bool is_hex_digit(CharT c) noexcept {
        return (c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f');
    }

    /// attention: this only supports up to 2 hex characters
    template <typename CharT = char, typename IntType = int>
    [[nodiscard]] static constexpr const CharT* to_percent_hex(IntType inp) noexcept {
        // Equal to:
        //   output += '%';
        //   output += make_hex_digit(ch >> 4u);
        //   output += make_hex_digit(ch & 0x0Fu);
        webpp_assume(inp <= static_cast<IntType>(0xFFu) && inp >= static_cast<IntType>(0u));
        return details::percent_hex_table<CharT> + static_cast<uint8_t>(inp) * 4;
    }

    // NOLINTEND(*-avoid-c-arrays)
    // NOLINTEND(*-magic-numbers)

} // namespace webpp::ascii

#endif // WEBPP_STRING_HEX_HPP
