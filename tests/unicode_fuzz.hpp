// Created by moisrex on 12/23/24.

#ifndef UNICODE_FUZZ_HPP
#define UNICODE_FUZZ_HPP

#include "../webpp/std/format.hpp"
#include "../webpp/strings/hex.hpp"
#include "../webpp/unicode/normalization.hpp"
#include "./common/tests_common_pch.hpp"

namespace webpp::tests {

    // NOLINTBEGIN(*)
    // Helper function to convert a single byte to hex
    inline std::string byteToHex(unsigned char byte) {
        return fmt::format("{:02X}", static_cast<unsigned int>(byte));
    }

    // Concept to check if a type is a character type suitable for basic_string or basic_string_view
    template <typename T>
    concept CharacterType = std::is_same_v<T, char> || std::is_same_v<T, char8_t> ||
                            std::is_same_v<T, char16_t> || std::is_same_v<T, char32_t>;

    // --- Overloads for std::basic_string ---

    // Overload for UTF-8 (std::string)
    template <CharacterType CharT>
    inline std::string to_hex(std::basic_string<CharT> const& str)
        requires(std::is_same_v<CharT, char> || std::is_same_v<CharT, char8_t>)
    {
        std::string hex_str;
        hex_str.reserve(str.length() * 2); // Pre-allocate memory for efficiency
        for (unsigned char const byte : str) {
            hex_str += "\\x";
            hex_str += byteToHex(byte);
        }
        return hex_str;
    }

    // Overload for UTF-16 (std::u16string)
    template <CharacterType CharT>
    inline std::string to_hex(std::basic_string<CharT> const& str)
        requires std::is_same_v<CharT, char16_t>
    {
        std::string hex_str;
        hex_str.reserve(str.length() * 4); // Each char16_t is 2 bytes, 4 hex chars
        for (char16_t const unit : str) {
            hex_str += "\\x";
            hex_str += byteToHex(static_cast<unsigned char>((unit >> 8) & 0xFF));
            hex_str += byteToHex(static_cast<unsigned char>(unit & 0xFF));
        }
        return hex_str;
    }

    // Overload for UTF-32 (std::u32string)
    template <CharacterType CharT>
    inline std::string to_hex(std::basic_string<CharT> const& str)
        requires std::is_same_v<CharT, char32_t>
    {
        std::string hex_str;
        hex_str.reserve(str.length() * 8); // Each char32_t is 4 bytes, 8 hex chars
        for (char32_t const unit : str) {
            hex_str += "\\x";
            hex_str += byteToHex(static_cast<unsigned char>((unit >> 24) & 0xFF));
            hex_str += byteToHex(static_cast<unsigned char>((unit >> 16) & 0xFF));
            hex_str += byteToHex(static_cast<unsigned char>((unit >> 8) & 0xFF));
            hex_str += byteToHex(static_cast<unsigned char>(unit & 0xFF));
        }
        return hex_str;
    }

    // --- Overloads for std::basic_string_view ---

    // Overload for UTF-8 (std::string_view)
    template <CharacterType CharT>
    inline std::string to_hex(std::basic_string_view<CharT> const& str_view)
        requires(std::is_same_v<CharT, char> || std::is_same_v<CharT, char8_t>)
    {
        std::string hex_str;
        hex_str.reserve(str_view.length() * 2);
        for (unsigned char byte : str_view) {
            hex_str += "\\x";
            hex_str += byteToHex(byte);
        }
        return hex_str;
    }

    // Overload for UTF-16 (std::u16string_view)
    template <CharacterType CharT>
    inline std::string to_hex(std::basic_string_view<CharT> const& str_view)
        requires std::is_same_v<CharT, char16_t>
    {
        std::string hex_str;
        hex_str.reserve(str_view.length() * 4);
        for (char16_t const unit : str_view) {
            hex_str += "\\x";
            hex_str += byteToHex(static_cast<unsigned char>((unit >> 8) & 0xFF));
            hex_str += byteToHex(static_cast<unsigned char>(unit & 0xFF));
        }
        return hex_str;
    }

    // Overload for UTF-32 (std::u32string_view)
    template <CharacterType CharT>
    inline std::string to_hex(std::basic_string_view<CharT> const& str_view)
        requires std::is_same_v<CharT, char32_t>
    {
        std::string hex_str;
        hex_str.reserve(str_view.length() * 8);
        for (char32_t const unit : str_view) {
            hex_str += "\\x";
            hex_str += byteToHex(static_cast<unsigned char>((unit >> 24) & 0xFF));
            hex_str += byteToHex(static_cast<unsigned char>((unit >> 16) & 0xFF));
            hex_str += byteToHex(static_cast<unsigned char>((unit >> 8) & 0xFF));
            hex_str += byteToHex(static_cast<unsigned char>(unit & 0xFF));
        }
        return hex_str;
    }

    // NOLINTEND(*)

    // NOLINTBEGIN(*-pro-type-reinterpret-cast)
    static void unicode_fuzz(std::string_view data) {
        using webpp::unicode::canonical_decomposed;
        using webpp::unicode::decompose_iterator;
        using webpp::unicode::isNFC;
        using webpp::unicode::normalization_form;
        using webpp::unicode::normalize;
        using webpp::unicode::toNFC;
        using webpp::unicode::toNFD;
        using webpp::unicode::checked::utf32_forward_iter;
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

        auto const dres   = canonical_decomposed<std::string>(str);
        auto const dres8  = canonical_decomposed<std::u8string>(str8);
        auto const dres16 = canonical_decomposed<std::u16string>(str16);
        auto const dres32 = canonical_decomposed<std::u32string>(str32);
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

        // we don't use the iterators directly since std::string will try to use distance on it
        stl::string idres;
        webpp::istl::resize_and_overwrite(
          idres,
          data.size() * 4,
          [&](auto* buf, [[maybe_unused]] stl::size_t max_len) {
              return stl::copy(dbeg, dend, buf) - buf;
          });

        ASSERT_EQ(idres, dres) << "Source: " << to_hex(data);
        ASSERT_TRUE(stl::equal(dbeg, dend, dres.begin()))
          << "Src: " << to_hex(data) << "\nNFD: " << to_hex(dres) << "\nBad NFD: " << to_hex(idres);


        ASSERT_TRUE(isNFC(res.begin(), res.end())) << "Src: " << to_hex(data) << "\nNFC: " << to_hex(res);
        ASSERT_TRUE(isNFC(res16.begin(), res16.end()))
          << "Src: " << to_hex(data) << "\nSrc16: " << to_hex(str16) << "\nNFC: " << to_hex(res16);
        ASSERT_TRUE(isNFC(res32.begin(), res32.end()))
          << "Src: " << to_hex(data) << "\nSrc32: " << to_hex(str32) << "\nNFC: " << to_hex(res32);


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
        resPtrStyle.resize(overwrite(resPtrStyle.data(), res.size() * 19));
        ASSERT_EQ(resPtrStyle, res) << to_hex(str);
    }
} // namespace webpp::tests

// NOLINTEND(*-pro-type-reinterpret-cast)


#endif // UNICODE_FUZZ_HPP
