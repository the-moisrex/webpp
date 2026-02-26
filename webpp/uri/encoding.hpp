// Created by moisrex on 10/30/20.

#ifndef WEBPP_URI_ENCODING_HPP
#define WEBPP_URI_ENCODING_HPP

#include "../common/meta.hpp"
#include "../std/string.hpp"
#include "../std/string_view.hpp"
#include "../strings/charset.hpp"
#include "../strings/hex.hpp"

namespace webpp::uri {

    enum struct uri_encoding_policy : stl::uint8_t {
        skip_chars,  // allow the specified chars
        encode_chars // allow all the chars except these
    };

    template <istl::String OutT>
    static constexpr void encode_uri_component_set_capacity(typename OutT::size_type const input_size, OutT& output) {
        // 1.5 is a somewhat educated guess, if you have better guess, let us know.
        // todo: see if this is necessary/performant
        auto const new_capacity = output.size() + static_cast<stl::size_t>(static_cast<double>(input_size) * 1.5);
        if (output.capacity() < new_capacity) {
            output.reserve(new_capacity);
        }
    }

    template <typename Iter, typename CIter>
    static constexpr void encode_uri_component_set_capacity(Iter pos, CIter end, istl::String auto& output) {
        encode_uri_component_set_capacity(end - pos, output);
    }

    template <typename CharT, typename AllocT>
    static constexpr void encode_uri_component_set_capacity(
      stl::basic_string_view<CharT>                              str,
      stl::basic_string<CharT, stl::char_traits<CharT>, AllocT>& output) {
        encode_uri_component_set_capacity(str.size(), output);
    }

    /**
     * in-place version of uri component decoding, this is also nothrow since encoded version is always
     * longer than or equal to the decoded version thus we don't need allocations.
     */
    template <uri_encoding_policy Policy = uri_encoding_policy::skip_chars,
              typename Iter              = char*,
              typename ConstIter         = char const*>
    [[nodiscard]] static constexpr bool
    decode_uri_component_inplace(Iter& pos, ConstIter end, CharSet auto const& chars) noexcept {
        using char_type = stl::iter_value_t<Iter>;

        webpp_static_constexpr auto ones      = static_cast<int>(~0UL);
        webpp_static_constexpr auto zero_char = static_cast<char_type>('\0');

        auto out = pos;
        for (; pos != end; ++pos) {
            if (*pos == static_cast<char_type>('%')) {
                if (pos++ >= end - 2) [[unlikely]] {
                    return false;
                }

                int decoded_char  = ascii::hex_digit_safe<int>(*pos++, ones) << 4U;
                decoded_char     |= ascii::hex_digit_safe<int>(*pos, ones);

                if (decoded_char != ones) {
                    auto const decoded = static_cast<char_type>(decoded_char);
                    if constexpr (uri_encoding_policy::skip_chars == Policy) {
                        if (!chars.contains(decoded)) [[unlikely]] {
                            pos  = out;
                            *pos = zero_char;
                            return false; // bad decoded chars
                        }
                    } else {
                        if (chars.contains(decoded)) [[unlikely]] {
                            pos  = out;
                            *pos = zero_char;
                            return false; // bad decoded chars
                        }
                    }
                    *out++ = decoded;
                } else [[unlikely]] {
                    pos  = out;
                    *pos = zero_char;
                    return false;
                }
            } else {
                if constexpr (uri_encoding_policy::skip_chars == Policy) {
                    if (!chars.contains(*pos)) [[unlikely]] {
                        pos  = out;
                        *pos = zero_char;
                        return false; // bad chars
                    }
                } else {
                    if (chars.contains(*pos)) [[unlikely]] {
                        pos  = out;
                        *pos = zero_char;
                        return false; // bad chars
                    }
                }
                *out++ += *pos;
            }
        }
        pos = out;
        if (pos != end) {
            *pos = zero_char;
        }
        return true;
    }

    template <uri_encoding_policy Policy = uri_encoding_policy::skip_chars,
              typename Iter,
              typename CIter,
              istl::String OutStrT = stl::string>
    [[nodiscard]] static constexpr bool
    decode_uri_component(Iter& pos, CIter end, OutStrT& output, CharSet auto const& chars) {
        using char_type = stl::iter_value_t<Iter>;

        webpp_static_constexpr auto ones = static_cast<int>(~0UL);

        for (; pos != end; ++pos) {
            if (*pos == static_cast<char_type>('%')) {
                if (pos++ >= end - 2) [[unlikely]] {
                    return false;
                }

                int decoded_char  = ascii::hex_digit_safe<int>(*pos++, ones) << 4U;
                decoded_char     |= ascii::hex_digit_safe<int>(*pos, ones);

                if (decoded_char != ones) {
                    auto const decoded = static_cast<char_type>(decoded_char);
                    if constexpr (uri_encoding_policy::skip_chars == Policy) {
                        if (!chars.contains(decoded)) [[unlikely]] {
                            return false; // bad decoded chars
                        }
                    } else {
                        if (chars.contains(decoded)) [[unlikely]] {
                            return false; // bad decoded chars
                        }
                    }
                    output += decoded;
                } else [[unlikely]] {
                    return false;
                }
            } else {
                if constexpr (uri_encoding_policy::skip_chars == Policy) {
                    if (!chars.contains(*pos)) [[unlikely]] {
                        return false; // bad chars
                    }
                } else {
                    if (chars.contains(*pos)) [[unlikely]] {
                        return false; // bad chars
                    }
                }
                output += *pos;
            }
        }
        return true;
    }

    /**
     * @brief this function will decode parts of uri
     * @details this function is almost the same as "decodeURIComponent" in JavaScript
     */
    template <uri_encoding_policy Policy = uri_encoding_policy::skip_chars, typename CharT, typename AllocT>
    [[nodiscard]] static constexpr bool decode_uri_component(
      stl::basic_string_view<CharT> const                        encoded_str,
      stl::basic_string<CharT, stl::char_traits<CharT>, AllocT>& output,
      CharSet auto const&                                        chars) {
        auto pos = encoded_str.begin();
        return decode_uri_component<Policy>(pos, encoded_str.end(), output, chars);
    }

    /// encode one character and add it to the output
    template <uri_encoding_policy Policy = uri_encoding_policy::skip_chars, istl::CharType CharT>
    static constexpr void encode_uri_component(CharT inp_char, istl::String auto& output, CharSet auto const& chars) {
        using char_type   = CharT;
        using string_type = stl::remove_cvref_t<decltype(output)>;
        static_assert(stl::is_same_v<char_type, typename string_type::value_type>,
                      "The specified string do not have the same char type.");

        if constexpr (uri_encoding_policy::skip_chars == Policy) {
            if (chars.contains(inp_char)) {
                output += inp_char;
                return;
            }
        } else {
            if (!chars.contains(inp_char)) {
                output += inp_char;
                return;
            }
        }
        output += ascii::to_percent_hex<char_type>(inp_char);
    }

    template <uri_encoding_policy Policy = uri_encoding_policy::skip_chars, istl::CharType CharT, istl::String OutStrT>
    static constexpr bool encode_uri_component(
      CharT               inp_char,
      OutStrT&            output,
      CharSet auto const& policy_chars,
      CharSet auto const& invalid_chars) {
        static_assert(stl::is_same_v<CharT, typename OutStrT::value_type>,
                      "The specified string do not have the same char type.");

        if (invalid_chars.contains(inp_char)) [[unlikely]] {
            return false;
        }
        if constexpr (uri_encoding_policy::skip_chars == Policy) {
            if (policy_chars.contains(inp_char)) {
                output += inp_char;
                return true;
            }
        } else {
            if (!policy_chars.contains(inp_char)) {
                output += inp_char;
                return true;
            }
        }
        output += ascii::to_percent_hex<CharT>(inp_char);
        return true;
    }

    /**
     * This method encodes the given URI element.
     * What we are calling a "URI element" is any part of the URI
     * which is a sequence of characters that:
     * - may be percent-encoded
     * - if not percent-encoded, are in a restricted set of characters
     *
     * @param[in] src
     *     This is the element to encode.
     *
     * @param[in] output
     *     Output
     *
     * @param[in] policy_chars
     *     This is the set of characters that do not need to
     *     be percent-encoded.
     *
     * @return
     *     The encoded element is returned.
     *
     *
     * @details this function is almost the same as "encodeURIComponent" in JavaScript
     */
    template <uri_encoding_policy Policy = uri_encoding_policy::skip_chars, typename CharT = char>
    static constexpr void encode_uri_component(
      stl::basic_string_view<CharT> const input,
      istl::String auto&                  output,
      CharSet auto const&                 policy_chars) {
        for (auto const ith_char : input) {
            encode_uri_component<Policy>(ith_char, output, policy_chars);
        }
    }

    /// Iterator based encoding
    template <uri_encoding_policy Policy = uri_encoding_policy::skip_chars, typename Iter, typename CIter>
    static constexpr void
    encode_uri_component(Iter beg, CIter end, istl::String auto& output, CharSet auto const& chars) {
        for (auto pos = beg; pos != end; ++pos) {
            encode_uri_component<Policy>(*pos, output, chars);
        }
    }

    /// Encode the specified characters, otherwise if it's an invalid character, then return false.
    template <uri_encoding_policy Policy = uri_encoding_policy::skip_chars, typename Iter, typename CIter>
    [[nodiscard]] static constexpr bool encode_uri_component(
      Iter&               pos,
      CIter               end,
      istl::String auto&  output,
      CharSet auto const& policy_chars,
      CharSet auto const& invalid_chars) {
        for (; pos != end; ++pos) {
            if (!encode_uri_component<Policy>(*pos, output, policy_chars, invalid_chars)) [[unlikely]] {
                return false;
            }
        }
        return true;
    }

    /// Check if the next 2 characters are valid percent encoded ascii-hex digits.
    template <stl::forward_iterator Iter, stl::forward_iterator EIter = Iter>
    [[nodiscard]] static constexpr bool next_percent_encode(Iter& pos, EIter end) noexcept {
        using ascii::is_hex_digit;
        webpp_assume(*pos == '%');

        // NOLINTNEXTLINE(*-inc-dec-in-conditions)
        return pos + 2 <= end && is_hex_digit(*++pos) && is_hex_digit(*++pos);
    }




} // namespace webpp::uri

#endif // WEBPP_URI_ENCODING_HPP
