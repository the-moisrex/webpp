// Created by moisrex on 5/14/24.

#ifndef WEBPP_UNICODE_HANGUL_HPP
#define WEBPP_UNICODE_HANGUL_HPP

#include "../std/string_like.hpp"

namespace webpp::unicode {

    namespace details {
        // From https://www.unicode.org/versions/Unicode15.1.0/ch03.pdf#G56669

        /// SBase in the standard:
        static constexpr auto hangul_syllable_base = 0xAC00UL;

        /// LBase in the standard:
        static constexpr auto hangul_leading_base = 0x1100UL;

        /// VBase in the standard:
        static constexpr auto hangul_vowel_base = 0x1161UL;

        /// TBase in the standard:
        static constexpr auto hangul_trailing_base = 0x11A7UL;

        /// LCount in the standard:
        static constexpr auto hangul_leading_count = 19UL;

        /// VCount in the standard:
        static constexpr auto hangul_vowel_count = 21UL;

        /// TCount in the standard:
        static constexpr auto hangul_trailing_count = 28UL;

        /// Total count of Hangul blocks and syllables
        /// NCount in the standard:
        static constexpr auto hangul_block_count = hangul_vowel_count * hangul_trailing_count;

        /// SCount in the standard:
        static constexpr auto hangul_syllable_count = hangul_leading_count * hangul_block_count;
    } // namespace details

    /**
     * Checks if a given code point is a Hangul syllable.
     *
     * A Hangul syllable refers to a complete unit of sound in the Korean writing system, Hangul. It is the
     * smallest unit of writing that represents a single syllable in the Korean language.
     *
     * In Hangul, each syllable is composed of one or more characters, which can include a leading consonant,
     * a vowel, and a trailing consonant. These characters are combined to form a visually connected
     * block that represents the sound of the syllable.
     *
     * For example, the Hangul syllable "가" (pronounced as "ga" in English) consists of the leading consonant
     * "ㄱ" (pronounced as "g" in English) and the vowel "ㅏ" (pronounced as "a" in English). Similarly, the
     * syllable "바" (pronounced as "ba" in English) consists of the leading consonant "ㅂ" (pronounced as "b"
     * in English) and the vowel "ㅏ" (pronounced as "a" in English).
     *
     * It does this by comparing the code point with the base value of Hangul syllables (hangul_syllable_base)
     * and the total count of Hangul syllables (hangul_syllable_count).
     * If the code point falls within this range, it returns true; otherwise, it returns false.
     */
    template <typename CharT = char32_t>
    [[nodiscard]] static constexpr bool is_hangul_code_point(CharT const code_point) noexcept {
        return code_point >= details::hangul_syllable_base &&
               code_point < details::hangul_syllable_base + details::hangul_syllable_count;
    }

    /**
     * Checks if a given code point is a Hangul leading consonant.
     *
     * A Hangul leading consonant refers to the initial consonant sound in a Hangul syllable. In the Korean
     * writing system, Hangul, each syllable is composed of one or more characters. The leading consonant is
     * the first character in a syllable and represents the initial sound.
     *
     * For example, in the Hangul syllable "가" (pronounced as "ga" in English), the leading consonant is "ㄱ"
     * (pronounced as "g" in English). Similarly, in the syllable "바" (pronounced as "ba" in English), the
     * leading consonant is "ㅂ" (pronounced as "b" in English).
     *
     * It compares the code point with the base value of Hangul leading consonants (hangul_leading_base) and
     * the count of Hangul leading consonants (hangul_leading_count).
     * If the code point falls within this range, it returns true; otherwise, it returns false.
     */
    template <typename CharT = char32_t>
    [[nodiscard]] static constexpr bool is_hangul_leading(CharT const code_point) noexcept {
        return code_point >= details::hangul_leading_base &&
               code_point < details::hangul_leading_base + details::hangul_leading_count;
    }

    /**
     * Checks if a given code point is a Hangul trailing consonant.
     *
     * A Hangul trailing consonant refers to the final consonant sound in a Hangul syllable. In the Korean
     * writing system, Hangul, each syllable is composed of one or more characters. The trailing consonant is
     * the last character in a syllable and represents the final sound.
     *
     * For example, in the Hangul syllable "갈" (pronounced as "gal" in English), the trailing consonant is
     * "ㄹ" (pronounced as "l" in English). Similarly, in the syllable "밟" (pronounced as "balp" in English),
     * the trailing consonant is "ㅂ" (pronounced as "b" in English).
     *
     * It compares the code point with the base value of Hangul trailing consonants (hangul_trailing_base) and
     * the count of Hangul trailing consonants (hangul_trailing_count).
     * If the code point falls within this range, it returns true; otherwise, it returns false.
     */
    template <typename CharT = char32_t>
    [[nodiscard]] static constexpr bool is_hangul_trailing(CharT const code_point) noexcept {
        return code_point >= details::hangul_trailing_base &&
               code_point < details::hangul_trailing_base + details::hangul_trailing_count;
    }

    /**
     * Checks if a given code point is a Hangul vowel.
     *
     * A Hangul vowel refers to the sound that represents the nucleus of a Hangul syllable in the Korean
     * writing system, Hangul. Vowels are essential components of syllables and contribute to the
     * pronunciation of words.
     *
     * In Hangul, there are 21 basic vowel sounds, represented by specific characters. These vowel characters
     * can be combined with leading consonants and trailing consonants to form complete syllables.
     *
     * For example, in the Hangul syllable "가" (pronounced as "ga" in English), the vowel is "ㅏ" (pronounced
     * as "a" in English). Similarly, in the syllable "바" (pronounced as "ba" in English), the vowel is "ㅏ"
     * (pronounced as "a" in English).
     *
     * It compares the code point with the base value of Hangul vowels (hangul_vowel_base) and the count of
     * Hangul vowels (hangul_vowel_count).
     * If the code point falls within this range, it returns true; otherwise, it returns false.
     */
    template <typename CharT = char32_t>
    [[nodiscard]] static constexpr bool is_hangul_vowel(CharT const code_point) noexcept {
        return code_point >= details::hangul_vowel_base &&
               code_point < details::hangul_vowel_base + details::hangul_vowel_count;
    }

    template <typename CharT = char32_t>
    struct decomposed_hangul {
        static constexpr CharT invalid_trailing = details::hangul_trailing_base;

        CharT leading;
        CharT vowel;
        CharT trailing = invalid_trailing;
    };

    /**
     * Decompose the Hangul code point and return its parts.
     * This function does not check if the inputted code point is Hangul or not.
     * https://www.unicode.org/versions/Unicode15.1.0/ch03.pdf#G56669
     *
     * @returns decomposed_hangul which contains all the parts of a hangul decomposed code point
     */
    template <typename CharT = char32_t>
    [[nodiscard]] static constexpr decomposed_hangul<CharT> decomposed_hangul_code_point(
      CharT const code_point) noexcept {
        using details::hangul_block_count;
        using details::hangul_leading_base;
        using details::hangul_syllable_base;
        using details::hangul_trailing_base;
        using details::hangul_trailing_count;
        using details::hangul_vowel_base;

        auto const pos = code_point - hangul_syllable_base;

        // Calculating the indices:
        auto const leading_pos  = pos / hangul_block_count;
        auto const vowel_pos    = (pos % hangul_block_count) / hangul_trailing_count;
        auto const trailing_pos = pos % hangul_trailing_count;

        // Calculating the values:
        return decomposed_hangul<CharT>{
          .leading  = hangul_leading_base + leading_pos,
          .vowel    = hangul_vowel_base + vowel_pos,
          .trailing = trailing_pos != 0 ? hangul_trailing_base + trailing_pos
                                        : decomposed_hangul<CharT>::invalid_trailing,
        };
    }

    /**
     * Append the decomposed Hangul code point to the output
     * This function does not check if the inputted code point is Hangul or not.
     *
     * @tparam CharT     char type
     * @tparam StrOrIter Can be a string/string-view/iterator/vector<CharT>/...
     */
    template <typename CharT = char32_t, typename StrOrIter>
    static void decompose_hangul_code_point(CharT const code_point, StrOrIter& out) {
        auto const [leading, vowel, trailing] = decompose_hangul_code_point<CharT>(code_point);
        istl::append(out, leading);
        istl::append(out, vowel);
        if (trailing != decomposed_hangul<CharT>::invalid_trailing) {
            istl::append(out, trailing);
        }
    }

    /**
     * L, V and LV, T Hangul Syllable Composition
     * L = Leading
     * V = Vowel
     * T = Trailing
     *
     * The following algorithm specifies how to take a canonically decomposed sequence of Hangul jamo
     * characters and arithmetically derive its mapping to an equivalent precomposed Hangul syllable. This
     * normative mapping can be used to calculate the Primary Composite for a sequence of Hangul jamo
     * characters, as specified in D117, Canonical Composition Algorithm, in Section 3.11, Normalization
     * Forms.
     *
     * Strictly speaking, this algorithm is simply the inverse of the full canonical decomposition
     * mappings specified by the Hangul Syllable Decomposition Algorithm. However, it is useful to have a
     * summary specification of that inverse mapping as a separate algorithm, for convenience in
     * implementation.
     *
     * From: https://www.unicode.org/versions/Unicode15.1.0/ch03.pdf#G59688
     */
    template <typename CharT = char32_t>
    [[nodiscard]] static constexpr CharT combine_hangul_code_points(
      CharT const lhs,
      CharT const rhs) noexcept {
        using details::hangul_block_count;
        using details::hangul_leading_base;
        using details::hangul_syllable_base;
        using details::hangul_trailing_base;
        using details::hangul_trailing_count;
        using details::hangul_vowel_base;

        if (is_hangul_leading(lhs) && is_hangul_vowel(rhs)) {
            auto const leading_pos = lhs - hangul_leading_base;
            auto const vowel_pos   = rhs - hangul_vowel_base;
            auto const leading_vowel_pos =
              (leading_pos * hangul_block_count) + (vowel_pos * hangul_trailing_count);
            return hangul_syllable_base + leading_vowel_pos;
        }

        // LV characters are the first in each "T block", so use this check to avoid combining LVT with T.
        if (is_hangul_code_point(lhs) && (lhs - hangul_syllable_base) % hangul_trailing_count == 0 &&
            is_hangul_trailing(rhs))
        {
            return lhs + rhs - hangul_trailing_base;
        }
        return 0;
    }

} // namespace webpp::unicode

#endif // WEBPP_UNICODE_HANGUL_HPP
