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
 */

#ifndef WEBPP_UNICODE_NORMALIZATION_HPP
#define WEBPP_UNICODE_NORMALIZATION_HPP

#include "../std/iterator.hpp"
#include "../std/type_traits.hpp"

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
    template <normalization_form NF = normalization_form::compose,
              typename Iter,
              typename EIter = istl::nothing_type>
    [[nodiscard]] static constexpr bool is_normalized(Iter start, EIter end = {}) noexcept {
        using char_type = typename stl::iterator_traits<Iter>::value_type;
        return false;
    }

} // namespace webpp::unicode

#endif // WEBPP_UNICODE_NORMALIZATION_HPP
