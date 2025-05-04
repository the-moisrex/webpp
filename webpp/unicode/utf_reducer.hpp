// Created by moisrex on 9/2/24.

#ifndef WEBPP_UNICODE_CODE_POINT_ITERATOR_HPP
#define WEBPP_UNICODE_CODE_POINT_ITERATOR_HPP

#include "../std/tuple.hpp"
#include "../std/type_traits.hpp"
#include "../std/utility.hpp"
#include "./unicode.hpp"

#include <array>
#include <cassert>

namespace webpp::unicode {

    /**
     * Mark a range of UTF-8 or UTF-16 code units
     * @tparam IterT Iterator Type
     */
    template <typename IterT = char8_t*>
    struct utf_range_marker {
        static_assert(stl::is_default_constructible_v<IterT>, "Iterator is not default constructible");

        using unit_type       = stl::iter_value_t<IterT>;
        using difference_type = stl::iter_difference_t<IterT>;
        using size_type       = stl::size_t;

      private:
        IterT beginp{};
        IterT endp{};

      public:
        constexpr utf_range_marker() noexcept                          = default;
        constexpr utf_range_marker(utf_range_marker const&)            = delete;
        constexpr utf_range_marker(utf_range_marker&&)                 = delete;
        constexpr utf_range_marker& operator=(utf_range_marker const&) = delete;
        constexpr utf_range_marker& operator=(utf_range_marker&&)      = delete;
        constexpr ~utf_range_marker() noexcept                         = default;

        // NOLINTNEXTLINE(*-easily-swappable-parameters)
        explicit constexpr utf_range_marker(IterT inp_beg, IterT inp_end) noexcept
          : beginp{inp_beg},
            endp{inp_end} {
            assert(beginp < endp);
        }

        // NOLINTNEXTLINE(*-easily-swappable-parameters)
        constexpr void mark(IterT inp_beg, IterT inp_end) noexcept {
            beginp = inp_beg;
            endp   = inp_end;
            assert(beginp < endp);
        }

        // NOLINTNEXTLINE(*-rvalue-reference-param-not-moved)
        constexpr void mark(utf_range_marker&& other) noexcept {
            assert(&other != this);
            if (other.empty()) {
                this->clear();
            } else {
                assert(other.beginp < other.endp);
                beginp = stl::exchange(other.beginp, istl::nullptr_of<decltype(other.beginp)>());
                endp   = stl::exchange(other.endp, istl::nullptr_of<decltype(other.endp)>());
                assert(beginp < endp);
            }
        }

        /// str_end is the end of the string, and has nothing to do with the hole's end
        constexpr void mark_code_point(IterT const& cp_beg, IterT const& str_end) noexcept {
            beginp = cp_beg;
            endp   = stl::next(beginp, checked::code_point_length<IterT, difference_type>(cp_beg, str_end));
            assert(beginp < endp);
        }

        [[nodiscard]] constexpr IterT begin() const noexcept {
            return beginp;
        }

        [[nodiscard]] constexpr IterT end() const noexcept {
            return endp;
        }

        [[nodiscard]] constexpr size_type size() const noexcept {
            return static_cast<size_type>(endp - beginp);
        }

        [[nodiscard]] constexpr bool empty() const noexcept {
            return beginp == endp;
        }

        constexpr void move_mark(difference_type diff) noexcept {
            if (empty()) {
                return;
            }
            beginp += diff;
            endp   += diff;
        }

        constexpr void shave_start(size_type index = 1) noexcept {
            assert(index <= size());
            beginp += static_cast<difference_type>(index);
            if (beginp >= endp) {
                this->clear();
            }
        }

        constexpr void shave_end(size_type index = 1) noexcept {
            assert(index <= size());
            endp -= static_cast<difference_type>(index);
            if (beginp >= endp) {
                this->clear();
            }
        }

        /// Remove part of the hole
        /// The behavior is undefined (asserted) if the specified range is in the middle of the hole.
        constexpr void shave(IterT const start, size_type const length = 1) noexcept {
            assert(length != 0);
            if (start <= beginp) {
                auto const new_length =
                  static_cast<difference_type>(length) - static_cast<difference_type>(beginp - start);
                if (new_length <= 0) {
                    return;
                }
                shave_start(static_cast<size_type>(new_length));
                return;
            }

            if (start >= endp) {
                return;
            }

            auto const tail_ptr = stl::next(beginp, static_cast<difference_type>(length));
            if (tail_ptr >= endp) {
                shave_end(static_cast<size_type>(tail_ptr - endp));
                return;
            }

            // It's in the middle of the hole
            [[unlikely]] {
                // let's just blow up for now, because moving requires access to iters
                assert(false);
            }
        }

        /// The direction of expansion is determined by the sign of the length
        constexpr void expand(difference_type length) noexcept {
            assert(length != 0);
            assert(!empty());
            if (length > 0) {
                endp += length;
            } else {
                beginp += length;
            }
        }

        /// Move the content of the hole
        constexpr void move_content(difference_type diff) noexcept {
            if constexpr (stl::is_pointer_v<IterT>) {
                assert(this->begin() != nullptr);
                assert(this->end() != nullptr);
            }
            assert(!this->empty());
            assert(this->begin() < this->end());
            if (diff == 0) {
                return;
            }
            auto const length = this->size();
            assert(length > 0);
            if (diff < 0) {
                stl::shift_right(beginp + diff, endp, static_cast<difference_type>(length));
            } else if (diff > 0) {
                stl::shift_left(beginp, endp + diff, static_cast<difference_type>(length));
            }
            beginp += diff;
            endp   += diff;
        }

        /// Move the pack of iterators that you pass, and move them to the right position according to the
        /// previously moved content by move_content.
        template <typename IterableT>
        constexpr void move_iterators(difference_type diff, IterableT& iters) noexcept {
            if constexpr (stl::is_pointer_v<IterT>) {
                assert(this->begin() != nullptr);
                assert(this->end() != nullptr);
            }
            assert(!this->empty());
            assert(this->begin() < this->end());
            auto const length  = this->size();
            auto const old_beg = beginp - diff;
            auto const old_end = endp - diff;
            auto const new_beg = beginp;
            auto const new_end = endp;
            if (diff < 0) {
                for (auto& cur : iters) {
                    if (cur >= new_beg && cur < old_beg) {
                        stl::advance(cur, length);
                    } else if (cur >= old_beg && cur < old_end) {
                        cur = old_end;
                        while (!is_code_unit_start(*--cur)) {
                            // moving the iterator to the beginning of the previous code point
                        }
                    }
                }
            } else if (diff > 0) {
                for (auto& cur : iters) {
                    if (cur >= old_end && cur < new_end) {
                        stl::advance(cur, -length);
                    } else if (cur >= old_beg && cur < old_end) {
                        cur = old_beg;
                        while (!is_code_unit_start(*--cur)) {
                            // moving the iterator to the beginning of the previous code point
                        }
                    }
                }
            }
        }

        /// Move the hole
        template <typename IterableT>
        constexpr void move(difference_type diff, IterableT& iters) noexcept {
            if (diff == 0 || empty()) {
                return;
            }
            move_content(diff);
            move_iterators(diff, iters);
        }

        /// Move a piece of the hole
        template <typename IterableT>
        constexpr void
        split_move(size_type const length, difference_type const diff, IterableT& iters) noexcept {
            assert(length != 0);
            assert(length <= size());
            assert(!empty());
            auto const old_beg = beginp;
            auto const old_end = endp;
            if (length == size()) {
                move_content(diff);
                move_iterators(diff, iters);
                return;
            }
            if (diff > 0) {
                // cutting the end half of the hole
                auto const mid = beginp + static_cast<difference_type>(length);
                beginp         = mid;
                move_content(diff);
                move_iterators(diff, iters);
                endp   = mid;
                beginp = old_beg;
            } else if (diff < 0) {
                // cutting the first half of the hole
                auto const mid = endp - static_cast<difference_type>(length);
                endp           = mid;
                move_content(diff);
                move_iterators(diff, iters);
                beginp = mid;
                endp   = old_end;
            } else if (diff == 0) {
                beginp += static_cast<difference_type>(length);
            }
            assert(beginp <= endp);
        }

        /// Storing the length of the hole, inside the hole itself.
        constexpr void sequence_fill() noexcept {
            auto cur_diff = this->size();
            for (auto& unit : *this) {
                unit = static_cast<unit_type>(unicode::utf_leading_code_units<unit_type>[cur_diff--]);
            }
        }

        /// Shrinks the code hole to the specified length, and fills the removed parts
        constexpr void shrink_into(size_type const length) noexcept {
            assert(length != 0);
            assert(length <= size());
            if (length == size()) {
                return;
            }
            auto const old_beginp = beginp;
            auto const mid        = beginp + static_cast<difference_type>(length);
            beginp                = mid;
            sequence_fill();
            endp   = mid;
            beginp = old_beginp;
        }

        /// Append a new hole to this hole, combining two holes
        template <typename IterableT>
        constexpr void append(utf_range_marker& other, IterableT& iters) noexcept {
            assert(other.begin() < other.end());
            assert(other.begin() >= endp || other.begin() < beginp);
            if (other.empty()) {
                return;
            }
            if (this->empty()) [[unlikely]] {
                beginp = other.begin();
                endp   = other.end();
            } else {
                auto const tail_diff = other.begin() - endp;
                if (tail_diff == 0) {
                    endp = other.end();
                    // for (auto& cur : iters) {
                    //     if (cur > beginp && cur < endp) {
                    //         cur = endp;
                    //     }
                    // }
                } else if (tail_diff > 0) {
                    other.move(-tail_diff, iters);
                    this->expand(static_cast<difference_type>(other.size()));
                } else {
                    auto const head_diff = beginp - other.end() - 1;
                    assert(head_diff > 0);
                    other.move(head_diff, iters);
                    this->expand(-static_cast<difference_type>(other.size()));
                }
            }
            other.clear();
        }

        template <typename IterableT>
        constexpr void append(IterT start, IterT end, IterableT& iters) noexcept {
            utf_range_marker other(start, end);
            this->append(other, iters);
        }

        template <typename IterableT>
        constexpr void append_code_point(IterT start, IterT str_end, IterableT& iters) noexcept {
            auto const end =
              stl::next(start, checked::code_point_length<IterT, difference_type>(start, str_end));
            utf_range_marker other(start, end);
            this->append(other, iters);
        }

        constexpr void clear() noexcept {
            beginp = endp = IterT{};
        }

        [[nodiscard]] constexpr bool has_overlaps(IterT const& start, IterT const& end) const noexcept {
            return endp >= start && end >= beginp;
        }

        [[nodiscard]] constexpr bool has_overlaps(IterT const& start, size_type length) const noexcept {
            return has_overlaps(start, stl::next(start, static_cast<difference_type>(length)));
        }
    };

    /// We don't need UTF-32 ranges, so we disable it
    template <typename IterT>
        requires(UTF32<stl::iter_value_t<IterT>>)
    struct utf_range_marker<IterT> {
        explicit constexpr utf_range_marker([[maybe_unused]] auto&&... args) noexcept {}

        constexpr void mark([[maybe_unused]] auto&&... args) noexcept {
            // do nothing
        }

        constexpr void mark_code_point([[maybe_unused]] auto&&... args) noexcept {
            // do nothing
        }

        constexpr void append_code_point([[maybe_unused]] auto&&... args) noexcept {
            // do nothing
        }

        [[nodiscard]] constexpr bool empty() const noexcept {
            return true;
        }

        constexpr void append([[maybe_unused]] auto&&... args) noexcept {
            // do nothing
        }

        constexpr void fallback_hole([[maybe_unused]] auto&... holes) noexcept {
            // do nothing
        }
    };

    template <stl::size_t PinCount = 1, typename IterT = char8_t*, UTF32 CodePointT = char32_t>
    struct utf_reducer;

    /**
     * Pin is a forward-iterator-like type that is designed to be used inside utf_reducer.
     *
     * Pin Acts (capabilities of pins):
     *   - Get: get the code point
     *   - Set (or Replace): replace a code point inplace if possible
     *   - Change Pin position to another pin location
     *   - Forward: go to the next code point
     *
     *
     *   enum struct pin_states {
     *       filled,  // [X|'|']      All code units are filled
     *       extra,   // [X|'|'] [']  There are extra code units left that can't be fit
     *       partial, // [X|'| ]      We've only used parts of the previous code units' space available
     *       // deleted  // [ | | ]      All code units are empty now
     *   };
     */
    template <std::size_t PinIndex = 0,
              std::size_t PinCount = 1,
              typename IterT       = char8_t*,
              UTF32 CodePointT     = char32_t>
    struct pin_type {
        static_assert(PinIndex < PinCount, "Either Pin count is invalid or Pin index.");

        using reducer_type    = utf_reducer<PinCount, IterT, CodePointT>;
        using iterator        = typename reducer_type::iterator;
        using size_type       = typename reducer_type::size_type;
        using value_type      = typename reducer_type::value_type;
        using unit_type       = typename reducer_type::unit_type;
        using difference_type = typename reducer_type::difference_type;

        static constexpr bool is_nothrow = reducer_type::is_nothrow;
        static constexpr auto npos       = reducer_type::npos;

      private:
        template <std::size_t, std::size_t, typename, UTF32>
        friend struct pin_type;

        template <typename T>
        using if_not_utf32 = stl::conditional_t<UTF32<unit_type>, istl::nothing_type, T>;

        template <typename T>
        using if_utf32 = stl::conditional_t<!UTF32<unit_type>, istl::nothing_type, T>;


        [[no_unique_address]] if_not_utf32<reducer_type*> reducer;
        [[no_unique_address]] if_utf32<iterator>          ptr;

        /// guarantee the correctness of the state that we're in
        constexpr void test_state_correctness() noexcept {
            if constexpr (!UTF32<unit_type>) {
                // guarantee that each pin's position will be less than or equal to the next one:
                // assert(reducer->template pin_iter<static_cast<difference_type>(PinIndex) + 1>() >= iter());

                // guarantee that each pin's position will be more than or equal to the previous one:
                assert(reducer->template pin_iter<static_cast<difference_type>(PinIndex) - 1>() <= iter());

                // range check
                assert(iter() <= reducer->end());
                assert(iter() >= reducer->begin());
            }

            // early blow up in case we did not find the correct ptr position:
            // assert(is_code_unit_start(*iter()));
        }

        /// The number of code units required to set this new code point with the specified length
        [[nodiscard]] constexpr stl::int_fast8_t required_code_units_of_len(stl::int_fast8_t const cp_len)
          noexcept(is_nothrow) {
            if constexpr (UTF32<unit_type>) {
                return 1; // always
            } else {
                stl::int_fast8_t count    = 0;
                auto             iter_cpy = istl::deref(iter());
                while (count <= cp_len) {
                    assert(iter_cpy <= reducer->endptr);
                    auto const cur_len =
                      checked::code_point_length<iterator, stl::int_fast8_t>(iter_cpy, reducer->end());
                    iter_cpy += cur_len;
                    count    += cur_len;
                }
                webpp_assume(count <= 6);
                return count;
            }
        }

        /// The number of code units required to set this new code point
        [[nodiscard]] constexpr stl::int_fast8_t required_code_units(value_type const inp_cp)
          noexcept(is_nothrow) {
            if constexpr (UTF32<unit_type>) {
                return 1; // always
            } else {
                return required_code_units_of_len(utf_length_from_utf32<unit_type, stl::int_fast8_t>(inp_cp));
            }
        }

      public:
        constexpr explicit pin_type(reducer_type* inp_reducer) noexcept
            requires(!UTF32<unit_type>)
          : reducer(inp_reducer) {}

        constexpr explicit pin_type(reducer_type* inp_reducer) noexcept
            requires(UTF32<unit_type>)
          : ptr(inp_reducer->begin()) {}

        constexpr pin_type(pin_type&& other) noexcept            = default;
        constexpr pin_type& operator=(pin_type&& other) noexcept = default;
        constexpr ~pin_type() noexcept                           = default;

        /// most likely a not so much great algorithm is being written, so, let's shut copying down:
        constexpr pin_type(pin_type const& other)            = delete;
        constexpr pin_type& operator=(pin_type const& other) = delete;

        /// Pin Act: Change position to another pin location
        template <stl::size_t OPinIndex>
        constexpr pin_type& operator=(
          pin_type<OPinIndex, PinCount, IterT, CodePointT> const& other) noexcept {
            static_assert(OPinIndex < PinCount,
                          "You cannot go back; these are forward-only set of constructs.");
            static_assert(OPinIndex != PinIndex, "Self-assignment is probably a mistake.");

            assert(other.iter() >= iter());
            if constexpr (UTF32<unit_type>) {
                ptr = other.ptr;
            } else {
                // this is not a copy assignment operator, it's a pin act.
                assert(reducer == other.reducer);
                assert(other.iter() <= reducer->end());
                reducer->iters[PinIndex] = reducer->iters[OPinIndex];
                test_state_correctness();
            }
            return *this;
        }

        /// Pin Act: Forward
        constexpr pin_type& operator++() noexcept(is_nothrow) {
            if constexpr (UTF32<unit_type>) {
                ++iter();
            } else {
                static_cast<void>(checked::next_char(iter(), reducer->end()));

                // bounds check:
                auto& cur = iter();
                if (cur > reducer->end()) {
                    cur = reducer->end();
                }
            }
            test_state_correctness();
            return *this;
        }

        [[nodiscard]] constexpr iterator& iter() noexcept {
            if constexpr (UTF32<unit_type>) {
                return ptr;
            } else {
                return reducer->iters[PinIndex];
            }
        }

        [[nodiscard]] constexpr iterator const& iter() const noexcept {
            if constexpr (UTF32<unit_type>) {
                return ptr;
            } else {
                return reducer->iters[PinIndex];
            }
        }

        [[nodiscard]] constexpr value_type operator*() const noexcept {
            if constexpr (UTF32<unit_type>) {
                return static_cast<value_type>(*iter());
            } else {
                using enum checked::error_handling;
                assert(iter() < reducer->end());
                return checked::next_code_point_copy<return_negated_char, value_type>(iter(), reducer->end());
            }
        }

        [[nodiscard]] constexpr iterator operator->() const noexcept {
            return iter();
        }

        template <typename NIterT = iterator>
        [[nodiscard]] constexpr bool operator==(NIterT other) const noexcept {
            return iter() == other;
        }

        template <typename NIterT = iterator>
        [[nodiscard]] constexpr bool operator!=(NIterT other) const noexcept {
            return iter() != other;
        }

        [[nodiscard]] constexpr stl::strong_ordering operator<=>(iterator other) const noexcept {
            // this might have unintentional consequences if other is in the middle of a UTF-8 code point
            // and the ptr is in the beginning of that code point. we're assuming pointers can't be
            // pointed to the middle of the Unicode code points.
            return iter() <=> other;
        }

        [[nodiscard]] constexpr stl::strong_ordering operator<=>(pin_type const& other) const noexcept {
            return iter() <=> other.iter();
        }

        constexpr pin_type& operator=(iterator other) noexcept(is_nothrow) {
            assert(other >= iter());
            if constexpr (UTF32<unit_type>) {
                ptr = other;
            } else {
                iter() = other;
            }
            test_state_correctness();
            return *this;
        }

        constexpr pin_type& operator=(value_type other) noexcept(is_nothrow) {
            set(other);
            return *this;
        }

        template <stl::size_t PinIndex2, typename Iter2>
        constexpr pin_type& operator=(pin_type<PinIndex2, PinCount, Iter2, CodePointT> const& other)
          noexcept(is_nothrow) {
            set(other);
            return *this;
        }

      private:
        constexpr void move_iterators(iterator old_iter, difference_type const diff) noexcept {
            if (diff != 0) {
                stl::advance(reducer->newend, diff);
                *reducer->newend = static_cast<unit_type>('\0');
            }
            for (auto index = PinIndex + 1; index < PinCount; ++index) {
                auto& cur = reducer->iters[index];
                if (cur >= old_iter) {
                    stl::advance(cur, diff);
                }
            }
        }

        /// Either shrink the hole at [loc end] by [+diff] amount,
        /// or Expand the hole at [loc end] by [-diff] amount.
        /// The extra space used is at [newend-endptr]
        constexpr void adjust_hole(iterator loc, difference_type diff) noexcept(is_nothrow) {
            assert(loc <= reducer->newend);
            assert(loc >= reducer->beg);
            if (diff == 0) {
                return;
            }
            if (diff > 0) {
                // We have additional space we need to fill
                auto const new_loc_end = loc - diff;
                stl::shift_left(new_loc_end, reducer->newend, diff);
                // It should be UB if there's some iterator between [new_loc_end, loc]
                move_iterators(loc, -diff);
            } else {
                // we need to make more space
                // we're making a hole now
                auto const length = -diff;
                assert(static_cast<difference_type>(reducer->empty_size()) >= length);
                stl::shift_right(loc, stl::next(reducer->newend, length), length);
                move_iterators(loc, length);
            }
        }

        constexpr void set_inplace(value_type            inp_code_point,
                                   difference_type const cur_len,
                                   difference_type const new_len) noexcept(is_nothrow) {
            if constexpr (UTF32<unit_type>) {
                *iter() = inp_code_point;
            } else {
                assert(iter() < reducer->newend);

                auto const diff     = cur_len - new_len;
                auto       iter_cpy = istl::deref(iter());

                adjust_hole(stl::next(iter(), cur_len), diff);
                auto const changed_length = unchecked::append(iter_cpy, inp_code_point);
                assert(static_cast<difference_type>(changed_length) == new_len);

                test_state_correctness();
            }
        }

      public:
        /// Pin Act: Set
        constexpr void set(value_type inp_code_point) noexcept(is_nothrow) {
            if constexpr (UTF32<unit_type>) {
                *iter() = inp_code_point;
            } else {
                assert(iter() < reducer->endptr);

                // handling invalid code points
                if (static_cast<stl::int32_t>(inp_code_point) < 0) [[unlikely]] {
                    *iter() = -static_cast<unit_type>(inp_code_point);
                    return;
                }

                auto const new_len = required_length_of<unit_type, stl::int_fast8_t>(inp_code_point);
                auto const cur_len =
                  checked::code_point_length<iterator, difference_type>(iter(), reducer->end());
                set_inplace(inp_code_point, cur_len, new_len);
            }
        }

        /// Pin Act: Set (and use the hole if needed as extra space)
        template <typename... HoleT>
            requires(stl::same_as<HoleT, utf_range_marker<iterator>> && ...)
        constexpr void set(value_type                                   inp_code_point,
                           [[maybe_unused]] utf_range_marker<iterator>& hole,
                           [[maybe_unused]] HoleT&... holes) noexcept(is_nothrow) {
            if constexpr (UTF32<unit_type>) {
                *iter() = inp_code_point;
            } else {
                assert(iter() != reducer->endptr);

                // handling invalid code points
                if (static_cast<stl::int32_t>(inp_code_point) < 0) [[unlikely]] {
                    *iter() = -static_cast<unit_type>(inp_code_point);
                    return;
                }

                auto const cur_len =
                  checked::code_point_length<iterator, stl::int_fast8_t>(iter(), reducer->end());
                auto const new_len  = utf_length_from_utf32<unit_type, stl::int_fast8_t>(inp_code_point);
                auto const old_diff = cur_len - new_len;

                // if new length is 0, a bad code point is given to the input.
                assert(new_len != 0);

                // Move the hole to the current place in order to make cur_len bigger than the new_len
                if (old_diff < 0 && this->reducer->empty_size() < static_cast<size_type>(-old_diff)) {
                    assert(reducer->beg <= hole.begin());
                    assert(reducer->endptr >= hole.end());
                    auto const cur_end = iter() + cur_len;
                    assert(hole.end() >= cur_end);
                    auto       iter_cpy      = istl::deref(iter());
                    auto const hole_distance = cur_end - hole.begin();
                    hole.split_move(static_cast<size_type>(-old_diff), hole_distance, reducer->iters);
                    auto const changed_length = unchecked::append(iter_cpy, inp_code_point);
                    assert(static_cast<stl::int_fast8_t>(changed_length) == new_len);
                    hole.shave(iter(), static_cast<size_type>(new_len));

                    // Moving the iterators if they've been replaced
                    auto const new_end = iter() + new_len;
                    for (auto& cur : reducer->iters) {
                        if (cur > iter() && cur < new_end) {
                            cur = iter();
                        }
                    }

                    // reducer->snap_hole_to_end(hole);
                    hole.sequence_fill();
                    test_state_correctness();
                } else {
                    set_inplace(inp_code_point, cur_len, new_len);
                    if (hole.begin() > iter()) {
                        hole.move_mark(static_cast<difference_type>(-old_diff));
                    }
                    (static_cast<void>(holes.begin() > iter() &&
                                       (holes.move_mark(static_cast<difference_type>(-old_diff)), true)),
                     ...);
                }
            }
        }

        /// Replace the LHS hole with RHS hole if it is in the current code point range.
        constexpr void fallback_hole(utf_range_marker<iterator>& lhs, utf_range_marker<iterator>& rhs)
          noexcept(is_nothrow) {
            if constexpr (!UTF32<unit_type>) {
                auto const cur_len = checked::code_point_length<iterator, size_type>(iter(), reducer->end());
                if (lhs.has_overlaps(iter(), cur_len)) {
                    lhs.mark(stl::move(rhs));
                }
            }
        }

        /// Pin Act: Set
        template <stl::size_t PinIndex2, typename Iter2>
        constexpr void set(pin_type<PinIndex2, PinCount, Iter2, CodePointT> const& other_ptr)
          noexcept(is_nothrow) {
            set(other_ptr.iter());
        }
    };

    /**
     * Reduce a UTF-8/16/32-encoded inplace.
     *
     * Usage cases:
     *   - If you have a range and want to replace something in it, and you're sure you're not going to change
     *     the length of the string.
     *   - It gives you multiple pins (forward-only-iterators of some sort) to modify inplace
     *
     * Rules:
     *   1. Pointers/Iterators should not be pointing to the middle of a Unicode code unit.
     *   2. Pins are ordered, meaning pin-1 cannot point to location after the pin-2's location.
     *   3. Pins can point to the same place though
     *   4. This class is not thread-safe, though you probably don't need that anyway.
     *
     * @tparam IterT Iterator type
     * @tparam CodePointT The Code Point Type (Must be UTF-32)
     */
    template <stl::size_t PinCount, typename IterT, UTF32 CodePointT>
    struct utf_reducer {
        using value_type      = CodePointT;
        using iterator        = IterT;
        using unit_type       = stl::iter_value_t<IterT>;
        using difference_type = stl::iter_difference_t<IterT>;
        using size_type       = stl::size_t;

        static constexpr value_type  npos      = stl::numeric_limits<value_type>::max();
        static constexpr stl::size_t pin_count = PinCount;
        static constexpr bool        is_nothrow =
          stl::is_nothrow_copy_assignable_v<unit_type> && requires(iterator iter, unit_type unit) {
              {
                  *iter = unit
              } noexcept;
          };

        static_assert(pin_count != 0, "This class is useless where you don't need the pins.");
        static_assert(stl::random_access_iterator<iterator>,
                      "Iterator must at least be a random access iterator");

      private:
        template <std::size_t, std::size_t, typename, UTF32>
        friend struct pin_type;

        template <typename T>
        using if_not_utf32 = stl::conditional_t<UTF32<unit_type>, istl::nothing_type, T>;

        iterator beg;
        iterator endptr;
        iterator newend;

        [[no_unique_address]] if_not_utf32<stl::array<iterator, PinCount>> iters;

        template <stl::size_t Index>
        using pin_type_of = pin_type<Index, PinCount, IterT, CodePointT>;

      public:
        // the reason why we take length instead of end position is because the end might be a const, but
        // we don't want the end to be a const internally.
        explicit constexpr utf_reducer(iterator inp_pos, size_type const inp_length)
          : beg{inp_pos},
            endptr{stl::next(inp_pos, static_cast<difference_type>(inp_length))},
            newend{endptr} {
            if constexpr (std::is_pointer_v<iterator>) {
                assert(inp_pos != nullptr);
                assert(endptr != nullptr);
            }
            assert(inp_pos <= endptr);
            // assert(is_code_unit_start(*inp_pos));
            if constexpr (!UTF32<unit_type>) {
                iters.fill(beg);
            }
        }

        // NOLINTNEXTLINE(*-easily-swappable-parameters)
        explicit constexpr utf_reducer(iterator inp_pos, iterator inp_endp)
          : beg{inp_pos},
            endptr{inp_endp},
            newend{endptr} {
            if constexpr (std::is_pointer_v<iterator>) {
                assert(inp_pos != nullptr);
                assert(endptr != nullptr);
            }
            assert(inp_pos <= inp_endp);
            // assert(is_code_unit_start(*inp_pos));
            if constexpr (!UTF32<unit_type>) {
                iters.fill(beg);
            }
        }

        constexpr utf_reducer(utf_reducer const&)                = default;
        constexpr utf_reducer(utf_reducer&&) noexcept            = default;
        constexpr utf_reducer& operator=(utf_reducer const&)     = default;
        constexpr utf_reducer& operator=(utf_reducer&&) noexcept = default;
        constexpr ~utf_reducer() noexcept                        = default;

        template <stl::size_t Index = 0>
        [[nodiscard]] constexpr auto pin() noexcept {
            static_assert(Index < PinCount, "Index must be in range.");
            return pin_type_of<Index>{this};
        }

        [[nodiscard]] constexpr auto& all_pins() noexcept {
            return iters;
        }

        template <difference_type Index = 0>
        [[nodiscard]] constexpr iterator& pin_iter() noexcept
            requires(!UTF32<unit_type>)
        {
            // Create a compile time error and not allow bad code:
            static_assert(Index <= static_cast<difference_type>(PinCount) && Index >= -1,
                          "Index must be in range, or the last element.");
            if constexpr (Index == -1) {
                return beg;
            } else if constexpr (Index == static_cast<difference_type>(PinCount)) {
                return endptr;
            } else {
                return iters[Index];
            }
        }

        [[nodiscard]] constexpr iterator& pin_iter(difference_type index = 0) noexcept
            requires(!UTF32<unit_type>)
        {
            assert(index <= static_cast<difference_type>(PinCount) && index >= -1);
            if (index == -1) {
                return beg;
            }
            if (index == static_cast<difference_type>(PinCount)) {
                return endptr;
            }
            return iters[static_cast<size_type>(index)];
        }

        /// Get all pins in a tuple construct
        /// Usage:
        ///   auto [pin1, pin2, pin3] = reducer.pins();
        [[nodiscard]] constexpr auto pins() noexcept {
            return ([&]<stl::size_t... I>(stl::index_sequence<I...>) {
                return stl::make_tuple(pin<I>()...);
            })(stl::make_index_sequence<PinCount>{});
        }

        [[nodiscard]] constexpr iterator begin() const noexcept {
            return beg;
        }

        [[nodiscard]] constexpr iterator end() const noexcept {
            return newend;
        }

        constexpr void snap_hole_to_end(utf_range_marker<iterator>& hole) noexcept(is_nothrow) {
            if constexpr (!UTF32<unit_type>) {
                if (!hole.empty()) {
                    auto const distance_till_hold_end = this->newend - hole.end();
                    assert(hole.end() + distance_till_hold_end <= this->endptr);
                    hole.move(distance_till_hold_end, this->iters);
                    this->newend  = hole.begin();
                    *this->newend = static_cast<unit_type>('\0');
                    if (hole.has_overlaps(this->newend, this->endptr)) {
                        hole.clear();
                    }
                    for (auto& cur : iters) {
                        if (cur >= newend) {
                            while (!is_code_unit_start(*--cur)) {
                                // go to the start of the latest code point
                            }
                        }
                    }
                }
            }
        }

        constexpr void set_end(iterator inp_end) noexcept {
            newend = inp_end;
        }

        template <stl::size_t Index>
        constexpr void set_end(pin_type_of<Index> const& inp_end) noexcept {
            newend = inp_end.iter();
        }

        [[nodiscard]] constexpr size_type size() const noexcept {
            return static_cast<size_type>(newend - beg);
        }

        /// Get the length of the empty hole at the end of the reducer
        [[nodiscard]] constexpr size_type empty_size() const noexcept {
            return static_cast<size_type>(endptr - newend);
        }
    };

    template <std::size_t PinIndex1,
              std::size_t PinIndex2,
              std::size_t PinCount,
              typename IterT,
              typename CodePointT>
    [[nodiscard]] static constexpr auto operator-(pin_type<PinIndex1, PinCount, IterT, CodePointT> const& lhs,
                                                  pin_type<PinIndex2, PinCount, IterT, CodePointT> const& rhs)
      noexcept(noexcept(lhs.iter() - rhs.iter())) {
        using difference_type = typename pin_type<PinIndex1, PinCount, IterT, CodePointT>::difference_type;
        return static_cast<difference_type>(lhs.iter() - rhs.iter());
    }

    template <std::size_t PinIndex,
              std::size_t PinCount,
              typename IterT,
              typename CodePointT,
              typename RIterT>
    [[nodiscard]] static constexpr auto operator-(pin_type<PinIndex, PinCount, IterT, CodePointT> const& lhs,
                                                  RIterT rhs_iter) noexcept(noexcept(lhs.iter() - rhs_iter)) {
        using difference_type = typename pin_type<PinIndex, PinCount, IterT, CodePointT>::difference_type;
        return static_cast<difference_type>(lhs.iter() - rhs_iter);
    }

    template <typename LIterT,
              std::size_t PinIndex,
              std::size_t PinCount,
              typename IterT,
              typename CodePointT>
    [[nodiscard]] static constexpr auto operator-(
      LIterT                                                 lhs_iter,
      pin_type<PinIndex, PinCount, IterT, CodePointT> const& rhs) noexcept(noexcept(lhs_iter - rhs.iter())) {
        using difference_type = typename pin_type<PinIndex, PinCount, IterT, CodePointT>::difference_type;
        return static_cast<difference_type>(lhs_iter - rhs.iter());
    }

} // namespace webpp::unicode

#endif // WEBPP_UNICODE_CODE_POINT_ITERATOR_HPP
