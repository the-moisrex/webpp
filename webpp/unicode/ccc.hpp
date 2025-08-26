// Created by moisrex on 4/3/25.

#ifndef WEBPP_UNICODE_CCC_HPP
#define WEBPP_UNICODE_CCC_HPP

#include "./details/ccc_tables.hpp"
#include "./unicode.hpp"

#include <print>

namespace webpp::unicode {

    static constexpr std::uint8_t max_canonical_combining_classes = 255;

    /**
     * Get CCC (Canonical Combining Class) and QC (Quick Check) info of the inputted Code Point
     */
    [[nodiscard]] static constexpr stl::uint16_t qc_ccc_of(char32_t const code_point) noexcept {
        using details::ccc_index;
        using details::ccc_indices;
        using details::ccc_values;
        using details::trailing_zero_cccs;

        // The CCC of anything bigger than this number is zero because zero is the default by Unicode standard
        if (code_point >= static_cast<char32_t>(trailing_zero_cccs)) [[unlikely]] {
            return 0;
        }

        // NOLINTBEGIN(*-pro-bounds-constant-array-index)
        // Look at the ccc_index table for how this works:
        auto const code = ccc_indices[static_cast<stl::uint32_t>(code_point) >> ccc_index::chunk_shift];

        // calculating the position of the value in the ccc_values table:
        return ccc_values[code.get_position(code_point)];
        // NOLINTEND(*-pro-bounds-constant-array-index)
    }

    /// Canonical Combining Class
    [[nodiscard]] static constexpr stl::uint8_t ccc_of(char32_t const code_point) noexcept {
        // NOLINTNEXTLINE(*-magic-numbers)
        return static_cast<stl::uint8_t>(qc_ccc_of(code_point) & 0xFFU);
    }

    /// Canonical Combining Class
    template <stl::input_iterator Iter = char8_t const*>
    [[nodiscard]] static constexpr stl::uint8_t ccc_of(Iter const pos) noexcept {
        return ccc_of(unchecked::next_code_point_copy(pos));
    }

    /**
     * A starter code point in Unicode is a base character that can be combined with combining characters to
     * form a grapheme cluster. Specifically:
     *
     * A starter is a code point that is not a combining character. It is a base character, spacing mark,
     * enclosing mark, or format control character that can start a grapheme cluster[1].
     *
     * Combining characters are code points that are typically rendered by applying them to the preceding
     * character. They include non-spacing marks, spacing marks, and enclosing marks[1].
     *
     * A grapheme cluster is a sequence of one or more Unicode code points that represent a single
     * grapheme-like unit. It consists of a base character (starter) followed by zero or more combining
     * characters[1].
     *
     * So in summary, a starter code point is a base character that can start a grapheme cluster and be
     * combined with combining characters to represent a single written unit. It is the first code point in a
     * grapheme cluster.
     */
    [[nodiscard]] static constexpr bool is_starter(char32_t const code_point) noexcept {
        return ccc_of(code_point) == 0;
    }

    template <stl::input_iterator Iter = char8_t const*>
    [[nodiscard]] static constexpr bool is_starter(Iter const pos) noexcept {
        return ccc_of(unchecked::next_code_point_copy(pos)) == 0;
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
     * Attention: this function does not check the validity of the iterator itself.
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
     *       https://www.unicode.org/versions/Unicode16.0.0/core-spec/chapter-3/#G49591
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
    template <stl::indirectly_swappable Iter = char8_t*, typename EIter = Iter>
        requires stl::sentinel_for<EIter, Iter>
    static constexpr void canonically_reorder(Iter const start, EIter const& end)
      noexcept(stl::is_nothrow_swappable_v<stl::iter_value_t<Iter>>) {
        using checked::next_code_point;
        using checked::prev_code_point;
        using unchecked::swap_code_points;
        using enum checked::error_handling;

        // using replacement character here in this function does not change the length of the string.

        if (start == end) {
            return;
        }

        auto pos = istl::deref(start);
        static_cast<void>(next_code_point(pos, end));
        while (pos != end) {
            auto       back_pos = istl::deref(pos);
            auto       cur_cp   = next_code_point<return_replacement_char>(pos, end);
            auto const ccc      = ccc_of(cur_cp);
            if (ccc == 0) {
                if (pos == end) {
                    break;
                }
                // skip the next code point as well, the next one is never going to be swapped with this one
                checked::next_char<Iter>(pos, end);
                continue; // Skip non-combining characters (starter code points)
            }

            // todo: instead of swapping code points, use one single rotate or move_backward
            while (back_pos != start) {
                auto       prev    = istl::deref(back_pos);
                auto const prev_cp = prev_code_point<return_replacement_char>(prev, start);
                if (ccc_of(prev_cp) <= ccc) {
                    break;
                }
                swap_code_points<Iter>(back_pos, prev);
                back_pos = prev;
            }
        }
    }

    /**
     * Check if the string is canonically ordered (will be when it has run through
     * the canonical_reorder function).
     */
    template <stl::bidirectional_iterator Iter, typename EIter = Iter>
        requires stl::sentinel_for<EIter, Iter>
    [[nodiscard]] static constexpr bool is_canonically_ordered(Iter const start, EIter const end) noexcept {
        using checked::next_code_point;
        using checked::prev_code_point;
        using enum checked::error_handling;

        if (start == end) {
            return true;
        }

        auto pos = istl::deref(start);
        static_cast<void>(next_code_point(pos, end));
        while (pos != end) {
            auto       back_pos = istl::deref(pos);
            auto       cur_cp   = next_code_point<return_replacement_char>(pos, end);
            auto const ccc      = ccc_of(cur_cp);
            if (ccc == 0) {
                if (pos == end) {
                    break;
                }
                // skip the next code point as well, the next one is never going to be swapped with this one
                checked::next_char<Iter>(pos, end);
                continue; // Skip non-combining characters (starter code points)
            }

            while (back_pos != start) {
                auto       prev     = istl::deref(back_pos);
                auto const prev_cp  = prev_code_point<return_replacement_char>(prev, start);
                auto const prev_ccc = ccc_of(prev_cp);
                if (prev_ccc <= ccc) {
                    break;
                }
                [[unlikely]] { return false; }
            }
        }
        return true;
    }

    template <istl::String StrT = stl::u32string>
    static constexpr void canonically_reorder(StrT& out)
      noexcept(stl::is_nothrow_swappable_v<stl::iter_value_t<stl::ranges::iterator_t<StrT>>>) {
        canonically_reorder<stl::ranges::iterator_t<StrT>>(stl::begin(out), stl::end(out));
    }

    template <istl::String StrT = stl::u32string>
    [[nodiscard]] static constexpr StrT canonically_reordered(StrT out) {
        canonically_reorder<stl::ranges::iterator_t<StrT>>(stl::begin(out), stl::end(out));
        return out;
    }

    /**
     * Iterator wrapper to convert a forward iterator into a something that it's combining marks are sorted.
     * This is equivalent to run the canonical reordering algorithm on the string.
     */
    template <stl::forward_iterator Iter, typename EIter = stl::default_sentinel_t>
        requires(stl::sentinel_for<EIter, Iter>)
    struct sorted_combining_marks_iterator {
        using difference_type   = stl::iter_difference_t<Iter>;
        using value_type        = stl::iter_value_t<Iter>;
        using traits            = stl::iterator_traits<Iter>;
        using pointer           = typename traits::pointer;
        using reference         = value_type&;
        using const_reference   = value_type const&;
        using iterator_category = stl::forward_iterator_tag;
        using iterator_concept  = stl::forward_iterator_tag;

        static_assert(UTF32<value_type>,
                      "Reordered iterator only works with UTF-32 iterators. Try wrapping that iterator.");

      private:
        enum struct state_type : stl::uint8_t {
            process, // We don't know what's what!
            random,  // We have to search
            sorted,  // It's already sorted
            rotate,  // It's half sorted, we need to rotate
        } state = state_type::process;
        [[no_unique_address]] Iter  beg{};
        [[no_unique_address]] Iter  cur{};
        [[no_unique_address]] Iter  nxt{};
        [[no_unique_address]] EIter endp{};

        stl::size_t length = 0;
        stl::size_t pccc   = 0;

        constexpr void find_state() noexcept {
            auto prev_ccc         = 0;
            state                 = state_type::sorted;
            length                = 0;
            beg                   = cur;
            nxt                   = beg;
            stl::uint8_t smallest = max_canonical_combining_classes; // smallest until we go into rotate state
            for (; nxt != endp; ++nxt) {
                ++length;
                auto const ccc = ccc_of(*nxt);
                if (ccc == 0) {
                    break;
                }
                switch (state) {
                    case state_type::sorted:
                        if (ccc < prev_ccc) {
                            cur   = nxt;
                            state = state_type::rotate;
                        } else {
                            smallest = stl::min(ccc, smallest);
                        }
                        break;
                    case state_type::process:
                    case state_type::rotate:
                        if (ccc <= prev_ccc || ccc <= smallest) {
                            state  = state_type::random;
                            length = 0;
                            pccc   = 0;
                            cur    = beg;
                            search_next();
                            return;
                        }
                        break;
                    case state_type::random: stl::unreachable();
                    default: smallest = stl::min(ccc, smallest); break;
                }
                prev_ccc = ccc;
            }
        }

        constexpr void search_next() noexcept {
            auto       smallest   = max_canonical_combining_classes;
            bool       found      = false;
            auto const prev_index = length + 1;
            auto const cpccc      = pccc;
            length                = 0;
            nxt = cur = beg;
            std::println("Searching Next");
            for (stl::size_t index = 0;; ++nxt) {
                if (nxt == endp) {
                    if (!found) {
                        cur = beg = nxt;
                    }
                    break;
                }
                auto const ccc = ccc_of(*nxt);
                std::println(
                  "  Testing U+{:X} {}<=>{}<=>{} {}-{}",
                  static_cast<int>(*nxt),
                  ccc,
                  cpccc,
                  smallest,
                  index,
                  prev_index);
                if (ccc == 0) {
                    if (!found) { // Next Code Point is a starter Code Point
                        std::println("Not Found.");
                        cur = beg = nxt;
                        state     = state_type::sorted;
                        length    = 1;
                        pccc      = 0;
                    }
                    break;
                }
                if (ccc < smallest && (ccc > cpccc || (ccc == cpccc && ++index == prev_index))) {
                    std::println(
                      "U+{:X} -- #{}-{} {}>={} && {}<{}",
                      static_cast<int>(*nxt),
                      index,
                      prev_index,
                      ccc,
                      cpccc,
                      ccc,
                      smallest);
                    cur      = nxt;
                    length   = index;
                    found    = true;
                    smallest = ccc;
                    pccc     = ccc;
                }
            }
        }

        constexpr void next() noexcept {
            switch (state) {
                case state_type::random: search_next(); break;
                case state_type::sorted:
                    ++cur;
                    if (--length == 0) {
                        find_state();
                    }
                    break;
                case state_type::rotate:
                    if (--length == 0) {
                        cur = nxt;
                        find_state();
                    }
                    if (++cur == nxt) {
                        cur = beg;
                    }
                    break;
                case state_type::process: stl::unreachable();
            }
        }

      public:
        explicit constexpr sorted_combining_marks_iterator(Iter inp_pos, EIter inp_end = EIter{}) noexcept
          : beg{inp_pos},
            cur{inp_pos},
            nxt{inp_pos},
            endp{inp_end} {
            find_state();
        }

        constexpr sorted_combining_marks_iterator()                                                      = default;
        constexpr sorted_combining_marks_iterator(sorted_combining_marks_iterator const&)                = default;
        constexpr sorted_combining_marks_iterator(sorted_combining_marks_iterator&&) noexcept            = default;
        constexpr sorted_combining_marks_iterator& operator=(sorted_combining_marks_iterator const&)     = default;
        constexpr sorted_combining_marks_iterator& operator=(sorted_combining_marks_iterator&&) noexcept = default;
        constexpr ~sorted_combining_marks_iterator() noexcept                                            = default;

        constexpr sorted_combining_marks_iterator& operator++() noexcept {
            if (cur != endp) {
                next();
            }
            return *this;
        }

        constexpr const_reference operator*() const noexcept {
            return *cur;
        }

        [[nodiscard]] constexpr sorted_combining_marks_iterator operator++(int) noexcept {
            auto const res = sorted_combining_marks_iterator{*this};
            operator++();
            return res;
        }

        [[nodiscard]] constexpr bool operator==(sorted_combining_marks_iterator const& other) const noexcept {
            return cur == other.cur;
        }

        [[nodiscard]] constexpr bool operator==(Iter const& other) const noexcept {
            return cur == other;
        }

        [[nodiscard]] constexpr bool operator==(EIter const& other) const noexcept {
            return cur == other;
        }

        [[nodiscard]] constexpr bool operator==(stl::default_sentinel_t) const noexcept
            requires(!stl::same_as<EIter, stl::default_sentinel_t>)
        {
            return at_end();
        }

        [[nodiscard]] constexpr bool at_end() const noexcept {
            return cur == endp;
        }
    };

} // namespace webpp::unicode

#endif // WEBPP_UNICODE_CCC_HPP
