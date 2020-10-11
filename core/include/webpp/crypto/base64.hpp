// Created by moisrex on 10/11/20.

#ifndef WEBPP_BASE64_HPP
#define WEBPP_BASE64_HPP

#include "../std/string_view.hpp"

namespace webpp::base64 {
    /**
     * Encodes the input binary data in base64.
     */
    std::string encode(span<const uint8_t> input);

    /**
     * Encodes the input string in base64.
     */
    void encode(istl::ConvertibleToStringView auto&& input, std::string* output);

    /**
     * Decodes the base64 input string.
     * Returns true if successful and false otherwise.
     * The output string is only modified if successful.
     * The decoding can be done in-place.
     */
    bool decode(istl::ConvertibleToStringView auto &&input, std::string* output);

}

#endif // WEBPP_BASE64_HPP
