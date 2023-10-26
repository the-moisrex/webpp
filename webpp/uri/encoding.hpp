// Created by moisrex on 10/30/20.

#ifndef WEBPP_URI_ENCODING_HPP
#define WEBPP_URI_ENCODING_HPP

#include "../common/meta.hpp"
#include "../std/string.hpp"
#include "../std/string_view.hpp"
#include "../strings/charset.hpp"
#include "../strings/hex.hpp"

namespace webpp {

    enum struct uri_encoding_policy { allowed_chars, disallowed_chars };



    /**
     * in-place version of uri component decoding, this is also nothrow since encoded version is always
     * longer than or equal to the decoded version thus we don't need allocations.
     */
    template <uri_encoding_policy Policy = uri_encoding_policy::allowed_chars,
              typename Iter              = char*,
              typename ConstIter         = char const*>
    [[nodiscard]] static constexpr bool
    decode_uri_component_inplace(Iter& pos, ConstIter end, CharSet auto const& chars) noexcept {
        using char_type = istl::char_type_of_t<Iter>;

        static_assert(stl::same_as<char_type, istl::char_type_of_t<ConstIter>>,
                      "The beginning and the end of the same string are required.");

        char_type* out = pos;
        for (; pos != end; ++pos) {
            if (*pos == '%') {
                if (pos++ >= end - 2) [[unlikely]] {
                    return false;
                }

                int decoded_char = ascii::hex_digit_value<int>(*pos++, -1) << 4u;
                decoded_char |= ascii::hex_digit_value<int>(*pos, -1);

                if ((decoded_char & ~0xFF) == 0) [[likely]] { // NOLINT(*-avoid-magic-numbers)
                    *out++ = static_cast<char_type>(decoded_char);
                } else {
                    pos  = out;
                    *pos = '\0';
                    return false;
                }
            } else {
                if constexpr (uri_encoding_policy::allowed_chars == Policy) {
                    if (!chars.contains(*pos)) {
                        pos  = out;
                        *pos = '\0';
                        return false; // bad chars
                    }
                } else {
                    if (chars.contains(*pos)) {
                        pos  = out;
                        *pos = '\0';
                        return false; // bad chars
                    }
                }
                *out++ += *pos;
            }
        }
        pos = out;
        if (pos != end)
            *pos = '\0';
        return true;
    }


    /**
     * @brief this function will decode parts of uri
     * @details this function is almost the same as "decodeURIComponent" in javascript
     */
    template <uri_encoding_policy Policy = uri_encoding_policy::allowed_chars>
    [[nodiscard]] static constexpr bool decode_uri_component(istl::StringViewifiable auto&& encoded_str,
                                                             istl::String auto&             output,
                                                             CharSet auto const&            chars) {
        using char_type = istl::char_type_of_t<decltype(encoded_str)>;
        auto const str  = istl::string_viewify(encoded_str);
        for (auto it = str.begin(); it != str.end(); ++it) {
            if (*it == '%') {
                if (it++ >= str.end() - 2) [[unlikely]] {
                    return false;
                }

                int decoded_char = ascii::hex_digit_value<int>(*it++, -1) << 4u;
                decoded_char |= ascii::hex_digit_value<int>(*it, -1);

                if ((decoded_char & ~0xFF) == 0) [[likely]] { // NOLINT(*-avoid-magic-numbers)
                    output += static_cast<char_type>(decoded_char);
                } else {
                    return false;
                }
            } else {
                if constexpr (uri_encoding_policy::allowed_chars == Policy) {
                    if (!chars.contains(*it)) [[unlikely]] {
                        return false; // bad chars
                    }
                } else {
                    if (chars.contains(*it)) [[unlikely]] {
                        return false; // bad chars
                    }
                }
                output += *it;
            }
        }
        return true;
    }

    /// encode one character and add it to the output
    template <uri_encoding_policy Policy = uri_encoding_policy::allowed_chars, istl::CharType CharT>
    static constexpr void
    encode_uri_component(CharT ch, istl::String auto& output, CharSet auto const& chars) {
        using char_type   = CharT;
        using string_type = stl::remove_cvref_t<decltype(output)>;
        static_assert(stl::is_same_v<char_type, typename string_type::value_type>,
                      "The specified string do not have the same char type.");

        if constexpr (uri_encoding_policy::allowed_chars == Policy) {
            if (chars.contains(ch)) {
                output += ch;
                return;
            }
        }
        output += ascii::to_percent_hex<char_type>(ch);
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
    template <uri_encoding_policy Policy = uri_encoding_policy::allowed_chars>
    static constexpr void encode_uri_component(istl::StringViewifiable auto&& src,
                                               istl::String auto&             output,
                                               CharSet auto const&            chars) {
        const auto input      = istl::string_viewify(src);
        const auto input_size = input.size();
        auto       it         = input.data();
        const auto input_end  = it + input_size;
        { // todo: see if this is necessary/performant
            const auto new_capacity =
              output.size() +
              static_cast<stl::size_t>(static_cast<double>(input_size) * 1.5); // 1.5 is by chance
            if (output.capacity() < new_capacity)
                output.reserve(new_capacity);
        }
        for (; it != input_end; ++it) {
            encode_uri_component<Policy>(*it, output, chars);
        }
        output.shrink_to_fit();
    }



} // namespace webpp

#endif // WEBPP_URI_ENCODING_HPP
