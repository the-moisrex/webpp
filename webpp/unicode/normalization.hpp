// Created by moisrex on 4/15/24.
/**
 * Normalization in Unicode:
 *
 * In Unicode it is possible to produce the same text with different sequences of characters. For example,
 * take the Hungarian word világ. The fourth letter could be stored in memory as a precomposed U+00E1 LATIN
 * SMALL LETTER A WITH ACUTE (a single character) or as a decomposed sequence of U+0061 LATIN SMALL LETTER A
 * followed by U+0301 COMBINING ACUTE ACCENT (two characters).
 *
 *   világ = világ
 *
 * The Unicode Standard allows either of these alternatives, but requires that both be treated as
 * identical. To improve efficiency, an application will usually normalize text before performing searches or
 * comparisons. Normalization, in this case, means converting the text to use all precomposed or all
 * decomposed characters.
 *
 * There are four normalization forms specified by the Unicode Standard:
 *   - NFC,          : Normalization Form               Composition
 *   - NFD,          : Normalization Form               Decomposition
 *   - NFKC, and     : Normalization Form Kompatibility Composition
 *   - NFKD.         : Normalization Form Kompatibility Decomposition
 * The C stands for (pre-)composed, and the D for decomposed. The K stands for compatibility. To improve
 * interoperability, the W3C recommends the use of NFC normalized text on the Web.
 *
 * NFD uses Unicode rules to maximally decompose a code point into component parts. For example, the
 * Vietnamese letter ề [U+1EC1 LATIN SMALL LETTER E WITH CIRCUMFLEX AND GRAVE] becomes the sequence ề [U+0065
 * LATIN SMALL LETTER E + U+0302 COMBINING CIRCUMFLEX ACCENT + U+0300 COMBINING GRAVE ACCENT].
 *
 * NFC runs that process in reverse, and will also completely compose partially decomposed sequences. However,
 * this composition process is only applied to a subset of the Unicode repertoire. For example, the sequence g̀
 * [U+0067 LATIN SMALL LETTER G + U+0300 COMBINING GRAVE ACCENT] has no precomposed form, and is unaffected by
 * normalization.
 *
 * NFKC and NFKD were introduced to handle characters that were included in Unicode in order to provide
 * compatibility with other character sets. This applies to code points that represent such things as glyph
 * variants, shaped forms, alternative compositions, and so on. NFKD and NFKC normalization replaces these
 * code points with canonical characters or character sequences, and you cannot convert back to the original
 * code points. In principle, such compatibility variants should not be used.
 *
 * from: https://www.w3.org/International/questions/qa-html-css-normalization
 *
 *
 *
 * Standards:
 *     'C'  or 'NFC'  === Normalization Form C  (UAX #15)
 *     'D'  or 'NFD'  === Normalization Form D  (UAX #15)
 *     'KC' or 'NFKC' === Normalization Form KC (UAX #15)
 *     'KD' or 'NFKD' === Normalization Form KD (UAX #15)
 *
 *     'FCD'          === "Fast C or D" Form    (UTN #5)
 *     'FCC'          === "Fast C Contiguous"   (UTN #5)
 *
 *
 *  NFD: Characters are decomposed by canonical equivalence
 *       "â" (U+00E2) -> "a" (U+0061) + " ̂" (U+0302)
 * NFKD: Characters are decomposed by compatibility
 *       "ﬁ" (U+FB01) -> "f" (U+0066) + "i" (U+0069)
 *  NFC: Characters are decomposed and then re-composed by canonical equivalence
 *       "â" (U+00E2) -> "a" (U+0061) + " ̂" (U+0302) -> "â" (U+00E2)
 * NFKC: Characters are decomposed by compatibility, then re-composed by canonical equivalence
 *       "ﬁ" (U+FB01) -> "f" (U+0066) + "i" (U+0069) -> "f" (U+0066) + "i" (U+0069)
 *
 * "NFC" is preffered: https://www.win.tue.nl/~aeb/linux/uc/nfc_vs_nfd.html
 *
 *
 * Terms:
 *       CCC: Canonical Combining Class
 *            A numeric value in the range 0..254 given to each Unicode code point.
 *            The combining class for each encoded character in the standard is specified in the
 *            file UnicodeData.txt in the Unicode Character Database.
 *            Default Value: 0
 *            DerivedCombiningClass.txt contains these values as well.
 *
 *   Starter: Any code point (assigned or not) with combining class of zero (ccc = 0).
 *
 *            Description    gc    ccc    Starter
 *            -----------------------------------
 *            Nonspacing     Mn    0      Yes
 *                                 > 0    No
 *            Spacing        Mc    0      Yes
 *                                 > 0    No
 *            Enclosing      Me    0      Yes
 *
 *        gc: General Category (such as letter, digit, punctuation, symbol, or mark)
 *
 *            "Mc" stands for "spacing combining mark," which refers to a character that can be combined with
 *            a base character to create a new character with a different visual appearance. These marks are
 *            called "spacing" because they take up space and are displayed as a separate glyph, unlike
 *            "non-spacing combining marks" (Mn) that are visually attached to the base character without
 *            taking up additional space.
 *
 *   Reorderable pair:
 *             Two adjacent characters A and B in a coded character sequence <A, B> are a Reorderable Pair if
 *             and only if ccc(A) > ccc(B) > 0.
 *
 *  Canonical Ordering Algorithm:
 *             In a decomposed character sequence D, exchange the positions of the characters in each
 *             Reorderable Pair until the sequence contains no more Reorderable Pairs.
 *
 * More information: https://www.w3.org/TR/charmod-norm/
 */

#ifndef WEBPP_UNICODE_NORMALIZATION_HPP
#define WEBPP_UNICODE_NORMALIZATION_HPP

#include "../std/iterator.hpp"
#include "../std/type_traits.hpp"
#include "./details/ccc_tables.hpp"

namespace webpp::unicode {


    /**
     * Normalization form constants to describe which normalization algorithm
     * should be performed.
     *
     * Also see:
     * - Unicode Standard, §2.12 Equivalent Sequences
     * - Unicode Standard, §3.11 Normalization Forms
     * - https://unicode.org/reports/tr15/
     */
    enum struct normalization_form : stl::uint8_t {
        compose,  // NFC: Normalization Form C (Prefered by W3C, Linux, and others)
        decmpose, // NFD: Normalization Form D
        NFKC,     // Normalization Form KC
        NFKD,     // Normalization Form KD
    };

    /// Canonical Combining Class
    template <typename CharT = char32_t>
    [[nodiscard]] static constexpr stl::uint8_t ccc_of(CharT const code_point) noexcept {
        using unicode::details::ccc_index;
        using unicode::details::ccc_values;
        using unicode::details::trailing_zero_cccs;

        // The CCC of anything bigger than this number is zero because zero is the default by unicode standard
        if (code_point >= static_cast<CharT>(trailing_zero_cccs)) [[unlikely]] {
            return 0;
        }

        // First we look up the ccc_index table,
        // ccc_index table references ccc_values table.
        // ccc_index values are like this:
        //   helper code: [16bit = start-pos] + [8bit = shift] + [8bit = mask]
        //
        //   start-pos: the staring position of this 256-length bucket of CCC inside ccc_values table
        //   mask:      the mask will be applied to position of that 256 range, not the values, and not to the
        //              whole index position
        //   shift:     the value you find will be shifted by this amount
        auto const code_point_range = static_cast<stl::size_t>(code_point) >> 8U;
        auto const code_point_index = static_cast<stl::uint8_t>(code_point);
        auto const helper           = ccc_index[code_point_range];

        // extract information from the helper code:
        auto const mask       = static_cast<stl::uint8_t>(helper);
        auto const shift      = static_cast<stl::uint8_t>(helper >> 8);
        auto const start_pos  = static_cast<stl::size_t>(helper >> 16U);
        auto const masked_pos = static_cast<stl::size_t>(mask & code_point_index);

        // calculating the position of te value in the ccc_values table:
        stl::size_t const index_pos = start_pos + masked_pos;
        return ccc_values[index_pos] + shift;
    }

    /**
     * A starter code point in Unicode is a base character that can be combined with combining characters to
     * form a grapheme cluster. Specifically:
     *
     * A starter is a code point that is not a combining character. It is a base character, spacing mark,
     * enclosing mark, or format control character that can start a grapheme cluster[1].
     *
     * Combining characters are code points that are typically rendered by applying them to the preceding
     * character. They include nonspacing marks, spacing marks, and enclosing marks[1].
     *
     * A grapheme cluster is a sequence of one or more Unicode code points that represent a single
     * grapheme-like unit. It consists of a base character (starter) followed by zero or more combining
     * characters[1].
     *
     * So in summary, a starter code point is a base character that can start a grapheme cluster and be
     * combined with combining characters to represent a single written unit. It is the first code point in a
     * grapheme cluster.
     */
    template <typename CharT = char32_t>
    [[nodiscard]] static constexpr bool is_starter(CharT const code_point) noexcept {
        return ccc_of(code_point) == 0;
    }

    /**
     * Reorderable pair:
     *       Two adjacent characters A and B in a coded character sequence <A, B> are
     *       a Reorderable Pair if and only if ccc(A) > ccc(B) > 0
     *
     * Canonical Ordering Algorithm:
     *       In a decomposed character sequence D, exchange the positions of the characters in each
     *       Reorderable Pair until the sequence contains no more Reorderable Pairs.
     *
     * More information about Canonical Ordering Algorithm:
     *       (section 3.11 of https://www.unicode.org/versions/latest)
     *
     * Reorderable Pairs:
     *       Classes Reorderable?  Reason
     *       -------------------- ---------------
     *       No                    ccc(A) = 0
     *       No                    ccc(B) = 0
     *       No                    ccc(A) = ccc(B)
     *       No                    ccc(A) < ccc(B)
     *       Yes                   ccc(A) > ccc(B)
     */
    template <typename Iter, typename EIter = Iter>
    static constexpr void canonical_reorder(Iter start, EIter end) noexcept {}

    /**
     * Is a normalized unicode string
     * UTX #15: https://www.unicode.org/reports/tr15/tr15-54.html
     *
     * When implementations keep strings in a normalized form, they can be assured that equivalent strings
     * have a unique binary representation
     *
     * @tparam Iter iterator
     * @tparam EIter end iterator
     * @param start start position
     * @param end if you don't pass it, it'll look for one character
     * @return true if it's normalized unicode
     */
    // template <normalization_form NF = normalization_form::compose,
    //           typename Iter,
    //           typename EIter = istl::nothing_type>
    // [[nodiscard]] static constexpr bool is_normalized(Iter start, EIter end = {}) noexcept {
    //     using char_type = typename stl::iterator_traits<Iter>::value_type;
    //     return false;
    // }

} // namespace webpp::unicode

#endif // WEBPP_UNICODE_NORMALIZATION_HPP
