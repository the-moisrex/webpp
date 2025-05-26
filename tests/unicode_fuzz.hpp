// Created by moisrex on 12/23/24.

#ifndef UNICODE_FUZZ_HPP
#define UNICODE_FUZZ_HPP

#include "../webpp/strings/hex.hpp"
#include "../webpp/unicode/normalization.hpp"
#include "./common/tests_common_pch.hpp"

namespace webpp::tests {

    std::string to_hex(auto const& hexString) {
        using str_t     = stl::remove_cvref_t<decltype(hexString)>;
        using char_type = stl::iter_value_t<str_t>;
        std::ostringstream oss;
        for (auto const codePoint : hexString) {
            if constexpr (unicode::UTF8<char_type>) {
                oss << "\\x" << ascii::to_percent_hex<stl::uint8_t>(codePoint) + 1;
            } else if constexpr (unicode::UTF16<char_type>) {
                oss << "\\x" << ascii::to_percent_hex<stl::uint16_t>(codePoint) + 1;
            } else if constexpr (unicode::UTF32<char_type>) {
                oss << "\\x" << ascii::to_percent_hex<stl::uint32_t>(codePoint) + 1;
            } else {
                oss << "\\x????";
            }
        }
        return oss.str();
    }

    // NOLINTBEGIN(*-pro-type-reinterpret-cast)
    static void unicode_fuzz(std::string_view data) {
        using webpp::unicode::decompose_iterator;
        using webpp::unicode::isNFC;
        using webpp::unicode::normalization_form;
        using webpp::unicode::normalize;
        using webpp::unicode::toNFC;
        using webpp::unicode::toNFD;
        using enum normalization_form;

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
        ASSERT_TRUE(isNFC(res.begin(), res.end())) << "Src: " << to_hex(data) << "\nNFC: " << to_hex(res);
        ASSERT_TRUE(isNFC(res16.begin(), res16.end())) << "Src: " << to_hex(data) << "\nNFC: " << to_hex(res);
        ASSERT_TRUE(isNFC(res32.begin(), res32.end())) << "Src: " << to_hex(data) << "\nNFC: " << to_hex(res);


        auto const dres   = toNFD<std::string>(str);
        auto const dres8  = toNFD<std::u8string>(str8);
        auto const dres16 = toNFD<std::u16string>(str16);
        auto const dres32 = toNFD<std::u32string>(str32);
        if (!str8.empty()) {
            if (length / 2 != 0) {
                ASSERT_NE(dres16.size(), 0) << to_hex(str);
            }
            if (length / 4 != 0) {
                ASSERT_NE(dres32.size(), 0) << to_hex(str);
            }
            ASSERT_NE(dres.size(), 0) << to_hex(str);
            ASSERT_NE(dres8.size(), 0) << to_hex(str);
        }

        decompose_iterator       dbeg{str.begin(), str.end()};
        decompose_iterator const dend{str.end(), str.end()};
        stl::string const        idres{dbeg, dend};

        ASSERT_EQ(idres, dres) << "Source: " << to_hex(data);
        ASSERT_TRUE(stl::equal(dbeg, dend, dres.begin()))
          << "Src: " << to_hex(data) << "\nNFD: " << to_hex(dres) << "\nBad NFD: " << to_hex(idres);



        std::string resStringStyle;
        normalize<NFC>(str.begin(), str.end(), resStringStyle);
        ASSERT_EQ(resStringStyle, res) << to_hex(str);

        // test inplace
        normalize<NFC>(resStringStyle);
        ASSERT_EQ(resStringStyle, res) << to_hex(str);

        // test inplace with pointers
        {
            resStringStyle.resize(resStringStyle.size() * 19); // UB if we don't
            auto outptr = resStringStyle.begin();
            auto endptr =
              stl::next(resStringStyle.begin(), static_cast<std::string::difference_type>(res.size()));
            normalize<NFC>(outptr, endptr, outptr);
            ASSERT_EQ(outptr - resStringStyle.begin(), res.size()) << to_hex(str);
            resStringStyle.resize(static_cast<stl::size_t>(outptr - resStringStyle.begin()));
            ASSERT_EQ(resStringStyle, res) << to_hex(str) << "\n" << str;
        }

        // test append
        {
            std::string out = "one two three. ";
            normalize<NFC>(str.begin(), str.end(), out);
            EXPECT_EQ(out, "one two three. " + res) << to_hex(res);
        }


        std::string resPtrStyle;
        auto const  overwrite =
          [&]<typename T>(T*                                 cur_ptr,
                          [[maybe_unused]] stl::size_t const n_length /* = max_length */) constexpr noexcept {
              auto const beg = cur_ptr;
              normalize<NFC>(str.data(),
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
