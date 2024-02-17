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
        static constexpr CharT hex_to_binary_table[] = {
          0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 0, 0, 0, 0, 0, 0, 10, 11, 12, 13, 14, 15, 0,  0,  0,  0, 0,
          0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0,  0,  10, 11, 12, 13, 14, 15};

        // template <typename IntegerType = stl::uint8_t, IntegerType NAN = -1, bool Hex = true>
        // static constexpr IntegerType hex_to_binary_table_full[256] = {
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   0,
        //   1,
        //   2,
        //   3,
        //   4,
        //   5,
        //   6,
        //   7,
        //   8,
        //   9,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   (Hex ? 10 : NAN),
        //   (Hex ? 11 : NAN),
        //   (Hex ? 12 : NAN),
        //   (Hex ? 13 : NAN),
        //   (Hex ? 14 : NAN),
        //   (Hex ? 15 : NAN),
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   (Hex ? 10 : NAN),
        //   (Hex ? 11 : NAN),
        //   (Hex ? 12 : NAN),
        //   (Hex ? 13 : NAN),
        //   (Hex ? 14 : NAN),
        //   (Hex ? 15 : NAN),
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN,
        //   NAN};

        /// 0123456789abcdef
        template <typename CharT = char>
        static constexpr CharT
          hex_chars[17]{'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f', '\0'};


        template <typename CharT = char>
        static constexpr CharT percent_hex_table[1024]{
          '%', '0', '0', 0, '%', '0', '1', 0, '%', '0', '2', 0, '%', '0', '3', 0, '%', '0', '4', 0,
          '%', '0', '5', 0, '%', '0', '6', 0, '%', '0', '7', 0, '%', '0', '8', 0, '%', '0', '9', 0,
          '%', '0', 'A', 0, '%', '0', 'B', 0, '%', '0', 'C', 0, '%', '0', 'D', 0, '%', '0', 'E', 0,
          '%', '0', 'F', 0, '%', '1', '0', 0, '%', '1', '1', 0, '%', '1', '2', 0, '%', '1', '3', 0,
          '%', '1', '4', 0, '%', '1', '5', 0, '%', '1', '6', 0, '%', '1', '7', 0, '%', '1', '8', 0,
          '%', '1', '9', 0, '%', '1', 'A', 0, '%', '1', 'B', 0, '%', '1', 'C', 0, '%', '1', 'D', 0,
          '%', '1', 'E', 0, '%', '1', 'F', 0, '%', '2', '0', 0, '%', '2', '1', 0, '%', '2', '2', 0,
          '%', '2', '3', 0, '%', '2', '4', 0, '%', '2', '5', 0, '%', '2', '6', 0, '%', '2', '7', 0,
          '%', '2', '8', 0, '%', '2', '9', 0, '%', '2', 'A', 0, '%', '2', 'B', 0, '%', '2', 'C', 0,
          '%', '2', 'D', 0, '%', '2', 'E', 0, '%', '2', 'F', 0, '%', '3', '0', 0, '%', '3', '1', 0,
          '%', '3', '2', 0, '%', '3', '3', 0, '%', '3', '4', 0, '%', '3', '5', 0, '%', '3', '6', 0,
          '%', '3', '7', 0, '%', '3', '8', 0, '%', '3', '9', 0, '%', '3', 'A', 0, '%', '3', 'B', 0,
          '%', '3', 'C', 0, '%', '3', 'D', 0, '%', '3', 'E', 0, '%', '3', 'F', 0, '%', '4', '0', 0,
          '%', '4', '1', 0, '%', '4', '2', 0, '%', '4', '3', 0, '%', '4', '4', 0, '%', '4', '5', 0,
          '%', '4', '6', 0, '%', '4', '7', 0, '%', '4', '8', 0, '%', '4', '9', 0, '%', '4', 'A', 0,
          '%', '4', 'B', 0, '%', '4', 'C', 0, '%', '4', 'D', 0, '%', '4', 'E', 0, '%', '4', 'F', 0,
          '%', '5', '0', 0, '%', '5', '1', 0, '%', '5', '2', 0, '%', '5', '3', 0, '%', '5', '4', 0,
          '%', '5', '5', 0, '%', '5', '6', 0, '%', '5', '7', 0, '%', '5', '8', 0, '%', '5', '9', 0,
          '%', '5', 'A', 0, '%', '5', 'B', 0, '%', '5', 'C', 0, '%', '5', 'D', 0, '%', '5', 'E', 0,
          '%', '5', 'F', 0, '%', '6', '0', 0, '%', '6', '1', 0, '%', '6', '2', 0, '%', '6', '3', 0,
          '%', '6', '4', 0, '%', '6', '5', 0, '%', '6', '6', 0, '%', '6', '7', 0, '%', '6', '8', 0,
          '%', '6', '9', 0, '%', '6', 'A', 0, '%', '6', 'B', 0, '%', '6', 'C', 0, '%', '6', 'D', 0,
          '%', '6', 'E', 0, '%', '6', 'F', 0, '%', '7', '0', 0, '%', '7', '1', 0, '%', '7', '2', 0,
          '%', '7', '3', 0, '%', '7', '4', 0, '%', '7', '5', 0, '%', '7', '6', 0, '%', '7', '7', 0,
          '%', '7', '8', 0, '%', '7', '9', 0, '%', '7', 'A', 0, '%', '7', 'B', 0, '%', '7', 'C', 0,
          '%', '7', 'D', 0, '%', '7', 'E', 0, '%', '7', 'F', 0, '%', '8', '0', 0, '%', '8', '1', 0,
          '%', '8', '2', 0, '%', '8', '3', 0, '%', '8', '4', 0, '%', '8', '5', 0, '%', '8', '6', 0,
          '%', '8', '7', 0, '%', '8', '8', 0, '%', '8', '9', 0, '%', '8', 'A', 0, '%', '8', 'B', 0,
          '%', '8', 'C', 0, '%', '8', 'D', 0, '%', '8', 'E', 0, '%', '8', 'F', 0, '%', '9', '0', 0,
          '%', '9', '1', 0, '%', '9', '2', 0, '%', '9', '3', 0, '%', '9', '4', 0, '%', '9', '5', 0,
          '%', '9', '6', 0, '%', '9', '7', 0, '%', '9', '8', 0, '%', '9', '9', 0, '%', '9', 'A', 0,
          '%', '9', 'B', 0, '%', '9', 'C', 0, '%', '9', 'D', 0, '%', '9', 'E', 0, '%', '9', 'F', 0,
          '%', 'A', '0', 0, '%', 'A', '1', 0, '%', 'A', '2', 0, '%', 'A', '3', 0, '%', 'A', '4', 0,
          '%', 'A', '5', 0, '%', 'A', '6', 0, '%', 'A', '7', 0, '%', 'A', '8', 0, '%', 'A', '9', 0,
          '%', 'A', 'A', 0, '%', 'A', 'B', 0, '%', 'A', 'C', 0, '%', 'A', 'D', 0, '%', 'A', 'E', 0,
          '%', 'A', 'F', 0, '%', 'B', '0', 0, '%', 'B', '1', 0, '%', 'B', '2', 0, '%', 'B', '3', 0,
          '%', 'B', '4', 0, '%', 'B', '5', 0, '%', 'B', '6', 0, '%', 'B', '7', 0, '%', 'B', '8', 0,
          '%', 'B', '9', 0, '%', 'B', 'A', 0, '%', 'B', 'B', 0, '%', 'B', 'C', 0, '%', 'B', 'D', 0,
          '%', 'B', 'E', 0, '%', 'B', 'F', 0, '%', 'C', '0', 0, '%', 'C', '1', 0, '%', 'C', '2', 0,
          '%', 'C', '3', 0, '%', 'C', '4', 0, '%', 'C', '5', 0, '%', 'C', '6', 0, '%', 'C', '7', 0,
          '%', 'C', '8', 0, '%', 'C', '9', 0, '%', 'C', 'A', 0, '%', 'C', 'B', 0, '%', 'C', 'C', 0,
          '%', 'C', 'D', 0, '%', 'C', 'E', 0, '%', 'C', 'F', 0, '%', 'D', '0', 0, '%', 'D', '1', 0,
          '%', 'D', '2', 0, '%', 'D', '3', 0, '%', 'D', '4', 0, '%', 'D', '5', 0, '%', 'D', '6', 0,
          '%', 'D', '7', 0, '%', 'D', '8', 0, '%', 'D', '9', 0, '%', 'D', 'A', 0, '%', 'D', 'B', 0,
          '%', 'D', 'C', 0, '%', 'D', 'D', 0, '%', 'D', 'E', 0, '%', 'D', 'F', 0, '%', 'E', '0', 0,
          '%', 'E', '1', 0, '%', 'E', '2', 0, '%', 'E', '3', 0, '%', 'E', '4', 0, '%', 'E', '5', 0,
          '%', 'E', '6', 0, '%', 'E', '7', 0, '%', 'E', '8', 0, '%', 'E', '9', 0, '%', 'E', 'A', 0,
          '%', 'E', 'B', 0, '%', 'E', 'C', 0, '%', 'E', 'D', 0, '%', 'E', 'E', 0, '%', 'E', 'F', 0,
          '%', 'F', '0', 0, '%', 'F', '1', 0, '%', 'F', '2', 0, '%', 'F', '3', 0, '%', 'F', '4', 0,
          '%', 'F', '5', 0, '%', 'F', '6', 0, '%', 'F', '7', 0, '%', 'F', '8', 0, '%', 'F', '9', 0,
          '%', 'F', 'A', 0, '%', 'F', 'B', 0, '%', 'F', 'C', 0, '%', 'F', 'D', 0, '%', 'F', 'E', 0,
          '%', 'F', 'F', 0};
        // same as above, but it wouldn't support other charater types
        // "%00\0%01\0%02\0%03\0%04\0%05\0%06\0%07\0"
        // "%08\0%09\0%0A\0%0B\0%0C\0%0D\0%0E\0%0F\0"
        // "%10\0%11\0%12\0%13\0%14\0%15\0%16\0%17\0"
        // "%18\0%19\0%1A\0%1B\0%1C\0%1D\0%1E\0%1F\0"
        // "%20\0%21\0%22\0%23\0%24\0%25\0%26\0%27\0"
        // "%28\0%29\0%2A\0%2B\0%2C\0%2D\0%2E\0%2F\0"
        // "%30\0%31\0%32\0%33\0%34\0%35\0%36\0%37\0"
        // "%38\0%39\0%3A\0%3B\0%3C\0%3D\0%3E\0%3F\0"
        // "%40\0%41\0%42\0%43\0%44\0%45\0%46\0%47\0"
        // "%48\0%49\0%4A\0%4B\0%4C\0%4D\0%4E\0%4F\0"
        // "%50\0%51\0%52\0%53\0%54\0%55\0%56\0%57\0"
        // "%58\0%59\0%5A\0%5B\0%5C\0%5D\0%5E\0%5F\0"
        // "%60\0%61\0%62\0%63\0%64\0%65\0%66\0%67\0"
        // "%68\0%69\0%6A\0%6B\0%6C\0%6D\0%6E\0%6F\0"
        // "%70\0%71\0%72\0%73\0%74\0%75\0%76\0%77\0"
        // "%78\0%79\0%7A\0%7B\0%7C\0%7D\0%7E\0%7F\0"
        // "%80\0%81\0%82\0%83\0%84\0%85\0%86\0%87\0"
        // "%88\0%89\0%8A\0%8B\0%8C\0%8D\0%8E\0%8F\0"
        // "%90\0%91\0%92\0%93\0%94\0%95\0%96\0%97\0"
        // "%98\0%99\0%9A\0%9B\0%9C\0%9D\0%9E\0%9F\0"
        // "%A0\0%A1\0%A2\0%A3\0%A4\0%A5\0%A6\0%A7\0"
        // "%A8\0%A9\0%AA\0%AB\0%AC\0%AD\0%AE\0%AF\0"
        // "%B0\0%B1\0%B2\0%B3\0%B4\0%B5\0%B6\0%B7\0"
        // "%B8\0%B9\0%BA\0%BB\0%BC\0%BD\0%BE\0%BF\0"
        // "%C0\0%C1\0%C2\0%C3\0%C4\0%C5\0%C6\0%C7\0"
        // "%C8\0%C9\0%CA\0%CB\0%CC\0%CD\0%CE\0%CF\0"
        // "%D0\0%D1\0%D2\0%D3\0%D4\0%D5\0%D6\0%D7\0"
        // "%D8\0%D9\0%DA\0%DB\0%DC\0%DD\0%DE\0%DF\0"
        // "%E0\0%E1\0%E2\0%E3\0%E4\0%E5\0%E6\0%E7\0"
        // "%E8\0%E9\0%EA\0%EB\0%EC\0%ED\0%EE\0%EF\0"
        // "%F0\0%F1\0%F2\0%F3\0%F4\0%F5\0%F6\0%F7\0"
        // "%F8\0%F9\0%FA\0%FB\0%FC\0%FD\0%FE\0%FF";
    } // namespace details

    /// convert hex to it's integral value
    /// Attention: use this utility where you KNOW the input is valid hex character,
    /// otherwise use hex_digit_value
    template <typename ResType = unsigned, typename CharT = char>
    [[nodiscard]] static constexpr ResType hex_to_binary(CharT inp_char) noexcept {
        webpp_assume(inp_char >= static_cast<CharT>('0') && inp_char <= static_cast<CharT>('f'));
        return details::hex_to_binary_table<ResType>[static_cast<unsigned>(
          inp_char - static_cast<CharT>('0'))];
    }

    /**
     * Return the value of CH as a hexademical digit, or -1 if it is a different type of character.
     * Almost the same as hex_to_binary
     */
    template <typename IntegerType  = int,
              bool SupportUppercase = true,
              bool SupportHex       = true,
              typename CharT        = char>
    [[nodiscard]] static constexpr IntegerType hex_digit_safe(
      CharT       inp_char,
      IntegerType default_value = static_cast<IntegerType>(-1)) noexcept {
        if (static_cast<CharT>('0') <= inp_char && inp_char <= static_cast<CharT>('9')) {
            return static_cast<IntegerType>(inp_char - static_cast<CharT>('0'));
        }
        if constexpr (SupportHex) {
            if (static_cast<CharT>('a') <= inp_char && inp_char <= static_cast<CharT>('f')) {
                return static_cast<IntegerType>(inp_char - static_cast<CharT>('a') + static_cast<CharT>(10));
            }
            if constexpr (SupportUppercase) {
                if (static_cast<CharT>('A') <= inp_char && inp_char <= static_cast<CharT>('F')) {
                    return static_cast<IntegerType>(
                      inp_char - static_cast<CharT>('A') + static_cast<CharT>(10));
                }
            }
        }
        return default_value;
    }

    /**
     * Return the value of CH as a hexademical digit, or NotANumber if it is a different type of character.
     * I benchmarked it, and it seems like a lookup table has no value here.
     * todo: write an specific benchmark for the lookup-table implementation
     */
    template <typename IntegerType   = int,
              bool        SupportHex = true,
              IntegerType NotANumber = -1,
              typename CharT         = char>
    [[nodiscard]] static constexpr IntegerType hex_digit(CharT inp_char) noexcept {
        // if constexpr (sizeof(CharT) == sizeof(stl::uint8_t)) {
        //     return details::hex_to_binary_table_full<IntegerType, NotANumber, SupportHex>[static_cast<
        //       stl::uint8_t>(inp_char)];
        // } else {
        return hex_digit_safe<IntegerType, true, SupportHex>(inp_char, NotANumber);
        // }
    }

    template <typename CharT = char>
    [[nodiscard]] static constexpr bool is_lowercase_hex(CharT inp_char) noexcept {
        return (inp_char >= static_cast<CharT>('0') && inp_char <= static_cast<CharT>('9')) ||
               (inp_char >= static_cast<CharT>('a') && inp_char <= static_cast<CharT>('f'));
    }

    template <typename CharT = char>
    [[nodiscard]] static constexpr bool is_uppercase_hex(CharT inp_char) noexcept {
        return (inp_char >= static_cast<CharT>('0') && inp_char <= static_cast<CharT>('9')) ||
               (inp_char >= static_cast<CharT>('A') && inp_char <= static_cast<CharT>('F'));
    }

    template <typename CharT = char>
    [[nodiscard]] static constexpr bool is_hex_digit(CharT inp_char) noexcept {
        return (inp_char >= static_cast<CharT>('0') && inp_char <= static_cast<CharT>('9')) ||
               (inp_char >= static_cast<CharT>('A') && inp_char <= static_cast<CharT>('F')) ||
               (inp_char >= static_cast<CharT>('a') && inp_char <= static_cast<CharT>('f'));
    }

    /// attention: this only supports up to 2 hex characters
    template <typename CharT = char, typename IntType = int>
    [[nodiscard]] static constexpr CharT const* to_percent_hex(IntType inp_int) noexcept {
        // Equal to:
        //   output += '%';
        //   output += make_hex_digit(ch >> 4u);
        //   output += make_hex_digit(ch & 0x0Fu);
        webpp_assume(inp_int <= static_cast<IntType>(0xFFU) && inp_int >= static_cast<IntType>(0U));
        return details::percent_hex_table<CharT> + static_cast<uint8_t>(inp_int) * 4;
    }

    // NOLINTEND(*-avoid-c-arrays)
    // NOLINTEND(*-magic-numbers)

} // namespace webpp::ascii

#endif // WEBPP_STRING_HEX_HPP
