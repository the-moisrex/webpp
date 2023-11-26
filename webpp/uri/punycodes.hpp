#ifndef WEBPP_URL_PUNY_CODES_HPP
#define WEBPP_URL_PUNY_CODES_HPP

/**
 * Original Source Code from:
 * https://github.com/lemire/Code-used-on-Daniel-Lemire-s-blog/blob/master/2023/01/03/puny.h Most likely
 * written by Daniel Lemire Modified by Mohammad Bahoosh
 */

#include "../std/string.hpp"

#include <cstdint>
#include <cstring>

namespace webpp::uri {



    using punycode_uint = stl::uint32_t;


    enum struct punycode_status {
        success,
        overflow,
        big_output,
        bad_input,
    };

    /**
     * Converts an UTF-8 input into punycode.
     * This function is non-allocating and it does not throw.
     *
     * Parameters:
     *  - 'input' should be made of 'input_length' bytes representing a valid UTF-8 sequence.
     *  - 'input_length' should be at most 63.
     *  - output should have capacity for at least 63 bytes. It should not overlap with input.
     *  - We return how many bytes are written to 'output' or a negative integers in case of error.
     *    The errors include: an invalid UTF-8 input, a punycode overflow (unlikely) or
     *    an output that might exceed 63 bytes.
     */
    constexpr int utf8_to_punycode(char const* input, stl::size_t input_length, char* output) noexcept {
        // NOLINTBEGIN(*-magic-numbers)
        // NOLINTBEGIN(cppcoreguidelines-pro-bounds-pointer-arithmetic)


        if (input_length > 62) {
            return -1; // not allowed.
        }

        // Handle the basic code points
        auto* char_pointer = const_cast<unsigned char*>(reinterpret_cast<unsigned char const*>(input));
        unsigned char* const end_char_pointer = char_pointer + input_length;

        // first check if it is all ASCII
        bool is_all_ascii = true;
        {
            uint64_t pos = 0;
            for (; pos + 9 < input_length; pos += 8) {
                uint64_t v;
                std::memcpy(&v, char_pointer + pos, sizeof(uint64_t));
                if ((v & 0x8080'8080'8080'8080) != 0) {
                    is_all_ascii = false;
                    break;
                }
            }
            // process the tail byte-by-byte
            for (; is_all_ascii && pos < input_length; pos++) {
                if (char_pointer[pos] >= 0b1000'0000) {
                    is_all_ascii = false;
                    break;
                }
            }
        }
        if (is_all_ascii) {
            std::memcpy(output, input, input_length);
            output[input_length] = '-';
            return static_cast<int>(input_length + 1);
        }
        std::memcpy(output, "xn--", 4);
        output                            += 4;
        char constchar* const init_output  = output;

        stl::array<stl::uint32_t, 64> all_buffer;
        stl::uint32_t*                all{all_buffer.data()};
        stl::array<stl::uint32_t, 64> non_basic_buffer;
        stl::uint32_t*                non_basic{non_basic_buffer.data()};

        while (char_pointer < end_char_pointer) {
            unsigned char const c = *char_pointer;
            if (c >= 0b1000'0000) {
                auto const    lookahead = static_cast<stl::size_t const>(char_pointer - end_char_pointer);
                stl::uint32_t code_point;
                stl::uint32_t const leading_byte = c;

                if ((leading_byte & 0b1110'0000) == 0b1100'0000) {
                    // We have a two-byte UTF-8
                    if (lookahead < 2) {
                        return -1;
                    }

                    if ((char_pointer[1] & 0b1100'0000) != 0b1000'0000) {
                        return -1;
                    }
                    // range check
                    code_point = (leading_byte & 0b0001'1111) << 6 | (char_pointer[1] & 0b0011'1111);
                    if (code_point < 0x80 || 0x7ff < code_point) {
                        return -1;
                    }
                    char_pointer += 2;
                } else if ((leading_byte & 0b1111'0000) == 0b1110'0000) {
                    // We have a three-byte UTF-8
                    if (lookahead < 3) {
                        return -1;
                    }
                    if ((char_pointer[1] & 0b1100'0000) != 0b1000'0000) {
                        return -1;
                    }
                    if ((char_pointer[2] & 0b1100'0000) != 0b1000'0000) {
                        return -1;
                    }
                    // range check
                    code_point = (leading_byte & 0b0000'1111) << 12 |
                                 static_cast<stl::uint32_t>((char_pointer[1] & 0b0011'1111) << 6) |
                                 (char_pointer[2] & 0b0011'1111);
                    if (code_point < 0x800 || 0xffff < code_point ||
                        (0xd7ff < code_point && code_point < 0xe000))
                    {
                        return -1;
                    }
                    char_pointer += 3;
                } else if ((leading_byte & 0b1111'1000) == 0b1111'0000) { // 0b11110000
                    // we have a 4-byte UTF-8 word.
                    if (lookahead < 4) {
                        return -1;
                    }

                    if ((char_pointer[1] & 0b1100'0000) != 0b1000'0000) {
                        return -1;
                    }
                    if ((char_pointer[2] & 0b1100'0000) != 0b1000'0000) {
                        return -1;
                    }
                    if ((char_pointer[3] & 0b1100'0000) != 0b1000'0000) {
                        return -1;
                    }

                    // range check
                    code_point = (leading_byte & 0b0000'0111) << 18 |
                                 static_cast<stl::uint32_t>((char_pointer[1] & 0b0011'1111) << 12) |
                                 static_cast<stl::uint32_t>((char_pointer[2] & 0b0011'1111) << 6) |
                                 (char_pointer[3] & 0b0011'1111);
                    if (code_point <= 0xffff || 0x10'ffff < code_point) {
                        return -1;
                    }
                    char_pointer += 4;
                } else {
                    // continuation byte
                    return -1;
                }
                *non_basic++ = code_point;
                *all++       = code_point;
                continue;
            }
            // It's ASCII here!!!
            *output++ = static_cast<char>(c);
            *all++    = c;
            char_pointer++;
        }

        auto const number_of_chars = static_cast<stl::size_t const>(all - all_buffer.data());
        auto const basic_count     = static_cast<const stl::size_t>(output - init_output);

        if (basic_count > 0) {
            *output++ = '-';
        }

        stl::uint32_t n     = 128;
        stl::uint32_t bias  = 72;
        stl::uint32_t delta = 0;
        auto const    sort_unique_values =
          [](stl::array<stl::uint32_t, 64>& array, stl::size_t const size) noexcept {
              stl::size_t duplicates = 0;
              for (stl::size_t k = 1; k < size; k++) {
                  stl::size_t         z   = k - duplicates;
                  stl::uint32_t const key = array[k];
                  for (; (z >= 1) && (array[z - 1] >= key); z--) {
                  }
                  if (z == k) {
                      // nothing to do!
                  } else if ((array[z] > key)) {
                      std::memmove(array.data() + z + 1,
                                   array.data() + z,
                                   (k - duplicates - z) * sizeof(stl::uint32_t));
                      array[z] = key;
                  } else if (array[z] == key) {
                      duplicates++;
                  } else {
                      array[z] = key;
                  }
              }
          };

        sort_unique_values(non_basic_buffer,
                           static_cast<const stl::size_t>(non_basic - non_basic_buffer.data()));
        non_basic = non_basic_buffer.data();

        auto const adapt = [](stl::uint32_t delta, stl::uint32_t n_points, bool is_first) noexcept {
            delta /= is_first ? 700 : 2;
            delta += delta / n_points;

            stl::uint32_t const s = 36 - 1;
            stl::uint32_t const t = (s * 26) / 2;

            stl::uint32_t k = 0;
            for (; delta > t; k += 36) {
                delta /= s;
            }

            stl::uint32_t const a = (36 - 1 + 1) * delta;
            stl::uint32_t const b = (delta + 38);

            return k + (a / b);
        };


        for (stl::size_t processed = basic_count; processed < number_of_chars; ++n, ++delta) {
            stl::uint32_t const non_ascii_code_point  = *non_basic++;
            delta                                    += (non_ascii_code_point - n) * (processed + 1);
            n                                         = non_ascii_code_point;
            for (stl::size_t i = 0; i < number_of_chars; i++) {
                stl::uint32_t const c = all_buffer[i];
                if (c < n && (++delta == 0)) { // overflow
                    return -1;
                }
                if (c == n) {
                    for (stl::uint32_t q = delta, k = 36;; k += 36) {
                        if (output >= init_output + 63) {
                            return -1; // too long
                        }
                        stl::uint32_t const t = k <= bias ? 1 : (k >= bias + 26 ? 26 : k - bias);
                        if (q < t) {
                            *output++ = static_cast<char>(q < 26 ? q + 97 : q + 22);
                            break;
                        }
                        stl::uint32_t const char_value = t + (q - t) % (36 - t); // unfortunate division
                        *output++ = static_cast<char>(char_value < 26 ? char_value + 97 : char_value + 22);
                        q         = (q - t) / (36 - t);
                    }

                    bias =
                      adapt(delta, static_cast<stl::uint32_t const>(processed + 1), basic_count == processed);
                    delta = 0;
                    processed++;
                }
            }
        }
        return static_cast<int>(output - init_output + 4);
        // NOLINTEND(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        // NOLINTEND(*-magic-numbers)
    }

} // namespace webpp::uri


#endif // WEBPP_URL_PUNY_CODES_HPP
