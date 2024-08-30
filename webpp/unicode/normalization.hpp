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
#include "../std/string.hpp"
#include "../std/string_like.hpp"
#include "../std/type_traits.hpp"
#include "./details/ccc_tables.hpp"
#include "./details/composition_tables.hpp"
#include "./details/decomposition_tables.hpp"
#include "./hangul.hpp"
#include "./unicode.hpp"

#include <cassert>

namespace webpp::unicode {


    /**
     * Normalization form constants to describe which normalization algorithm should be performed.
     *
     * Also see:
     * - Unicode Standard, §2.12 Equivalent Sequences
     * - Unicode Standard, §3.11 Normalization Forms
     * - https://unicode.org/reports/tr15/
     */
    enum struct normalization_form : stl::uint8_t {
        compose,  // NFC: Normalization Form C (Preferred by W3C, Linux, and others)
        decmpose, // NFD: Normalization Form D
        NFKC,     // Normalization Form KC
        NFKD,     // Normalization Form KD
    };

    /// Canonical Combining Class
    template <stl::integral CharT = char32_t>
    [[nodiscard]] static constexpr stl::uint8_t ccc_of(CharT const code_point) noexcept {
        using details::ccc_index;
        using details::ccc_indices;
        using details::ccc_values;
        using details::trailing_zero_cccs;

        // The CCC of anything bigger than this number is zero because zero is the default by Unicode standard
        if (code_point >= static_cast<CharT>(trailing_zero_cccs)) [[unlikely]] {
            return 0;
        }

        // Look at the ccc_index table, for how this works:
        auto const code = ccc_indices[static_cast<stl::uint32_t>(code_point) >> ccc_index::chunk_shift];

        // calculating the position of te value in the ccc_values table:
        return ccc_values[code.get_position(code_point)];
    }

    /// Canonical Combining Class
    template <stl::input_iterator Iter = char8_t const*>
    [[nodiscard]] static constexpr stl::uint8_t ccc_of(Iter const pos) noexcept {
        return ccc_of(unicode::next_code_point_copy(pos));
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
    template <stl::integral CharT = char32_t>
    [[nodiscard]] static constexpr bool is_starter(CharT const code_point) noexcept {
        return ccc_of(code_point) == 0;
    }

    template <stl::input_iterator Iter = char8_t const*>
    [[nodiscard]] static constexpr bool is_starter(Iter const pos) noexcept {
        return ccc_of(unicode::next_code_point_copy(pos)) == 0;
    }

    /**
     * Function to check if a combining character sequence is blocked based on combining classes
     * See Section 3.11, D115 of Version 15.1.0 of the Unicode Standard.
     *
     * In Unicode, a "blocked" combining character sequence is one where a base character is followed by
     * one or more combining characters, but the sequence is not a valid representation of a single
     * abstract character. This can happen when the combining characters have a combining class that
     * prevents them from being applied to the base character.
     *
     * For example, the sequence <a, combining_grave, combining_acute> is blocked because the combining
     * acute accent cannot be applied to the base character after the combining grave accent has already
     * been applied. The combining classes of the combining characters determine the order in which they
     * can be applied to the base character.
     *
     * Attention: this function does not check validity of the iterator itself.
     */
    template <stl::input_iterator Iter>
    [[nodiscard]] static constexpr bool is_blocked(Iter const inp) noexcept {
        return ccc_of(*inp) >= ccc_of(*stl::next(inp));
    }

    /**
     * Sort Marks
     * Reorder-able pair:
     *       Two adjacent characters A and B in a coded character sequence <A, B> are
     *       a Reorder-able Pair if and only if ccc(A) > ccc(B) > 0
     *
     * Canonical Ordering Algorithm:
     *       In a decomposed character sequence D, exchange the positions of the characters in each
     *       Reorder-able Pair until the sequence contains no more Reorder-able Pairs.
     *
     * More information about Canonical Ordering Algorithm:
     *       (section 3.11 of https://www.unicode.org/versions/latest)
     *
     * Reorder-able Pairs:
     *       Classes Reorder-able?  Reason
     *       -------------------- ---------------
     *       No                    ccc(A) = 0
     *       No                    ccc(B) = 0
     *       No                    ccc(A) = ccc(B)
     *       No                    ccc(A) < ccc(B)
     *       Yes                   ccc(A) > ccc(B)
     */
    template <stl::indirectly_swappable Iter = char8_t*, stl::indirectly_swappable EIter = Iter>
    static constexpr void canonical_reorder(Iter start, EIter end) noexcept(
      stl::is_nothrow_swappable_v<typename stl::iterator_traits<Iter>::value_type>) {
        using unchecked::next_char_copy;
        using unchecked::swap_code_points;

        for (auto pos = next_char_copy(start); pos != end;) {
            auto const ccc = ccc_of(next_code_point(pos));
            if (ccc == 0) {
                continue; // Skip non-combining characters (starter code points)
            }

            auto back_pos = pos;
            while (back_pos != start) {
                auto prev = back_pos;
                if (auto const prev_cp = prev_code_point(prev); ccc_of(prev_cp) <= ccc) {
                    back_pos = prev;
                    continue;
                }
                swap_code_points(back_pos, prev);
                back_pos = prev;
            }
        }
    }

    // /**
    //  * Is a normalized Unicode string
    //  * UTX #15: https://www.unicode.org/reports/tr15/tr15-54.html
    //  *
    //  * When implementations keep strings in a normalized form, they can be assured that equivalent strings
    //  * have a unique binary representation
    //  *
    //  * @tparam Iter iterator
    //  * @tparam EIter end iterator
    //  * @param start start position
    //  * @param end if you don't pass it, it'll look for one character
    //  * @return true if it's normalized unicode
    //  */
    // template <normalization_form NF = normalization_form::compose,
    //           typename Iter,
    //           typename EIter = istl::nothing_type>
    // [[nodiscard]] static constexpr bool is_normalized(Iter start, EIter end = {}) noexcept {
    //     using char_type = typename stl::iterator_traits<Iter>::value_type;
    //     return false;
    // }


    // NOLINTBEGIN(*-avoid-nested-conditional-operator)
    template <istl::CharType CharT = char8_t>
    static constexpr auto max_decomposed_length =
      UTF32<CharT> ? details::decomp_index::max_utf32_mapped_length
                   : (UTF16<CharT> ? details::decomp_index::max_utf16_mapped_length
                                   : details::decomp_index::max_utf8_mapped_length);
    // NOLINTEND(*-avoid-nested-conditional-operator)

    template <istl::CharType CharT = char8_t>
    using decomposed_array = std::array<CharT, max_decomposed_length<CharT>>;

    /**
     * Decompose the `code_point` into `out`.
     * @tparam Iter Iter can be an array, iterator, string, or similar types.
     * @returns the number of mapped values
     */
    template <istl::Appendable       Iter  = std::u8string::iterator,
              stl::unsigned_integral SizeT = istl::size_type_of_t<Iter>,
              istl::CharType         CharT = char32_t>
        requires UTF32<CharT>
    static constexpr SizeT decompose_to(Iter& out, CharT const code_point) noexcept(
      istl::NothrowAppendable<Iter>) {
        using details::decomp_index;
        using details::decomp_indices;
        using details::trailing_mapped_deomps;
        using unicode::unchecked::append;

        // Not mapped
        if (static_cast<stl::uint32_t>(code_point) >= trailing_mapped_deomps) [[unlikely]] {
            return append<Iter, SizeT>(out, code_point);
        }

        // It's Hangul, so we can answer algorithmically instead of looking it up in the lookup tables
        if (is_hangul_code_point(code_point)) [[unlikely]] {
            return decompose_hangul<Iter, SizeT>(out, code_point);
        }

        // Look at the ccc_index table, for how this works:
        auto const code_point_range = static_cast<stl::size_t>(code_point) >> decomp_index::chunk_shift;
        auto const code             = decomp_indices[code_point_range];

        // Not mapped at all, that means the code point is mapped to itself.
        if (code.max_length == 0) [[likely]] {
            return append<Iter, SizeT>(out, code_point);
        }

        auto const start_ptr = decomp_ptr(code, code_point);
        auto       ptr       = start_ptr;
        auto const end_ptr   = start_ptr + code.max_length;

        webpp_assume(code.max_length <= decomp_index::max_utf8_mapped_length);
        while (*ptr != u8'\0' && ptr != end_ptr) {
            append<Iter, SizeT>(out, ptr);
        }
        webpp_assume(static_cast<stl::size_t>(start_ptr - ptr) <= decomp_index::max_utf8_mapped_length);

        auto const len = static_cast<SizeT>(ptr - start_ptr);

        if (len == 0) [[likely]] {
            return append<Iter, SizeT>(out, code_point);
        }

        return len;
    }

    /**
     * Decompose to an array/string
     */
    template <istl::AppendableStorage StrT  = decomposed_array<>,
              istl::CharType          CharT = char32_t,
              typename... Args>
    [[nodiscard]] static constexpr StrT decomposed(CharT const code_point, Args&&... args) noexcept(
      istl::NothrowAppendable<StrT>) {
        StrT arr{stl::forward<Args>(args)...};
        auto iter = istl::appendable_iter_of(arr);
        decompose_to(iter, code_point);
        return arr;
    }

    /**
     * Decompose the `code_point` into `out`.
     * @tparam Iter Iter can be an array, iterator, string, or similar types.
     * @returns the number of mapped values
     */
    template <istl::Appendable Iter   = std::u8string::iterator,
              stl::integral    SizeT  = istl::size_type_of_t<Iter>,
              istl::Iterable   InpStr = stl::u32string_view>
    static constexpr SizeT decompose_to(Iter& out, InpStr const str) noexcept(istl::NothrowAppendable<Iter>) {
        SizeT count = 0;
        for (auto pos = stl::begin(str); pos != str.end();) {
            count += decompose_to(out, next_code_point(pos));
        }
        return count;
    }

    template <istl::AppendableStorage StrT   = decomposed_array<>,
              istl::Iterable          InpStr = stl::u32string_view,
              typename... Args>
        requires(stl::constructible_from<StrT, Args...>)
    [[nodiscard]] static constexpr StrT decomposed(InpStr&& str, Args&&... args) noexcept(
      istl::NothrowAppendable<StrT>) {
        StrT arr{stl::forward<Args>(args)...};
        auto iter = istl::appendable_iter_of(arr);
        decompose_to(iter, stl::forward<InpStr>(str));
        return arr;
    }

    namespace details {

        template <stl::integral SizeT = stl::size_t>
        struct decomposition_details {
            SizeT max_length       = 0; // max possible length required for decompositions
            bool  requires_mapping = false;
        };

        /**
         * Finding out these things:
         *   1. the max length required (the sum of max_lengths essentially)
         *   2. and should we continue mapping or not
         */
        template <stl::integral               SizeT = stl::size_t,
                  stl::random_access_iterator Iter  = stl::u8string::const_iterator,
                  stl::random_access_iterator EIter = Iter>
        static constexpr decomposition_details<SizeT> decomp_details(Iter pos, EIter end) noexcept {
            using details::decomp_index;
            using details::decomp_indices;
            using details::decomp_values;
            using details::trailing_mapped_deomps;

            using char_type = typename std::iterator_traits<Iter>::value_type;

            decomposition_details<SizeT> info;
            auto const                   actual_length = static_cast<SizeT>(end - pos);
            while (pos != end) {
                auto const code_point = unicode::next_code_point(pos);

                // handling hangul code points
                if (is_hangul_code_point(code_point)) [[unlikely]] {
                    info.max_length +=
                      hangul_decompose_length<char_type, decltype(code_point), SizeT>(code_point);
                    continue;
                }

                if (static_cast<stl::uint32_t>(code_point) >= trailing_mapped_deomps) [[unlikely]] {
                    continue;
                }

                // Look at the ccc_index table, for how this works:
                auto const code_point_range =
                  static_cast<stl::size_t>(code_point) >> decomp_index::chunk_shift;
                auto const code = decomp_indices[code_point_range];

                // calculating the length of te value in the decomp_values table:
                info.max_length += code.max_length;
                if (code.max_length != 0) {
                    info.requires_mapping = true;
                }
            }

            info.max_length += actual_length;
            assert(info.max_length >= actual_length);
            return info;
        }
    } // namespace details

    /**
     * Get the max required length for the specified range of code points
     * Attention: this does not return the exact size.
     * @tparam Iter
     * @tparam EIter Iter
     * @tparam SizeT std::size_t
     * @param pos start position
     * @param end end of the string
     */
    template <typename Iter, typename EIter = Iter, stl::unsigned_integral SizeT = stl::size_t>
    [[nodiscard]] static constexpr SizeT decomposition_max_size(Iter pos, EIter end) noexcept {
        return details::decomp_details<Iter, EIter, SizeT>(pos, end).max_length;
    }

    /**
     * Decompose inplace
     */
    template <istl::String StrT = stl::u32string>
    static constexpr void decompose(StrT& out) {
        using size_type = typename stl::remove_cvref_t<StrT>::size_type;

        auto const [max_length, requires_mapping] = details::decomp_details(out.begin(), out.end());
        auto const cur_len                        = out.size();

        if (!requires_mapping) [[likely]] {
            return;
        }

        assert(out.size() <= max_length);
        auto const overwrite =
          [cur_len](auto* ptr, stl::size_t const length /* = max_length */) constexpr noexcept {
              auto const* const beg = ptr;

              if (cur_len == length) {
                  auto       backup_start = ptr;
                  auto const backup_end   = ptr + cur_len;
                  while (backup_start != backup_end) {
                      decompose_to(ptr, next_code_point(backup_start));
                  }
                  return static_cast<size_type>(ptr - beg);
              }
              auto       backup_start = ptr + length - cur_len;
              auto const backup_end   = ptr + length;

              // moving everything to the end
              stl::copy(ptr, ptr + cur_len, backup_start);

              while (backup_start != backup_end) {
                  decompose_to(ptr, next_code_point(backup_start));
              }
              return static_cast<size_type>(ptr - beg);
          };
        if constexpr (requires { out.resize_and_overwrite(max_length, overwrite); }) {
            out.resize_and_overwrite(max_length, overwrite);
        } else {
            out.resize(max_length);
            out.resize(overwrite(out.data(), max_length));
        }
    }

    /**
     * Compose 2 code points into one
     * Attention: it'll return 0xFFFD (replacement character) if they're not valid inputs
     *
     * Canonical Composition code points are embedded inside Decomposition tables to save space.
     */
    template <UTF32 CharT = char32_t>
    [[nodiscard]] static constexpr CharT composed(CharT const lhs, CharT const rhs) noexcept {
        using details::composition::cp1s;
        using details::composition::cp2s;
        using details::composition::cp2s_rem;

        // no need to check if lhs or rhs are in range, the cp2s_rem will take care of such situation.

        // there are less second code points, so there will be more early bailouts
        auto const [cp2, cp1_pos, cp1_rem] =
          cp2s[static_cast<stl::size_t>(rhs) % static_cast<stl::size_t>(cp2s_rem)];

        // early bailout:
        if (cp2 == 0 || static_cast<CharT>(cp2) != rhs) {
            auto const hangul = compose_hangul(lhs, rhs);
            if (hangul != 0) {
                return hangul;
            }
            return replacement_char<CharT>;
        }

        auto [cp1, value] = cp1s[static_cast<stl::size_t>(cp1_pos + (lhs % cp1_rem))];
        if ((static_cast<CharT>(cp1) | lhs) != lhs) {
            return replacement_char<CharT>;
        }
        return static_cast<CharT>(value);
    }

    /**
     * Compose a unicode string inplace
     *
     * Attention: this function does NOT decompose, meaning, this function can be used in
     * NFC normalization, but itself is NOT NFC.
     *
     * This functio is indeed noexcept, and that's not a mistake; but in case you know your
     * string type might throw, you can explicitly specify it as the template parameter.
     *
     * String types won't throw since composing (that doesn't do decompose first) will only
     * reduce the size of the string and it cannot make the string bigger, thus no allocations
     * would be required.
     */
    template <istl::String StrT = stl::u32string, bool isNothrow = true>
    static constexpr void compose(StrT& out) noexcept(isNothrow) {
        using size_type = typename stl::remove_cvref_t<StrT>::size_type;

        auto const* const beg = out.data();
        auto*             ptr = out.data();
        auto*             rep = out.data();
        auto const* const end = ptr + out.size();
        char32_t          cp1 = next_code_point(ptr, end);

        while (ptr != end) {
            char32_t const cp2         = next_code_point(ptr, end);
            auto const     replaced_cp = composed(cp1, cp2);
            if (replaced_cp == replacement_char<char32_t>) {
                unchecked::append(rep, cp1);
                cp1 = cp2;
            } else {
                unchecked::append(rep, replaced_cp);
                cp1 = next_code_point(ptr, end);
            }
        }
        out.resize(static_cast<size_type>(rep - beg));
    }

} // namespace webpp::unicode

#endif // WEBPP_UNICODE_NORMALIZATION_HPP
