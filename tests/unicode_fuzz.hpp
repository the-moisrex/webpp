// Created by moisrex on 12/23/24.

#ifndef UNICODE_FUZZ_HPP
#define UNICODE_FUZZ_HPP

#include "../webpp/std/format.hpp"
#include "../webpp/strings/hex.hpp"
#include "../webpp/unicode/normalization.hpp"
#include "./common/tests_common_pch.hpp"

namespace webpp::tests {

    // NOLINTBEGIN(*)
    namespace old_impl {
        using webpp::istl::Appendable;
        using webpp::istl::appendable_value_type_t;
        using webpp::istl::iter_append;
        using webpp::istl::NothrowAppendable;
        using webpp::istl::size_type_of_t;
        using webpp::stl::integral;
        using webpp::stl::make_unsigned_t;
        using webpp::unicode::max_bmp;
        using webpp::unicode::UTF16;
        using webpp::unicode::UTF32;
        using webpp::unicode::UTF8;

        /**
         * Append a code point to a string
         * "out" can be an iterator/pointer or a string
         */
        template <Appendable StrT, integral SizeT = size_type_of_t<StrT>, typename CharT = char32_t>
        SizeT append(StrT& out, CharT code_point) noexcept(NothrowAppendable<StrT>) {
            using char_type = appendable_value_type_t<StrT>;
            using uchar_t   = make_unsigned_t<CharT>;
            auto const ccp  = static_cast<uint32_t>(code_point);
            if constexpr (UTF8<char_type>) {
                if (ccp < 0x80U) { // one octet
                    iter_append(out, ccp);
                    return 1U;
                }
                if (ccp < 0x800) {                                   // two octets
                    iter_append(out, (ccp >> 6U) | 0xC0U);           // 0b110,'....
                    iter_append(out, (ccp & 0x3FU) | 0x80U);         // 0b10..'....
                    return 2U;
                }
                if (ccp < 0x1'0000U) {                               // three octets
                    iter_append(out, (ccp >> 12U) | 0xE0U);          // 0b1110'....
                    iter_append(out, ((ccp >> 6U) & 0x3FU) | 0x80U); // 0b10..'....
                    iter_append(out, (ccp & 0x3FU) | 0x80U);         // 0b10..'....
                    return 3U;
                }
                // four octets
                iter_append(out, (ccp >> 18U) | 0xF0U);           // 0b1111'0...
                iter_append(out, ((ccp >> 12U) & 0x3FU) | 0x80U); // 0b10..'....
                iter_append(out, ((ccp >> 6U) & 0x3FU) | 0x80U);  // 0b10..'....
                iter_append(out, (ccp & 0x3FU) | 0x80U);          // 0b10..'....
                return 4U;
            } else if constexpr (UTF16<char_type>) {
                if (ccp <= max_bmp<char_type>) {
                    iter_append(out, ccp); // normal case
                    return 1U;
                }
                iter_append(out, 0xD7C0U + (static_cast<uchar_t>(ccp) >> 10U));
                iter_append(out, 0xDC00U + (static_cast<uchar_t>(ccp) & 0x3FFU));
                return 2U;
            } else { // for char32_t or others
                iter_append(out, ccp);
                return 1U;
            }
        }
    } // namespace old_impl

    std::u8string utf32_to_utf8(std::u32string const& utf32_str) {
        std::u8string utf8_str;
        utf8_str.reserve(utf32_str.length() * 4); // Estimate maximum size of UTF-8 string

        std::u8string test_str;
        for (char32_t const code_point : utf32_str) {
            old_impl::append(test_str, code_point);
            if (!webpp::unicode::checked::append(utf8_str, code_point)) {
                throw webpp::stl::invalid_argument("Invalid code point");
            }

            EXPECT_EQ(utf8_str, test_str);
        }

        return utf8_str;
    }

    std::u16string utf32_to_utf16(std::u32string const& utf32_str) {
        std::u16string utf16_str;
        utf16_str.reserve(utf32_str.length() * 4); // Estimate maximum size of UTF-8 string

        std::u16string test_str;
        for (char32_t const code_point : utf32_str) {
            old_impl::append(test_str, code_point);
            if (!webpp::unicode::checked::append(utf16_str, code_point)) {
                throw webpp::stl::invalid_argument("Invalid code point");
            }

            EXPECT_EQ(utf16_str, test_str);
        }

        return utf16_str;
    }

    template <typename CharT>
        requires istl::part_of<CharT, char, char8_t>
    constexpr char32_t utf8_to_utf32(std::basic_string_view<CharT> const input) {
        char32_t codepoint = 0;

        if (!input.empty() && (0b1000'0000U & input[0]) == 0b0000'0000U) {
            codepoint = static_cast<unsigned char>(input[0]);
        } else if (input.size() > 1 && (0b1110'0000U & input[0]) == 0b1100'0000U) {
            codepoint = static_cast<char32_t>(((0b0001'1111U & input[0]) << 6U) | (input[1] & 0b0011'1111U));
        } else if (input.size() > 2 && (0b1111'0000U & input[0]) == 0b1110'0000U) {
            codepoint = static_cast<char32_t>(
              ((0b0000'1111U & input[0]) << 12U) | ((input[1] & 0b0011'1111U) << 6U) | (input[2] & 0b0011'1111U));
        } else if (input.size() > 3 && (0b1111'1000U & input[0]) == 0b1111'0000U) {
            codepoint = static_cast<char32_t>(((input[0] & 0b0000'0111U) << 18U) | ((0b0011'1111U & input[1]) << 12U) |
                                              ((input[2] & 0b0011'1111U) << 6U) | (0b0011'1111U & input[3]));
        }

        return codepoint;
    }

    template <typename CharT>
        requires istl::part_of<CharT, char, char8_t>
    std::u32string utf8_to_utf32(std::basic_string<CharT> const& utf8_str) {
        std::u32string utf32_str;
        utf32_str.reserve(utf32_str.length() * 4); // Estimate maximum size of UTF-8 string

        for (auto pos = utf8_str.begin(); pos != utf8_str.end();) {
            auto const impl_copy = webpp::unicode::checked::next_code_point(pos, utf8_str.end());
            // auto const impl2     = utf8_to_utf32(webpp::stl::basic_string_view<CharT>{pos, utf8_str.end()});
            utf32_str.push_back(impl_copy);

            // EXPECT_EQ(utf32_str.back(), impl2);
            // EXPECT_EQ(impl_copy, impl2);
        }

        return utf32_str;
    }

    // Helper function to convert a single byte to hex
    inline std::string byteToHex(auto byte) {
        return fmt::format("{:02X}", static_cast<stl::uint8_t>(byte));
    }

    // Concept to check if a type is a character type suitable for basic_string or basic_string_view
    template <typename T>
    concept CharacterType = std::is_same_v<T, char> || std::is_same_v<T, char8_t> || std::is_same_v<T, char16_t> ||
                            std::is_same_v<T, char32_t>;

    // --- Overloads for std::basic_string ---

    // Overload for UTF-8 (std::string)
    template <CharacterType CharT>
    inline std::string to_hex(std::basic_string<CharT> const& str)
        requires(std::is_same_v<CharT, char> || std::is_same_v<CharT, char8_t>)
    {
        std::string hex_str;
        hex_str.reserve(str.length() * 2); // Pre-allocate memory for efficiency
        for (auto const byte : str) {
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
        for (auto byte : str_view) {
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



    void canonical_reorder_simple(std::u32string& input) {
        for (stl::size_t idx = 1; idx < input.size(); idx++) {
            auto const ccc = unicode::ccc_of(input[idx]);
            if (ccc == 0) {
                continue;
            }
            auto const cur     = input[idx];
            auto       prev_id = idx;
            while (prev_id != 0 && unicode::ccc_of(input[prev_id - 1]) > ccc) {
                input[prev_id] = input[prev_id - 1];
                prev_id--;
            }
            input[prev_id] = cur;
        }
    }

    void check_idempotent(auto const& str, auto const& nfc, auto const& nfd) {
        using tests::to_hex;
        using unicode::toNFC;
        using unicode::toNFD;

        // toNFC
        EXPECT_EQ(toNFC(str), toNFC(toNFC(str)))
          << "  Src: " << to_hex(str) << "\n  NFC Layer 1: " << to_hex(toNFC(str)) << "\n  NFC Answer: " << to_hex(nfc)
          << "\n  NFD Answer: " << to_hex(nfd);
        EXPECT_EQ(toNFC(str), toNFC(toNFD(str)))
          << "  NFD: " << to_hex(toNFD(str)) << "\n  Source: " << to_hex(str) << "\n  NFC Answer: " << to_hex(nfc)
          << "\n  NFD Answer: " << to_hex(nfd);

        // toNFD
        EXPECT_EQ(toNFD(str), toNFD(toNFC(str)))
          << "  NFC: " << to_hex(toNFC(str)) << "\n  Source: " << to_hex(str) << "\n  NFC Answer: " << to_hex(nfc)
          << "\n  NFD Answer: " << to_hex(nfd);
        EXPECT_EQ(toNFD(str), toNFD(toNFD(str)))
          << "  Src: " << to_hex(str) << "\n  NFD Layer 1: " << to_hex(toNFD(str)) << "\n  NFC Answer: " << to_hex(nfc)
          << "\n  NFD Answer: " << to_hex(nfd);

        // toNFKC
        // EXPECT_EQ(toNFKC(str), toNFC(toNFKC(str)));
        // EXPECT_EQ(toNFKC(str), toNFC(toNFKD(str)));
        // EXPECT_EQ(toNFKC(str), toNFKC(toNFC(str)));
        // EXPECT_EQ(toNFKC(str), toNFKC(toNFD(str)));
        // EXPECT_EQ(toNFKC(str), toNFKC(toNFKC(str)));
        // EXPECT_EQ(toNFKC(str), toNFKC(toNFKD(str)));

        // toNFKD
        // EXPECT_EQ(toNFKD(str), toNFD(toNFKC(str)));
        // EXPECT_EQ(toNFKD(str), toNFD(toNFKD(str)));
        // EXPECT_EQ(toNFKD(str), toNFKD(toNFC(str)));
        // EXPECT_EQ(toNFKD(str), toNFKD(toNFD(str)));
        // EXPECT_EQ(toNFKD(str), toNFKD(toNFKC(str)));
        // EXPECT_EQ(toNFKD(str), toNFKD(toNFKD(str)));
    }

    void check_idempotent(auto const& str) {
        using tests::to_hex;
        using unicode::toNFC;
        using unicode::toNFD;

        // toNFC
        EXPECT_EQ(toNFC(str), toNFC(toNFC(str)))
          << "  Src: " << to_hex(str) << "\n  NFC Layer 1: " << to_hex(toNFC(str));
        EXPECT_EQ(toNFC(str), toNFC(toNFD(str))) << "  NFD: " << to_hex(toNFD(str)) << "\n  Source: " << to_hex(str);

        // toNFD
        EXPECT_EQ(toNFD(str), toNFD(toNFC(str))) << "  NFC: " << to_hex(toNFC(str)) << "\n  Source: " << to_hex(str);
        EXPECT_EQ(toNFD(str), toNFD(toNFD(str)))
          << "  Src: " << to_hex(str) << "\n  NFD Layer 1: " << to_hex(toNFD(str));

        auto ordered_str = str;
        unicode::canonically_reorder(ordered_str);
        EXPECT_TRUE(unicode::is_canonically_ordered(ordered_str.begin(), ordered_str.end()));
        if (ordered_str != str) {
            EXPECT_FALSE(unicode::is_canonically_ordered(str.begin(), str.end()))
              << "  Src: " << to_hex(str) << "\n  Ordered: " << to_hex(ordered_str);
        }
        auto ordered_str2 = ordered_str;
        unicode::canonically_reorder(ordered_str2);
        EXPECT_EQ(ordered_str, ordered_str2);

        // Test UTF-X to std::u32string conversion, canonical reorder, and back conversion
        // Convert input string to UTF-32
        std::u32string utf32_str;

        // Handle different input types
        if constexpr (std::is_same_v<std::decay_t<decltype(str)>, std::string>) {
            utf32_str = utf8_to_utf32(str);
        } else if constexpr (std::is_same_v<std::decay_t<decltype(str)>, std::u8string>) {
            utf32_str = utf8_to_utf32(str);
        } else if constexpr (std::is_same_v<std::decay_t<decltype(str)>, std::u16string>) {
            utf32_str = utf16_to_utf32(str);
        } else if constexpr (std::is_same_v<std::decay_t<decltype(str)>, std::u32string>) {
            utf32_str = str;
        } else {
            // For string views, convert through appropriate function
            if constexpr (std::is_same_v<std::decay_t<decltype(str)>, std::string_view>) {
                utf32_str = utf8_to_utf32(str);
            } else if constexpr (std::is_same_v<std::decay_t<decltype(str)>, std::u8string_view>) {
                utf32_str = utf8_to_utf32(str);
            } else if constexpr (std::is_same_v<std::decay_t<decltype(str)>, std::u16string_view>) {
                utf32_str = utf16_to_utf32(str);
            } else if constexpr (std::is_same_v<std::decay_t<decltype(str)>, std::u32string_view>) {
                utf32_str = std::u32string{str};
            }
        }

        // Make a copy for comparison
        auto utf32_copy = utf32_str;

        // Apply canonical reorder to the copy
        canonical_reorder_simple(utf32_copy);

        // Check if the result matches what the official functions produce
        auto official_reordered = utf32_str;
        unicode::canonically_reorder(official_reordered);

        // Convert back to original type for comparison
        EXPECT_EQ(utf32_copy, official_reordered)
          << "  UTF-32: " << to_hex(utf32_str) << "\n  Simple reordered: " << to_hex(utf32_copy)
          << "\n  Official reordered: " << to_hex(official_reordered);

        // Check ordering status
        EXPECT_TRUE(unicode::is_canonically_ordered(utf32_copy.begin(), utf32_copy.end()))
          << "  UTF-32: " << to_hex(utf32_str);
        EXPECT_TRUE(unicode::is_canonically_ordered(official_reordered.begin(), official_reordered.end()))
          << "  UTF-32: " << to_hex(official_reordered);

        // toNFKC
        // EXPECT_EQ(toNFKC(str), toNFC(toNFKC(str)));
        // EXPECT_EQ(toNFKC(str), toNFC(toNFKD(str)));
        // EXPECT_EQ(toNFKC(str), toNFKC(toNFC(str)));
        // EXPECT_EQ(toNFKC(str), toNFKC(toNFD(str)));
        // EXPECT_EQ(toNFKC(str), toNFKC(toNFKC(str)));
        // EXPECT_EQ(toNFKC(str), toNFKC(toNFKD(str)));

        // toNFKD
        // EXPECT_EQ(toNFKD(str), toNFD(toNFKC(str)));
        // EXPECT_EQ(toNFKD(str), toNFD(toNFKD(str)));
        // EXPECT_EQ(toNFKD(str), toNFKD(toNFC(str)));
        // EXPECT_EQ(toNFKD(str), toNFKD(toNFD(str)));
        // EXPECT_EQ(toNFKD(str), toNFKD(toNFKC(str)));
        // EXPECT_EQ(toNFKD(str), toNFKD(toNFKD(str)));
    }

    // NOLINTBEGIN(*-pro-type-reinterpret-cast)
    static void unicode_fuzz(std::string_view data) {
        using unicode::canonical_decomposed;
        using unicode::decompose_iterator;
        using unicode::isNFC;
        using unicode::isNFD;
        using unicode::norm_form;
        using unicode::normalize;
        using unicode::toNFC;
        using unicode::toNFD;
        using unicode::checked::utf32_forward_iter;
        using enum norm_form;
        using diff_type = std::iter_difference_t<std::string::iterator>;

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
        check_idempotent(str);
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

        decompose_iterator       dbeg16{str16.begin(), str16.end()};
        decompose_iterator const dend16{str16.end(), str16.end()};

        // we don't use the iterators directly since std::string will try to use distance on it
        stl::string idres;
        stl::u16string idres16;
        istl::resize_and_overwrite(idres, data.size() * 4, [=](auto* buf, [[maybe_unused]] stl::size_t max_len) {
            return stl::copy(dbeg, dend, buf) - buf;
        });
        istl::resize_and_overwrite(idres16, data.size() * 4, [=](auto* buf, [[maybe_unused]] stl::size_t max_len) {
            return stl::copy(dbeg16, dend16, buf) - buf;
        });

        ASSERT_EQ(idres, dres) << "Source: " << to_hex(data);
        ASSERT_TRUE(stl::equal(dbeg, dend, dres.begin()))
          << "Src: " << to_hex(data) << "\nNFD: " << to_hex(dres) << "\nBad NFD: " << to_hex(idres);
        stl::advance(dbeg, dres.size());
        ASSERT_TRUE(stl::equal(std::reverse_iterator{dbeg},
                               std::reverse_iterator{dend},
                               std::next(dres.begin(), static_cast<diff_type>(dres.size()))))
          << "Src: " << to_hex(data) << "\nNFD: " << to_hex(dres) << "\nBad NFD: " << to_hex(idres);


        ASSERT_TRUE(isNFC(res.begin(), res.end()))
          << "Src: " << to_hex(data) << "\nNFC: " << to_hex(res) << "\ndecomposed: " << to_hex(idres)
          << "\nNFD: " << to_hex(toNFD<std::string>(str));
        ASSERT_TRUE(isNFC(res16.begin(), res16.end()))
          << "Src: " << to_hex(data) << "\nSrc16: " << to_hex(str16) << "\nNFC: " << to_hex(res16)
          << "\nDecomposed     : " << to_hex(dres16) << "\nDecomposed Iter: " << to_hex(idres16);
        ASSERT_TRUE(isNFC(res32.begin(), res32.end()))
          << "Src: " << to_hex(data) << "\nSrc32: " << to_hex(str32) << "\nNFC: " << to_hex(res32);

        ASSERT_TRUE(isNFD(toNFD(std::basic_string{str})));
        if (isNFD(str.begin(), str.end())) {
            ASSERT_EQ(toNFD(std::basic_string{str}), str);
        }

        if (isNFC(str.begin(), str.end())) {
            ASSERT_EQ(toNFC(std::basic_string{str}), str);
        }

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
            auto endptr = stl::next(resStringStyle.begin(), static_cast<std::string::difference_type>(res.size()));
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
