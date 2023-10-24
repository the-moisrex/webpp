// Created by moisrex on 10/30/20.

#ifndef WEBPP_URI_ENCODING_HPP
#define WEBPP_URI_ENCODING_HPP

#include "../common/meta.hpp"
#include "../std/string.hpp"
#include "../std/string_view.hpp"
#include "../strings/charset.hpp"

namespace webpp {

    enum struct uri_encoding_policy { allowed_chars, disallowed_chars };



    /**
     * in-place version of uri component decoding, this is also nothrow since encoded version is always
     * longer than or equal to the decoded version thus we don't need allocations.
     */
    template <uri_encoding_policy Policy = uri_encoding_policy::allowed_chars,
              stl::size_t         N,
              typename Iter      = char*,
              typename ConstIter = char const*>
    [[nodiscard]] bool
    decode_uri_component_inplace(Iter&                                       pos,
                                 ConstIter                                   end,
                                 charset<istl::char_type_of<Iter>, N> const& chars) noexcept {
        using char_type = istl::char_type_of<Iter>;

        static_assert(stl::same_as<char_type, istl::char_type_of<ConstIter>>,
                      "The beginning and the end of the same string are required.");

        stl::size_t digits_left  = 0;
        char_type   decoded_char = 0;
        char_type*  out          = pos;
        for (; pos != end; ++pos) {
            if (digits_left) {
                decoded_char <<= 4;
                if (*pos >= '0' && *pos <= '9') { // DIGITS
                    decoded_char += static_cast<char_type>(*pos - '0');
                } else if (*pos >= 'A' && *pos <= 'F') {                     // UPPER_HEX
                    decoded_char += static_cast<char_type>(*pos - 'A' + 10); // NOLINT(*-avoid-magic-numbers)
                } else if (*pos >= 'a' && *pos <= 'f') {                     // LOWER_HEX
                    decoded_char += static_cast<char_type>(*pos - 'a' + 10); // NOLINT(*-avoid-magic-numbers)
                } else {
                    pos  = out;
                    *pos = '\0';
                    return false;
                }
                --digits_left;

                if (digits_left == 0) {
                    *out++ = decoded_char;
                }
            } else if (*pos == '%') {
                // resetting:
                digits_left  = 2;
                decoded_char = 0;
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
    template <uri_encoding_policy Policy = uri_encoding_policy::allowed_chars, stl::size_t N>
    [[nodiscard]] bool
    decode_uri_component(istl::StringViewifiable auto&&                                 encoded_str,
                         istl::String auto&                                             output,
                         charset<istl::char_type_of_t<decltype(encoded_str)>, N> const& chars) {
        using char_type          = istl::char_type_of_t<decltype(encoded_str)>;
        stl::size_t digits_left  = 0;
        char_type   decoded_char = 0;
        for (const auto c : istl::string_viewify(encoded_str)) {
            if (digits_left) {
                decoded_char <<= 4;
                if (c >= '0' && c <= '9') { // DIGITS
                    decoded_char += static_cast<char_type>(c - '0');
                } else if (c >= 'A' && c <= 'F') {                        // UPPER_HEX
                    decoded_char += static_cast<char_type>(c - 'A' + 10); // NOLINT(*-avoid-magic-numbers)
                } else if (c >= 'a' && c <= 'f') {                        // LOWER_HEX
                    decoded_char += static_cast<char_type>(c - 'a' + 10); // NOLINT(*-avoid-magic-numbers)
                } else {
                    return false;
                }
                --digits_left;

                if (digits_left == 0) {
                    output += static_cast<char_type>(decoded_char);
                }
            } else if (c == '%') {
                // resetting:
                digits_left  = 2;
                decoded_char = 0;
            } else {
                if constexpr (uri_encoding_policy::allowed_chars == Policy) {
                    if (!chars.contains(c))
                        return false; // bad chars
                } else {
                    if (chars.contains(c))
                        return false; // bad chars
                }
                output += c;
            }
        }
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
    template <uri_encoding_policy Policy = uri_encoding_policy::allowed_chars, stl::size_t N>
    static constexpr void encode_uri_component(istl::StringViewifiable auto&&                         src,
                                               istl::String auto&                                     output,
                                               charset<istl::char_type_of_t<decltype(src)>, N> const& chars) {
        using char_type   = istl::char_type_of_t<decltype(src)>;
        using uchar_type  = stl::make_unsigned_t<char_type>;
        using string_type = stl::remove_cvref_t<decltype(output)>;
        static_assert(stl::is_same_v<char_type, typename string_type::value_type>,
                      "The specified string do not have the same char type.");
        webpp_static_constexpr auto make_hex_digit = [](auto value) constexpr noexcept -> char_type {
            if (value < 10) { // NOLINT(*-avoid-magic-numbers)
                return static_cast<char_type>(value + '0');
            } else {
                return static_cast<char_type>(value - 10 + 'A'); // NOLINT(*-avoid-magic-numbers)
            }
        };

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
            bool const need_conversion = (uri_encoding_policy::allowed_chars == Policy) // constexpr
                                         && chars.contains(*it);
            if (need_conversion) {
                output += *it;
            } else {
                output += '%';
                output += make_hex_digit(static_cast<uchar_type>(*it) >> 4u);
                output +=
                  make_hex_digit(static_cast<uchar_type>(*it) & 0x0Fu); // NOLINT(*-avoid-magic-numbers)
            }
        }
        output.shrink_to_fit();
    }



    /// https://url.spec.whatwg.org/#string-percent-encode-after-encoding
    static constexpr bool percent_encode() noexcept {
        return true;
    }


} // namespace webpp

#endif // WEBPP_URI_ENCODING_HPP
