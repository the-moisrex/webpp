// Created by moisrex on 10/11/20.

#ifndef WEBPP_BASE64_HPP
#define WEBPP_BASE64_HPP

#include "../libs/modp_b64/modp_b64.hpp"
#include "../std/string.hpp"
#include "../std/string_view.hpp"

namespace webpp::base64 {


    /**
     * Encodes the input binary data in base64.
     */
    void encode(istl::StringViewifiable auto&& input, istl::String auto& output) {
        auto input_view = istl::string_viewify(stl::forward<decltype(input)>(input));
        output.resize(modp_b64::encode_len(input_view.size())); // makes room for null byte

        // modp_b64::encode_len() returns at least 1, so output[0] is safe to use.
        size_t const output_size = modp_b64::encode(&output[0], input_view.data(), input_view.size());
        output.resize(output_size);
    }

    /**
     * Decodes the base64 input string.
     * Returns true if successful and false otherwise.
     * The output string is only modified if successful.
     * The decoding can be done in-place.
     */
    [[nodiscard]] bool decode(istl::StringViewifiable auto&& input, istl::String auto& output) {
        using str_t      = stl::remove_pointer_t<stl::remove_cvref_t<decltype(output)>>;
        auto  input_view = istl::string_viewify(stl::forward<decltype(input)>(input));
        str_t temp(output.get_allocator());
        temp.resize(modp_b64::decode_len(input_view.size()));

        // does not null terminate result since result is binary data!
        stl::size_t const input_size  = input_view.size();
        stl::size_t       output_size = modp_b64::decode(&(temp[0]), input_view.data(), input_size);
        if (output_size == modp_b64::error) {
            return false;
        }

        temp.resize(output_size);
        output.swap(temp);
        return true;
    }

    /**
     * @brief Gets the expected length of an encoded buffer
     * Can be used to allocate the required storage before encoding
     *
     * len - The length of the buffer to encode
     * You many need to add one to it if you want space for '\0' as well.
     * @returns The expected length of an encoded buffer
     */
    static constexpr stl::size_t expected_length(stl::size_t const len) noexcept {
        return (len + 2ULL) / 3ULL * 4ULL;
    }


} // namespace webpp::base64

#endif // WEBPP_BASE64_HPP
