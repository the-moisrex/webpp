// Created by moisrex on 10/11/20.

#ifndef WEBPP_BASE64_HPP
#define WEBPP_BASE64_HPP

#include "../std/string_view.hpp"
#include "../std/string.hpp"
#include "../libs/modp_b64/modp_b64.hpp"

namespace webpp::base64 {


    /**
     * Encodes the input binary data in base64.
     */
     void encode(istl::ConvertibleToStringView auto&& input, istl::String auto& output) {
        auto input_view = istl::to_string_view(stl::forward<decltype(input)>(input));
        output.resize(modp_b64::encode_len(input_view.size()));  // makes room for null byte

        // modp_b64::encode_len() returns at least 1, so output[0] is safe to use.
        const size_t output_size = modp_b64::encode(&output[0], input_view.data(), input_view.size());
        output.resize(output_size);
    }

    /**
     * Decodes the base64 input string.
     * Returns true if successful and false otherwise.
     * The output string is only modified if successful.
     * The decoding can be done in-place.
     */
    [[nodiscard]] bool decode(istl::ConvertibleToStringView auto &&input, istl::String auto& output) {
        using str_t = stl::remove_pointer_t<stl::remove_cvref_t<decltype(output)>>;
        auto input_view = istl::to_string_view(stl::forward<decltype(input)>(input));
        str_t temp(output.get_allocator());
        temp.resize(modp_b64::decode_len(input_view.size()));

        // does not null terminate result since result is binary data!
        stl::size_t input_size = input_view.size();
        stl::size_t output_size = modp_b64::decode(&(temp[0]), input_view.data(), input_size);
        if (output_size == modp_b64::error)
            return false;

        temp.resize(output_size);
        output.swap(temp);
        return true;
    }

}

#endif // WEBPP_BASE64_HPP
