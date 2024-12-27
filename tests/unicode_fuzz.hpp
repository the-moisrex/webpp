// Created by moisrex on 12/23/24.

#ifndef UNICODE_FUZZ_HPP
#define UNICODE_FUZZ_HPP

#include "../webpp/unicode/normalization.hpp"
#include "./common/tests_common_pch.hpp"

namespace webpp::tests {

    template <typename CharT>
    std::string to_hex(std::basic_string<CharT> const& hexString) {
        std::ostringstream oss;
        for (auto const codePoint : hexString) {
            oss << "\\x" << std::hex << static_cast<std::uint32_t>(codePoint);
        }
        return oss.str();
    }


    // NOLINTBEGIN(*-pro-type-reinterpret-cast)
    static void unicode_fuzz(std::string_view data) {
        using webpp::unicode::toNFC;

        auto const        length = data.size();
        auto const* const ptr    = data.data();

        std::string const   str{ptr, length};
        std::u8string const str8{reinterpret_cast<char8_t const*>(ptr), length};
        std::u16string      str16;
        std::u32string      str32;
        if (length / 2 != 0) {
            str16 = std::u16string{reinterpret_cast<char16_t const*>(ptr), length / 2};
        }
        if (length / 4 != 0) {
            str32 = std::u32string{reinterpret_cast<char32_t const*>(ptr), length / 4};
        }
        auto const res   = toNFC<std::string>(str);
        auto const res8  = toNFC<std::u8string>(str8);
        auto const res16 = toNFC<std::u16string>(str16);
        auto const res32 = toNFC<std::u32string>(str32);
        if (!str8.empty()) {
            ASSERT_NE(str8.size(), 0) << to_hex(str);
            if (length / 2 != 0) {
                ASSERT_NE(str16.size(), 0) << to_hex(str);
                ASSERT_NE(res16.size(), 0) << to_hex(str);
            }
            if (length / 4 != 0) {
                ASSERT_NE(str32.size(), 0) << to_hex(str);
                ASSERT_NE(res32.size(), 0) << to_hex(str);
            }
            ASSERT_NE(str.size(), 0) << to_hex(str);
            ASSERT_NE(res.size(), 0) << to_hex(str);
            ASSERT_NE(res8.size(), 0) << to_hex(str);
        }
    }
} // namespace webpp::tests

// NOLINTEND(*-pro-type-reinterpret-cast)


#endif // UNICODE_FUZZ_HPP
