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
 * "NFC" is preferred: https://www.win.tue.nl/~aeb/linux/uc/nfc_vs_nfd.html
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
#include "../std/string_view.hpp"
#include "../std/type_traits.hpp"
#include "./details/composition_tables.hpp"
#include "./details/decomposition_tables.hpp"
#include "./hangul.hpp"
#include "./quick_check.hpp"
#include "./unicode.hpp"
#include "./utf32_iterator.hpp"
#include "./utf_reducer.hpp"

#include <cassert>

namespace webpp::unicode {

    // NOLINTBEGIN(*-avoid-nested-conditional-operator)
    template <UTF CharT = char8_t>
    static constexpr auto max_decomposed_length =
      UTF32<CharT>   ? details::decomp_index::max_utf32_mapped_length
      : UTF16<CharT> ? details::decomp_index::max_utf16_mapped_length
                     : details::decomp_index::max_utf8_mapped_length;
    // NOLINTEND(*-avoid-nested-conditional-operator)

    template <UTF CharT = char8_t>
    using decomposed_array = std::array<CharT, max_decomposed_length<CharT> + 1U>; // +1 for null terminator

    /**
     * Decompose the `code_point` into `out`.
     * @tparam Iter Iter can be an array, iterator, string, or similar types.
     * @returns the UTF-8 length of mapped values
     */
    template <istl::Appendable Iter = std::u8string::iterator>
    static constexpr stl::size_t canonical_decompose_to(Iter& out, char32_t const code_point)
      noexcept(istl::NothrowAppendable<Iter>) {
        using details::decomp_breakpoints;
        using details::decomp_common_pos;
        using details::decomp_index;
        using details::decomp_indices;
        using details::decomp_values;
        using unchecked::append;

        // Not mapped
        // if (static_cast<stl::uint32_t>(code_point) >= trailing_mapped_decomps) [[unlikely]] {
        //     return append<Iter, SizeT>(out, code_point);
        // }

        // It's Hangul, so we can answer algorithmically instead of looking it up in the lookup tables
        if (is_hangul_code_point(code_point)) {
            return decompose_hangul<Iter>(out, code_point);
        }

        // NOLINTBEGIN(*-pro-bounds-constant-array-index, *-pro-bounds-pointer-arithmetic)
        auto const chunk         = static_cast<std::uint32_t>(code_point) >> decomp_index::chunk_shift;
        auto const section_index = static_cast<stl::uint16_t>(chunk >> details::decomp_breakpoint_shift);
        if (chunk >= details::decomp_last_breakpoint) [[unlikely]] {
            return append<Iter>(out, code_point);
        }
        auto const [starting, ending, offset] = decomp_breakpoints[section_index];
        decomp_index const code =
          chunk < starting || chunk >= ending
            ? decomp_common_pos
            : decomp_indices[static_cast<stl::uint16_t>(chunk - offset)];

        // Not mapped at all, that means the code point is mapped to itself.
        if (code.max_length == 0) {
            return append<Iter>(out, code_point);
        }

        auto const* const start_ptr = decomp_ptr(code, code_point);
        auto const*       ptr       = start_ptr;
        auto const* const end_ptr   = start_ptr + code.max_length;
        // NOLINTEND(*-pro-bounds-constant-array-index, *-pro-bounds-pointer-arithmetic)

        webpp_assume(code.max_length <= decomp_index::max_utf8_mapped_length);
        while (*ptr != u8'\0' && ptr != end_ptr) {
            append<Iter>(out, ptr); // append increments ptr
        }
        webpp_assume(static_cast<stl::size_t>(start_ptr - ptr) <= decomp_index::max_utf8_mapped_length);

        auto const len = static_cast<stl::size_t>(ptr - start_ptr);
        if (len == 0) {
            return append<Iter>(out, code_point);
        }
        return len; // UTF-8 Length regardless of the output type.
    }

    /**
     * Canonical Decompose Into the specified output.
     * This function is not the same as taking char32_t as input since bad UTF-8 code units that have
     * been turned into UTF-32 will not go back to being UTF-8 the same way they came in.
     */
    template <istl::Appendable Iter = std::u8string::iterator, stl::forward_iterator SIter, typename SEIter = SIter>
        requires stl::sentinel_for<SEIter, SIter>
    static constexpr stl::size_t canonical_decompose_to(Iter& out, SIter& spos, SEIter const& send)
      noexcept(istl::NothrowAppendable<Iter>) {
        using enum checked::error_handling;

        assert(spos != send);
        auto const code_point = checked::next_code_point<return_replacement_char>(spos, send);
        return canonical_decompose_to(out, code_point);
    }

    template <istl::Appendable Iter = std::u8string::iterator, stl::forward_iterator SIter, typename SEIter = SIter>
        requires stl::sentinel_for<SEIter, SIter>
    static constexpr stl::size_t canonical_decompose_prev_to(Iter& out, SIter& spos, SEIter const& sbeg)
      noexcept(istl::NothrowAppendable<Iter>) {
        using enum checked::error_handling;

        auto const code_point = checked::prev_code_point<return_replacement_char>(spos, sbeg);
        return canonical_decompose_to(out, code_point);
    }

    /**
     * Go to the first Code Point that requires Decomposition.
     */
    template <stl::forward_iterator Iter, typename EIter = Iter>
        requires stl::sentinel_for<EIter, Iter>
    static constexpr void skip_to_decomp(Iter& spos, EIter const& send) noexcept {
        using details::decomp_breakpoints;
        using details::decomp_common_pos;
        using details::decomp_index;
        using details::decomp_indices;
        using stl::swap;
        using enum checked::error_handling;

        // The spos is periodically moved because we don't want spos to point to
        // invalid code points at any time.
        for (auto pos = spos; pos != send; spos = pos) {
            auto const code_point = checked::next_code_point<return_negated>(pos, send);

            // we'll consider replacement character as a special one.
            if (static_cast<stl::int32_t>(code_point) < 0 || is_hangul_code_point(code_point)) {
                break;
            }

            // invalid characters are needed to be skipped too, we cast the code point into unsigned integer,
            // and it'll cause the same effect.
            auto const chunk         = static_cast<stl::uint32_t>(code_point) >> decomp_index::chunk_shift;
            auto const section_index = static_cast<stl::uint16_t>(chunk >> details::decomp_breakpoint_shift);
            if (chunk >= details::decomp_last_breakpoint) [[unlikely]] {
                continue;
            }
            auto const [starting, ending, offset] = decomp_breakpoints[section_index];
            decomp_index const code =
              chunk < starting || chunk >= ending
                ? decomp_common_pos
                : decomp_indices[static_cast<stl::uint16_t>(chunk - offset)];

            // Not mapped at all; that means the code point is mapped to itself.
            if (code.max_length == 0) {
                continue;
            }

            auto const start_ptr = decomp_ptr(code, code_point);
            if (*start_ptr != u8'\0') {
                break;
            }
        }
    }

    /**
     * Decompose to an array/string
     */
    template <istl::AppendableStorage StrT = decomposed_array<>, typename... Args>
    [[nodiscard]] static constexpr StrT canonical_decomposed(char32_t const code_point, Args&&... args)
      noexcept(istl::NothrowAppendable<StrT>) {
        StrT arr{stl::forward<Args>(args)...};
        auto iter = istl::appendable_iter_of(arr);
        canonical_decompose_to(iter, code_point);
        return arr;
    }

    /// Get the max length required for decomposition
    template <UTF InCharT = char32_t, UTF OutCharT = InCharT>
    [[nodiscard]] static constexpr stl::size_t decomp_max_required_length(stl::size_t const orig_size) noexcept {
        // There is also a Unicode Consortium stability policy that canonical mappings are always limited
        // in all versions of Unicode, so that no string when decomposed with NFC expands to more than 3x
        // in length (measured in code units). This is true whether the text is in UTF-8, UTF-16, or
        // UTF-32. This guarantee also allows for certain optimizations in processing, especially in
        // determining buffer sizes.
        return adjust_utf_output_size<InCharT, OutCharT>(orig_size * details::max_decomp_expand_factor);
    }

    template <stl::random_access_iterator Iter, stl::random_access_iterator OIter = Iter>
    static constexpr void canonical_decompose(Iter spos, Iter send, OIter& ptr, stl::size_t const max_length) noexcept {
        using enum checked::error_handling;
        using diff_type     = stl::iter_difference_t<OIter>;
        using in_char_type  = stl::iter_value_t<Iter>;
        using out_char_type = stl::iter_value_t<OIter>;

        auto const orig_len = static_cast<stl::size_t>(send - spos);
        auto const ptr_beg  = ptr;

        if constexpr (stl::is_convertible_v<Iter, OIter>) {
            // Output cannot be in between the input, out == spos is okay, it's an inplace decomposition.
            assert(!(ptr > spos && ptr < send));
        }

        assert((max_length >= decomp_max_required_length<in_char_type, out_char_type>(orig_len)));

        // inplace decomposition has been asked of us:
        if constexpr (stl::same_as<Iter, OIter>) {
            if (ptr == spos) {
                auto const sbeg = spos;
                skip_to_decomp(spos, send);
                // moving everything to the end
                // spos may be const, so we change the output pointer
                auto const cur_len    = spos - sbeg;
                auto const rem_len    = send - spos;
                auto const rest_start = static_cast<diff_type>(max_length) - static_cast<diff_type>(rem_len);
                stl::copy_n(spos, rem_len, stl::next(ptr, rest_start));
                stl::advance(ptr, cur_len);
                spos = stl::next(sbeg, rest_start);
                send = stl::next(sbeg, static_cast<diff_type>(max_length));
            }
        }

        while (spos != send) {
            canonical_decompose_to(ptr, spos, send);
        }

        assert(max_length >= static_cast<stl::size_t>(ptr - ptr_beg));
    }

    template <istl::Appendable StrT = stl::u32string, stl::random_access_iterator Iter>
    static constexpr void canonical_decompose(Iter spos, Iter send, StrT& out) noexcept(istl::NothrowAppendable<StrT>) {
        using size_type = stl::size_t;
        using enum checked::error_handling;
        using in_char_type  = stl::iter_value_t<Iter>;
        using out_char_type = istl::char_type_of_t<StrT>;

        if constexpr (istl::String<StrT>) {
            if constexpr (stl::convertible_to<Iter, typename StrT::const_iterator>) {
                // The source should not be the output; for inplace decomposition, use its own function.
                assert(!(spos >= out.begin() && spos < out.end()));
                assert(!(send >= out.begin() && send < out.end()));
            }

            // A poor man's attempt at not allocating if the string doesn't require decomposition.
            auto const sbeg = spos;
            skip_to_decomp(spos, send);
            if (spos == send) {
                // copy it over
                out.append(sbeg, spos);
                return;
            }

            auto const new_len     = static_cast<size_type>(send - spos);
            auto const skipped_len = static_cast<size_type>(spos - sbeg);
            auto const max_len     = out.size() + adjust_utf_output_size<in_char_type, out_char_type>(skipped_len) +
                                 decomp_max_required_length<in_char_type, out_char_type>(new_len);

            istl::resize_and_overwrite(
              out,
              max_len,
              [out_len = out.size(), sbeg, skipped_len, &spos, send](
                auto*                            ptr,
                [[maybe_unused]] size_type const length) constexpr noexcept {
                  auto const beg = ptr;
                  stl::advance(ptr, out_len); // We're going to append to the string
                  stl::copy_n(sbeg, skipped_len, ptr);
                  stl::advance(ptr, skipped_len);

                  while (spos != send) {
                      canonical_decompose_to(ptr, spos, send);
                  }

                  auto const str_len = static_cast<size_type>(ptr - beg);
                  assert(str_len <= length);
                  return str_len;
              });
        } else {
            auto const      cur_len    = static_cast<size_type>(send - spos);
            size_type const max_length = decomp_max_required_length<in_char_type, out_char_type>(cur_len);
            canonical_decompose(spos, send, out, max_length);
        }
    }

    template <istl::Appendable StrT = stl::u32string, istl::StringViewifiable InpStrT>
    static constexpr void canonical_decompose(InpStrT&& src, StrT& out) noexcept(istl::NothrowAppendable<StrT>) {
        auto const strv = istl::string_viewify(stl::forward<InpStrT>(src));
        canonical_decompose(strv.begin(), strv.end(), out);
    }

    /**
     * Decompose inplace
     */
    template <istl::String StrT = stl::u32string>
    static constexpr void canonical_decompose(StrT& out) {
        using size_type = typename StrT::size_type;
        using diff_type = typename StrT::difference_type;
        using enum checked::error_handling;

        // A poor man's attempt at not allocating if the string doesn't require decomposition.
        auto const sbeg = out.begin();
        auto       spos = out.begin();
        auto const send = out.end();
        skip_to_decomp(spos, send);
        if (spos == send) [[likely]] {
            return;
        }
        auto const new_len     = static_cast<size_type>(send - spos);
        auto const skipped_len = static_cast<size_type>(spos - sbeg);

        // no need to use adjust_utf_size since input and output are the same
        auto const max_len = skipped_len + decomp_max_required_length(new_len);
        istl::resize_and_overwrite(
          out,
          max_len,
          [new_len, skipped_len](auto* ptr, size_type const max_length) constexpr noexcept {
              // copy the skipped Code Points
              auto const beg  = ptr;
              auto       sptr = stl::next(ptr, static_cast<diff_type>(max_length - new_len)); // source start
              auto const sfin = stl::next(ptr, static_cast<diff_type>(max_length));           // source end
              stl::advance(ptr, skipped_len);

              // moving the source to the end
              stl::copy_n(ptr, new_len, sptr);

              while (sptr != sfin) {
                  canonical_decompose_to(ptr, sptr, sfin);
              }
              auto const written_len = static_cast<size_type>(ptr - beg);
              assert(ptr <= sfin);
              assert(written_len <= max_length);
              return written_len;
          });
    }

    template <istl::String OStrT = stl::u32string, istl::StringViewifiable InpStr, typename... Args>
    [[nodiscard]] static constexpr OStrT canonical_decomposed(InpStr&& src, Args&&... args) {
        auto const strv = istl::string_viewify(stl::forward<InpStr>(src));
        OStrT      out{stl::forward<Args>(args)...};
        canonical_decompose(strv.begin(), strv.end(), out);
        return out;
    }

    /**
     * Compose 2 code points into one
     * Attention: You could return 0xFFFD (replacement character) or 0 if they're not valid inputs
     */
    [[nodiscard]] static constexpr char32_t canonical_composed(
      char32_t const lhs,
      char32_t const rhs,
      char32_t const error = replacement_char<char32_t>) noexcept {
        using details::composition::cp1s;
        using details::composition::cp2s;
        using details::composition::cp2s_rem;


        // there are fewer second code points, so there will be more early bailouts
        stl::size_t const pos2 = static_cast<stl::size_t>(rhs) % static_cast<stl::size_t>(cp2s_rem);
        if (pos2 >= cp2s.size()) [[unlikely]] {
            return error;
        }
        // NOLINTBEGIN(*-pro-bounds-constant-array-index)
        auto const [cp2, cp1_pos, cp1_rem] = cp2s[pos2];

        // todo: use -1 as invalid values for cp2 instead of 0 to eliminate the necessity of cp2 == 0 comparison
        if (cp2 == 0 || cp2 != rhs) {
            auto const hangul = compose_hangul(lhs, rhs);
            return hangul != 0 ? hangul : error;
        }

        stl::size_t const pos              = cp1_pos + static_cast<stl::size_t>(lhs % cp1_rem);
        // there's no need to check if the position here is valid or not, the `cp1s` table is guaranteed to
        // have the max number of elements.
        auto const [cp1_mask, replacement] = cp1s[pos];
        // NOLINTEND(*-pro-bounds-constant-array-index)

        bool has_error  = !is_code_point_valid(lhs);
        has_error      |= !is_code_point_valid(rhs);
        has_error       |= static_cast<std::uint16_t>(lhs & details::composition::cp1_mask) != cp1_mask;
        // has_error      |= lhs == 0;
        // has_error      |= rhs == 0;
        if (has_error) [[unlikely]] {
            return error;
        }
        return replacement;
    }

    /**
     * Compose a Unicode string inplace
     *
     * Attention: this function does NOT decompose, meaning this function can be used in
     * NFC normalization, but itself is NOT NFC.
     *
     * @returns The new length of the string. Specified end is no longer valid.
     */
    template <stl::integral               SizeT = stl::size_t,
              stl::random_access_iterator Iter  = char32_t*,
              typename EIter                    = char32_t const* const>
        requires stl::sentinel_for<EIter, Iter>
    [[nodiscard("Use the new size to resize the container.")]] static constexpr SizeT canonical_compose(
      Iter& ptr,
      EIter end) noexcept(stl::is_nothrow_copy_assignable_v<stl::iter_value_t<Iter>>) {
        using reducer_type = utf_reducer<4, Iter>;

        reducer_type           reducer{ptr, static_cast<stl::size_t>(end - ptr)};
        utf_range_marker<Iter> hole;
        auto [starter_pin, rep_pin, cp1_pin, cp2_pin] = reducer.pins();
        for (; cp1_pin != reducer.end(); ++cp1_pin, ++rep_pin) {
            starter_pin = rep_pin;
            cp2_pin     = cp1_pin;
            ++cp2_pin;
            auto cp1 = *cp1_pin;
            for (stl::int_fast16_t prev_ccc = -1; cp2_pin != reducer.end(); ++cp1_pin, ++cp2_pin) {
                auto const cp2         = *cp2_pin;
                auto const ccc         = static_cast<stl::int_fast16_t>(ccc_of(cp2));
                auto const replaced_cp = canonical_composed(cp1, cp2, U'\0');
                if (prev_ccc < ccc && replaced_cp != U'\0') {
                    // found a composition of cp1 and cp2
                    cp1 = replaced_cp;
                    hole.append_code_point(cp2_pin.iter(), reducer.end(), reducer.all_pins());
                    continue;
                }
                if (ccc == 0) [[likely]] {
                    break;
                }
                prev_ccc = ccc;

                utf_range_marker<Iter> cp2_hole;
                cp2_hole.mark_code_point(cp2_pin.iter(), reducer.end());
                ++rep_pin;
                rep_pin.set(cp2, cp2_hole, hole);
                rep_pin.fallback_hole(hole, cp2_hole);
            }

            // use the hole if you run out of space in UTF-8 and UTF-16 mode
            starter_pin.set(cp1, hole);
            reducer.snap_hole_to_end(hole);
        }
        reducer.set_end(rep_pin);
        return static_cast<SizeT>(reducer.size());
    }

    /**
     * Compose a Unicode string inplace
     *
     * Attention: this function does NOT decompose, meaning this function can be used in
     * NFC normalization, but itself is NOT NFC.
     *
     * This function is indeed noexcept, and that's not a mistake; but in case you know your
     * string type might throw, you can explicitly specify it as the template parameter.
     *
     * String types won't throw since composing (that doesn't do decompose first) will only
     * reduce the size of the string, and it cannot make the string bigger, thus no allocations
     * would be required.
     */
    template <istl::String StrT = stl::u32string, bool isNothrow = true>
    static constexpr void canonical_compose(StrT& out) noexcept(isNothrow) {
        using size_type = typename stl::remove_cvref_t<StrT>::size_type;

        auto*             ptr = out.data();
        auto const* const end = ptr + out.size();

        out.resize(canonical_compose<size_type>(ptr, end));
    }

    template <istl::String StrT = stl::u32string>
    [[nodiscard(
      "Use unicode::canonical_compose instead of this if you wanted to compose "
      "inplace")]] static constexpr StrT
    canonical_composed(StrT out) {
        canonical_compose<StrT>(out);
        return out;
    }

    /**
     * Normalize a string inplace.
     *
     * Normalization in Unicode is a process that standardizes different binary representations of characters
     * to ensure that equivalent characters are represented consistently. This is essential for
     * accurate string comparison and processing in software applications.
     *
     * Check out the normalization FAQ to find out the max length of normalization:
     *    https://www.unicode.org/faq/normalization.html
     *
     * @tparam Form Normalization Form
     * @tparam StrT String type
     * @param out the string you want to be normalized
     */
    template <norm_form Form = norm_form::NFC, istl::String StrT = stl::u32string>
    static constexpr void normalize(StrT& out) {
        // We don't need to reserve it, canonical_decompose will do it.
        // out.reserve(out.size() * 3 + 1);

        if constexpr (norm_form::gibberish == Form) {
            throw std::invalid_argument(
              "We don't know what your intentions are, but calling this function and ask to normalize it to "
              "gibberish is not it.");
        } else if constexpr (norm_form::NFD == Form) {
            canonical_decompose(out);
            canonically_reorder(out);
        } else if constexpr (norm_form::NFC == Form) {
            canonical_decompose(out);
            canonically_reorder(out);
            canonical_compose(out);
        } else {
            // todo: NFKC and NFKD
            throw stl::invalid_argument("NFKC and NFKD are not yet implemented.");
        }
    }

    /**
     * Normalize a string (possibly) inplace.
     *
     * Normalization in Unicode is a process that standardizes different binary representations of characters
     * to ensure that equivalent characters are represented consistently. This is essential for
     * accurate string comparison and processing in software applications.
     *
     * Check out the normalization FAQ to find out the max length of normalization:
     *    https://www.unicode.org/faq/normalization.html
     */
    template <norm_form Form = norm_form::NFC, istl::Appendable StrT = stl::u32string, stl::random_access_iterator Iter>
    static constexpr void normalize(Iter spos, Iter send, StrT& out) noexcept(istl::NothrowAppendable<StrT>) {
        if constexpr (norm_form::gibberish == Form) {
            throw std::invalid_argument(
              "We don't know what your intentions are, but calling this function and ask to normalize it to "
              "gibberish is not it.");
        } else if constexpr (norm_form::NFD == Form) {
            if constexpr (istl::String<StrT>) {
                canonical_decompose(spos, send, out);
                canonically_reorder(out);
            } else {
                stl::random_access_iterator auto const obeg = out;
                canonical_decompose(spos, send, out);
                stl::random_access_iterator auto const oend = out;
                out                                         = obeg + canonical_compose(obeg, oend);
            }
        } else if constexpr (norm_form::NFC == Form) {
            // Q: Is text always the same length or shorter after being put into NFC?
            // Although it is usually the same length or shorter, it may expand. One of the goals for NFC was
            // to match legacy practice where possible, and in some cases, the legacy representation was
            // decomposed. In addition, for stability, characters encoded after Unicode 3.0 do not compose,
            // except in unusual circumstances. See UAX #15 for more details.
            // https://www.unicode.org/faq/normalization.html
            if constexpr (istl::String<StrT>) {
                canonical_decompose(spos, send, out);
                canonically_reorder(out);
                canonical_compose(out);
            } else {
                using diff_type                       = stl::iter_difference_t<StrT>;
                stl::random_access_iterator auto obeg = out;
                canonical_decompose(spos, send, out);
                stl::random_access_iterator auto const oend = out;
                canonically_reorder(obeg, oend);
                out = stl::next(obeg, static_cast<diff_type>(canonical_compose(obeg, oend)));
            }
        } else {
            // todo: NFKC and NFKD
            throw stl::invalid_argument("NFKC and NFKD are not yet implemented.");
        }
    }

    /**
     * Decomposition Iterator Wrapper
     * It decomposes the iterator given.
     *
     * This is not the most performant way of doing this, so use it in slow paths of your code.
     * This allocates NOTHING.
     */
    template <stl::forward_iterator Iter, typename EIter = stl::default_sentinel_t>
        requires stl::sentinel_for<EIter, Iter>
    struct decompose_iterator {
        using difference_type = stl::iter_difference_t<Iter>;
        using value_type      = stl::iter_value_t<Iter>;
        using traits          = stl::iterator_traits<Iter>;
        using pointer         = typename traits::pointer;
        using reference       = value_type&;
        using const_reference = value_type const&;

        static constexpr bool is_bidi = stl::bidirectional_iterator<Iter>;

        using iterator_category =
          stl::conditional_t<is_bidi, stl::bidirectional_iterator_tag, stl::forward_iterator_tag>;
        using iterator_concept = iterator_category;

      private:
        [[no_unique_address]] istl::begin_iterator<Iter> beg{};
        [[no_unique_address]] Iter                       cur{};
        [[no_unique_address]] Iter                       nxt{};
        [[no_unique_address]] EIter                      send{};
        stl::int8_t                                      index = 0;
        stl::int8_t                                      len   = 0;
        decomposed_array<value_type>                     buf{};

      public:
        explicit constexpr decompose_iterator(Iter inp_pos, EIter inp_end = {}) noexcept
          : beg{istl::begin_sentinel(inp_pos)},
            cur{inp_pos},
            nxt{inp_pos},
            send{stl::move(inp_end)} {
            if (nxt == send) {
                return;
            }
            auto cur_buf = buf.data();
            canonical_decompose_to(cur_buf, nxt, send);
            len = static_cast<stl::int8_t>(cur_buf - buf.data());
            assert(len >= 0 && static_cast<stl::size_t>(len) <= buf.size());
        }

        constexpr decompose_iterator()                                         = default;
        constexpr decompose_iterator(decompose_iterator const&)                = default;
        constexpr decompose_iterator(decompose_iterator&&) noexcept            = default;
        constexpr decompose_iterator& operator=(decompose_iterator const&)     = default;
        constexpr decompose_iterator& operator=(decompose_iterator&&) noexcept = default;
        constexpr ~decompose_iterator() noexcept                               = default;

        constexpr decompose_iterator& operator++() noexcept {
            // todo: we can optimize?
            ++index;
            if (index == len) {
                index = 0;
                cur   = nxt;
                if (send == nxt) {
                    len = 0;
                } else {
                    auto cur_buf = buf.data();
                    canonical_decompose_to(cur_buf, nxt, send);
                    len = static_cast<stl::int8_t>(cur_buf - buf.data());
                }
            }
            assert(len >= 0 && static_cast<stl::size_t>(len) <= buf.size());
            assert(index >= 0 && static_cast<stl::size_t>(index) < buf.size());
            return *this;
        }

        constexpr decompose_iterator& operator--() noexcept
            requires(is_bidi)
        {
            --index;
            if (index < 0) {
                nxt          = cur;
                auto cur_buf = buf.data();
                canonical_decompose_prev_to(cur_buf, cur, beg);
                len   = static_cast<stl::int8_t>(cur_buf - buf.data());
                index = stl::max<stl::int8_t>(len - 1, 0);
            }
            assert(len >= 0 && static_cast<stl::size_t>(len) <= buf.size());
            assert(index >= 0 && static_cast<stl::size_t>(index) < buf.size());
            return *this;
        }

        constexpr const_reference operator*() const noexcept {
            return buf[static_cast<stl::uint8_t>(index)];
        }

        [[nodiscard]] constexpr decompose_iterator operator--(int) noexcept
            requires(is_bidi)
        {
            auto const res = decompose_iterator{*this};
            operator--();
            return res;
        }

        [[nodiscard]] constexpr decompose_iterator operator++(int) noexcept {
            auto const res = decompose_iterator{*this};
            operator++();
            return res;
        }

        [[nodiscard]] constexpr bool operator==(decompose_iterator const& other) const noexcept {
            return cur == other.cur;
        }

        [[nodiscard]] constexpr bool operator==(EIter const&) const noexcept {
            return cur == send;
        }

        [[nodiscard]] constexpr bool operator==(stl::default_sentinel_t) const noexcept
            requires(!stl::convertible_to<EIter, stl::default_sentinel_t>)
        {
            return cur == send;
        }

        [[nodiscard]] constexpr bool operator==(istl::begin_sentinel_t) const noexcept {
            return cur == beg;
        }
    };

    /**
     * Check if the specified string is decomposable
     */
    template <stl::forward_iterator Iter>
    [[nodiscard]] static constexpr bool is_decomposable(Iter spos, Iter const& send) noexcept {
        skip_to_decomp(spos, send);
        return spos != send;
    }

    [[nodiscard]] static constexpr bool is_composable(char32_t const lhs, char32_t const rhs) noexcept {
        return canonical_composed(lhs, rhs, U'\0') != U'\0';
    }

    /**
     * Check if composing the decomposed string (spos/send) would result in the original string (cpos/cend).
     * This is used to verify that a string is in NFC form.
     *
     * Attention: this function requires the input to be canonically ordered and decomposed (== NFD).
     */
    template <stl::forward_iterator Iter, typename EIter = Iter, stl::forward_iterator CIter, typename CEIter = CIter>
        requires(stl::sentinel_for<EIter, Iter> && stl::sentinel_for<CEIter, CIter>)
    [[nodiscard]] static constexpr bool
    is_composable_to(Iter const& spos, EIter const& send, CIter cpos, CEIter const& cend) noexcept {
        static_assert(UTF32<stl::iter_value_t<Iter>>, "We need utf-32 string, wrap it in utf32 iterator.");
        static_assert(UTF32<stl::iter_value_t<CIter>>, "We need utf-32 string, wrap it in utf32 iterator.");

        // I'm not bailing out early and using is_valid because I want to put less branches on the path that gives true
        Iter cp1_pin  = spos;
        bool is_valid = true;
        while (cp1_pin != send) {
            if (cpos == cend) [[unlikely]] {
                return false;
            }

            auto       cp1         = *cp1_pin;
            auto const starter_ccp = *cpos;
            ++cp1_pin;

            Iter cp2_pin = cp1_pin;
            for (stl::int_fast16_t prev_ccc = -1; cp2_pin != send; ++cp1_pin, ++cp2_pin) {
                auto const cp2         = *cp2_pin;
                auto const ccc         = static_cast<stl::int_fast16_t>(ccc_of(cp2));
                auto const replaced_cp = canonical_composed(cp1, cp2, U'\0');
                if (prev_ccc < ccc && replaced_cp != U'\0') {
                    // found a composition
                    cp1 = replaced_cp;
                    continue;
                }
                if (ccc == 0) [[likely]] {
                    break;
                }
                prev_ccc = ccc;
                ++cpos;
                is_valid &= cpos != cend && *cpos == cp2;
            }
            is_valid &= starter_ccp == cp1;
            ++cpos;

            // bailout early, but not too early
            if (!is_valid) [[unlikely]] {
                return false;
            }
        }
        is_valid &= cpos == cend;
        return is_valid;
    }

    /**
     * Is a normalized Unicode string
     * UTX #15: https://www.unicode.org/reports/tr15/tr15-54.html
     *
     * When implementations keep strings in a normalized form, they can be assured that equivalent strings
     * have a unique binary representation
     */
    template <norm_form Form = norm_form::NFC, stl::random_access_iterator Iter>
    [[nodiscard]] static constexpr bool is_normalized(Iter spos, Iter const& send) noexcept {
        using enum norm_form;
        using enum quick_check_state;

        if constexpr (gibberish == Form) {
            static_assert_false(
              Iter,
              "We don't know what your intentions are, but calling this function and ask to normalize it to "
              "gibberish is not it.");
        } else if constexpr (NFD == Form) {
            // https://www.unicode.org/reports/tr15/tr15-54.html#NFD_Table_Optimization
            // The values of the Canonical_Combining_Class property are constrained by the character encoding
            // stability guarantees to the range 0..254; the value 255 will never be assigned for a
            // Canonical_Combining_Class value. Because of this constraint, implementations can make use of
            // 255 as an implementation-specific value for optimizing data tables. For example, one can do a
            // fast and compact table for implementing isNFD(x) by using the value 255 to represent
            // NFKC_QC=No.
            if constexpr (!details::exclude_NFD) {
                return quick_check<NFD>(spos, send) == YES;
            } else if (is_decomposable(spos, send) || !is_canonically_ordered(spos, send)) [[unlikely]] {
                return false;
            }
        } else if constexpr (NFC == Form) {
            // https://www.unicode.org/reports/tr15/tr15-54.html#NFC_QC_Optimization
            // When normalizing to NFC, rather than first decomposing the text fully, a quick check can be
            // made on each character. If it is already in the proper precomposed form, then no work has to be
            // done. Only if the current character is a combining mark or is in the Composition Exclusion
            // Table [Exclusions], does a slower code path need to be invoked. The slower code path will need
            // to look at previous characters, back to the last starter. See Section 9, Detecting
            // Normalization Forms, for more information.

            for (;;) {
                switch (quick_check_till_maybe<NFC>(spos, send)) {
                    [[likely]] case YES:
                        return true;
                    [[unlikely]] case NO:
                        return false;
                    case MAYBE: {
                        // Slow path:
                        Iter pos = spos;
                        switch (next_definite_starter(spos, send)) {
                            case NO: return false;
                            default: break;
                        }
                        checked::utf32_forward_iter const utf32_pos{pos, spos};
                        if (!is_composable_to(
                              sorted_combining_marks_iterator{
                                decompose_iterator{utf32_pos, stl::default_sentinel}
                        },
                              stl::default_sentinel,
                              utf32_pos,
                              stl::default_sentinel))
                        {
                            return false;
                        }
                        if (spos == send) {
                            return true;
                        }
                        break;
                    }
                    default: assert(false); stl::unreachable();
                }
            }
            return true;
        } else {
            // todo: NFKC and NFKD
            static_assert_false(Iter, "NFKC and NFKD are not yet implemented.");
        }
        return true;
    }

    /// to Normalization Form C (this is not inplace)
    template <istl::String StrT = stl::u32string>
    [[nodiscard]] static constexpr StrT toNFC(StrT src) {
        normalize<norm_form::NFC>(src);
        return src;
    }

    /// to Normalization Form D (this is not inplace)
    template <istl::String StrT = stl::u32string>
    [[nodiscard]] static constexpr StrT toNFD(StrT src) {
        normalize<norm_form::NFD>(src);
        return src;
    }

    /// Check the Normalization Form
    // template <stl::forward_iterator Iter>
    // [[nodiscard]] static constexpr norm_form normalization_form_of(
    //   [[maybe_unused]] Iter start,
    //   [[maybe_unused]] Iter end) noexcept {
    //     using enum norm_form;
    //     // todo
    //     static_assert_false(Iter, "Not yet implemented.");
    //     return gibberish;
    // }

    template <stl::forward_iterator Iter>
    [[nodiscard]] static constexpr bool isNFC(Iter start, Iter end) noexcept {
        return is_normalized<norm_form::NFC>(start, end);
    }

    template <istl::StringViewifiable StrT = stl::u32string_view>
    [[nodiscard]] static constexpr bool isNFC(StrT&& str) noexcept {
        auto str_view = istl::string_viewify(stl::forward<StrT>(str));
        return is_normalized<norm_form::NFC>(str_view.begin(), str_view.end());
    }

    template <stl::forward_iterator Iter>
    [[nodiscard]] static constexpr bool isNFD(Iter start, Iter end) noexcept {
        return is_normalized<norm_form::NFD>(start, end);
    }

    template <istl::StringViewifiable StrT = stl::u32string_view>
    [[nodiscard]] static constexpr bool isNFD(StrT&& str) noexcept {
        auto str_view = istl::string_viewify(stl::forward<StrT>(str));
        return is_normalized<norm_form::NFD>(str_view.begin(), str_view.end());
    }

    template <stl::forward_iterator Iter>
    [[nodiscard]] static constexpr bool isNFKC(Iter start, Iter end) noexcept {
        return is_normalized<norm_form::NFKC>(start, end);
    }

    template <istl::StringViewifiable StrT = stl::u32string_view>
    [[nodiscard]] static constexpr bool isNFKC(StrT&& str) noexcept {
        auto str_view = istl::string_viewify(stl::forward<StrT>(str));
        return is_normalized<norm_form::NFKC>(str_view.begin(), str_view.end());
    }

    template <stl::forward_iterator Iter>
    [[nodiscard]] static constexpr bool isNFKD(Iter start, Iter end) noexcept {
        return is_normalized<norm_form::NFKD>(start, end);
    }

    template <istl::StringViewifiable StrT = stl::u32string_view>
    [[nodiscard]] static constexpr bool isNFKD(StrT&& str) noexcept {
        auto str_view = istl::string_viewify(stl::forward<StrT>(str));
        return is_normalized<norm_form::NFKD>(str_view.begin(), str_view.end());
    }

} // namespace webpp::unicode

#endif // WEBPP_UNICODE_NORMALIZATION_HPP
