// Created by moisrex on 10/30/20.

#ifndef WEBPP_URI_ENCODING_HPP
#define WEBPP_URI_ENCODING_HPP

#include "../common/meta.hpp"
#include "../std/string.hpp"
#include "../std/string_view.hpp"
#include "../strings/charset.hpp"
#include "../strings/hex.hpp"

namespace webpp {

    enum struct uri_encoding_policy : stl::uint_fast8_t {
        allowed_chars,   // allow the specified chars
        disallowed_chars // allow all the chars except these
    };



    /**
     * in-place version of uri component decoding, this is also nothrow since encoded version is always
     * longer than or equal to the decoded version thus we don't need allocations.
     */
    template <uri_encoding_policy Policy = uri_encoding_policy::allowed_chars,
              typename Iter              = char*,
              typename ConstIter         = char const*>
    [[nodiscard]] static constexpr bool
    decode_uri_component_inplace(Iter& pos, ConstIter end, CharSet auto const& chars) noexcept {
        using iterator_traits = stl::iterator_traits<Iter>;
        using pointer         = typename iterator_traits::pointer;
        using char_type       = istl::char_type_of_t<pointer>;

        webpp_static_constexpr auto ones      = static_cast<int>(~0UL);
        webpp_static_constexpr auto zero_char = static_cast<char_type>('\0');

        auto out = pos;
        for (; pos != end; ++pos) {
            if (*pos == static_cast<char_type>('%')) {
                if (pos++ >= end - 2) [[unlikely]] {
                    return false;
                }

                int decoded_char = ascii::hex_digit_value<int>(*pos++, ones) << 4U;
                decoded_char |= ascii::hex_digit_value<int>(*pos, ones);

                if (decoded_char != ones) [[likely]] { // NOLINT(*-magic-numbers)
                    *out++ = static_cast<char_type>(decoded_char);
                } else {
                    pos  = out;
                    *pos = zero_char;
                    return false;
                }
            } else {
                if constexpr (uri_encoding_policy::allowed_chars == Policy) {
                    if (!chars.contains(*pos)) {
                        pos  = out;
                        *pos = zero_char;
                        return false; // bad chars
                    }
                } else {
                    if (chars.contains(*pos)) {
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


    template <uri_encoding_policy Policy = uri_encoding_policy::allowed_chars,
              typename Iter,
              typename CIter,
              istl::String OutStrT = stl::string>
    [[nodiscard]] static constexpr bool
    decode_uri_component(Iter& pos, CIter end, OutStrT& output, CharSet auto const& chars) {
        using char_type = istl::char_type_of_t<typename stl::iterator_traits<Iter>::pointer>;

        webpp_static_constexpr auto ones = static_cast<int>(~0UL);

        for (; pos != end; ++pos) {
            if (*pos == static_cast<char_type>('%')) {
                if (pos++ >= end - 2) [[unlikely]] {
                    return false;
                }

                int decoded_char = ascii::hex_digit_value<int>(*pos++, ones) << 4U;
                decoded_char |= ascii::hex_digit_value<int>(*pos, ones);

                if (decoded_char != ones) [[likely]] { // NOLINT(*-magic-numbers)
                    output += static_cast<char_type>(decoded_char);
                } else {
                    return false;
                }
            } else {
                if constexpr (uri_encoding_policy::allowed_chars == Policy) {
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
     * @details this function is almost the same as "decodeURIComponent" in javascript
     */
    template <uri_encoding_policy     Policy  = uri_encoding_policy::allowed_chars,
              istl::StringViewifiable StrVT   = stl::string_view,
              istl::String            OutStrT = stl::string>
    [[nodiscard]] static constexpr bool
    decode_uri_component(StrVT&& encoded_str, OutStrT& output, CharSet auto const& chars) {
        auto const str = istl::string_viewify(stl::forward<StrVT>(encoded_str));
        auto       pos = str.begin();
        return decode_uri_component(pos, str.end(), output, chars);
    }

    /// encode one character and add it to the output
    template <uri_encoding_policy Policy = uri_encoding_policy::allowed_chars, istl::CharType CharT>
    static constexpr void
    encode_uri_component(CharT inp_char, istl::String auto& output, CharSet auto const& chars) {
        using char_type   = CharT;
        using string_type = stl::remove_cvref_t<decltype(output)>;
        static_assert(stl::is_same_v<char_type, typename string_type::value_type>,
                      "The specified string do not have the same char type.");

        if constexpr (uri_encoding_policy::allowed_chars == Policy) {
            if (chars.contains(inp_char)) {
                output += inp_char;
                return;
            }
        }
        output += ascii::to_percent_hex<char_type>(inp_char);
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
     * @param[in] allowed_chars
     *     This is the set of characters that do not need to
     *     be percent-encoded.
     *
     * @return
     *     The encoded element is returned.
     *
     *
     * @details this function is almost the same as "encodeURIComponent" in javascript
     */
    template <uri_encoding_policy     Policy  = uri_encoding_policy::allowed_chars,
              istl::StringViewifiable InpStrT = stl::string_view>
    static constexpr void
    encode_uri_component(InpStrT&& src, istl::String auto& output, CharSet auto const& chars) {
        const auto input      = istl::string_viewify(stl::forward<InpStrT>(src));
        const auto input_size = input.size();
        { // todo: see if this is necessary/performant
            const auto new_capacity =
              output.size() +
              static_cast<stl::size_t>(static_cast<double>(input_size) * 1.5); // 1.5 is by chance
            if (output.capacity() < new_capacity) {
                output.reserve(new_capacity);
            }
        }
        for (auto const ith_char : input) {
            encode_uri_component<Policy>(ith_char, output, chars);
        }
        output.shrink_to_fit();
    }

    /// Iterator based encoding
    template <uri_encoding_policy Policy = uri_encoding_policy::allowed_chars, typename Iter, typename CIter>
    static constexpr void
    encode_uri_component(Iter beg, CIter end, istl::String auto& output, CharSet auto const& chars) {
        const auto input_size = end - beg;
        { // todo: see if this is necessary/performant
            const auto new_capacity =
              output.size() +
              static_cast<stl::size_t>(static_cast<double>(input_size) * 1.5); // 1.5 is by chance
            if (output.capacity() < new_capacity) {
                output.reserve(new_capacity);
            }
        }
        for (auto pos = beg; pos != end; ++pos) {
            encode_uri_component<Policy>(*pos, output, chars);
        }
        output.shrink_to_fit();
    }



} // namespace webpp

#endif // WEBPP_URI_ENCODING_HPP
