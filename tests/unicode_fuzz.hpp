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
        using webpp::unicode::normalization_form;
        using webpp::unicode::normalize;
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


        std::string resStringStyle;
        normalize<normalization_form::NFC>(str.begin(), str.end(), resStringStyle);
        ASSERT_EQ(resStringStyle, res) << to_hex(str);

        // test inplace
        normalize<normalization_form::NFC>(resStringStyle);
        ASSERT_EQ(resStringStyle, res) << to_hex(str);

        // test inplace with pointers
        {
            resStringStyle.resize(resStringStyle.size() * 19); // UB if we don't
            auto outptr = resStringStyle.begin();
            auto endptr =
              stl::next(resStringStyle.begin(), static_cast<std::string::difference_type>(res.size()));
            normalize<normalization_form::NFC>(outptr, endptr, outptr);
            ASSERT_EQ(outptr - resStringStyle.begin(), res.size()) << to_hex(str);
            resStringStyle.resize(static_cast<stl::size_t>(outptr - resStringStyle.begin()));
            ASSERT_EQ(resStringStyle, res) << to_hex(str) << "\n" << str;
        }

        // test append
        {
            std::string out = "one two three. ";
            normalize<normalization_form::NFC>(str.begin(), str.end(), out);
            EXPECT_EQ(out, "one two three. " + res) << to_hex(res);
        }


        std::string resPtrStyle;
        auto const  overwrite =
          [&]<typename T>(T*                                 cur_ptr,
                          [[maybe_unused]] stl::size_t const n_length /* = max_length */) constexpr noexcept {
              auto const beg = cur_ptr;
              normalize<normalization_form::NFC>(
                str.data(),
                str.data() + str.size(), // NOLINT(*-pro-bounds-pointer-arithmetic)
                cur_ptr);
              return static_cast<std::size_t>(cur_ptr - beg);
          };
        resPtrStyle.resize(res.size() * 19);
        resPtrStyle.resize(overwrite(resPtrStyle.data(), res.size()));
        ASSERT_EQ(resPtrStyle, res) << to_hex(str);
    }
} // namespace webpp::tests

// NOLINTEND(*-pro-type-reinterpret-cast)


#endif // UNICODE_FUZZ_HPP
