// Created by moisrex on 10/11/20.

#ifndef WEBPP_BASE64_URL_HPP
#define WEBPP_BASE64_URL_HPP

#include "./base64.hpp"

#include <algorithm>

namespace webpp::base64 {


    enum struct url_encode_policy {
        include_padding, // include the trailing padding in the output, when necessary.
        omit_padding     // remove the trailing padding from the output.
    };

    enum struct url_decode_policy {
        require_padding, // require inputs contain trailing padding if non-aligned.
        ignore_padding,  // accept inputs regardless of whether or not they have the correct padding.
        disallow_padding // reject inputs if they contain any trailing padding.
    };


    static constexpr auto padding_char = '=';

    // base64url maps {+, /} to {-, _} in order for the encoded content to be safe
    // to use in a URL. These characters will be translated by this implementation.
    static constexpr auto base64_chars          = "+/";
    static constexpr auto base64_url_safe_chars = "-_";


    /*
     * Encodes the |input| string in base64url, defined in RFC 4648:
     * https://tools.ietf.org/html/rfc4648#section-5
     *
     * The |policy| defines whether padding should be included or omitted from the
     * encoded |*output|. |input| and |*output| may reference the same storage.
     */
    template <url_encode_policy Policy = url_encode_policy::include_padding>
    static void url_encode(istl::StringViewfiable auto&& _input, istl::String auto& output) {
        auto input = istl::string_viewify(stl::forward<decltype(_input)>(_input));

        base64::encode(input, output);

        stl::replace(output.begin(), output.end(), '+', '-');
        stl::replace(output.begin(), output.end(), '/', '_');

        if constexpr (url_encode_policy::include_padding == Policy) {
            // The padding included in |*output| will not be amended.
        } else if constexpr (url_encode_policy::omit_padding == Policy) {
            // The padding included in |*output| will be removed.
            const size_t last_non_padding_pos = output.find_last_not_of(padding_char);
            if (last_non_padding_pos != output.npos)
                output.resize(last_non_padding_pos + 1);
        }
    }

    /*
     * Decodes the |input| string in base64url, defined in RFC 4648:
     * https://tools.ietf.org/html/rfc4648#section-5
     *
     * The |policy| defines whether padding will be required, ignored or disallowed
     * altogether. |input| and |*output| may reference the same storage.
     */
    template <url_decode_policy Policy = url_decode_policy::require_padding>
    [[nodiscard]] bool url_decode(istl::StringViewfiable auto&& _input, istl::String auto& output) {
        auto input  = istl::string_viewify(stl::forward<decltype(_input)>(_input));
        using str_t = stl::remove_cvref_t<decltype(output)>;
        using str_v = stl::remove_cvref_t<decltype(input)>;

        // Characters outside of the base64url alphabet are disallowed, which includes
        // the {+, /} characters found in the conventional base64 alphabet.
        if (input.find_first_of(base64_chars) != str_v::npos)
            return false;


        const size_t required_padding_characters = input.size() % 4;

        if constexpr (url_decode_policy::require_padding == Policy) {
            // Fail if the required padding is not included in |input|.
            if (required_padding_characters > 0)
                return false;
        } else if constexpr (url_decode_policy::ignore_padding == Policy) {
            // Missing padding will be silently appended.
        } else if constexpr (url_decode_policy::disallow_padding == Policy) {
            // Fail if padding characters are included in |input|.
            if (input.find_first_of(padding_char) != std::string::npos)
                return false;
        }

        const bool needs_replacement = input.find_first_of(base64_url_safe_chars) != str_v::npos;

        // If the string either needs replacement of URL-safe characters to normal
        // base64 ones, or additional padding, a copy of |input| needs to be made in
        // order to make these adjustments without side effects.
        if (required_padding_characters > 0 || needs_replacement) {
            str_t base64_input;

            size_t base64_input_size = input.size();
            if (required_padding_characters > 0)
                base64_input_size += 4 - required_padding_characters;

            base64_input.reserve(base64_input_size);
            base64_input.append(input.data(), input.size());

            // Substitute the base64url URL-safe characters to their base64 equivalents.
            stl::replace(base64_input.begin(), base64_input.end(), '-', '+');
            stl::replace(base64_input.begin(), base64_input.end(), '_', '/');

            // Append the necessary padding characters.
            base64_input.resize(base64_input_size, '=');

            return base64::decode(base64_input, output);
        }

        return base64::decode(input, output);
    }


} // namespace webpp::base64

#endif // WEBPP_BASE64_URL_HPP
