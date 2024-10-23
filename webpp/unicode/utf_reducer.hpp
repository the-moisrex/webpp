// Created by moisrex on 9/2/24.

#ifndef WEBPP_UNICODE_CODE_POINT_ITERATOR_HPP
#define WEBPP_UNICODE_CODE_POINT_ITERATOR_HPP

#include "../std/type_traits.hpp"
#include "../std/utility.hpp"
#include "./unicode.hpp"

#include <array>
#include <cassert>

namespace webpp::unicode {

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

      private:
        reducer_type* reducer;

        /// guarantee the correctness of the state that we're in
        constexpr void test_state_correctness() noexcept {
            // guarantee that each pin's position will be less than or equal to the next one:
            assert(reducer->template pin_iter<static_cast<difference_type>(PinIndex) + 1>() >= iter());

            // guarantee that each pin's position will be more than or equal to the previous one:
            assert(reducer->template pin_iter<static_cast<difference_type>(PinIndex) - 1>() <= iter());

            // early blow up in case we did not find the correct ptr position:
            assert(is_code_unit_start(*iter()));
        }

        [[nodiscard]] constexpr stl::int_fast8_t state_cmp_size(stl::int_fast8_t const len) noexcept {
            if constexpr (UTF32<unit_type>) {
                return 0; // state: filled
            } else {
                return len - required_length_of<unit_type, stl::int_fast8_t>(*iter());
            }
        }

        /// Return type is like the return type of strcmp
        ///   - 0 if both are the same length (Filled)
        ///   - positive value if the new value requires more space (Extra)
        ///   - negative value if the new value requires less space (Partial)
        [[nodiscard]] constexpr stl::int_fast8_t state_cmp(value_type const inp_cp) noexcept {
            if constexpr (UTF32<unit_type>) {
                return 0; // state: filled
            } else {
                return state_cmp_size(utf_length_from_utf32<unit_type, stl::int_fast8_t>(inp_cp));
            }
        }

        /// The number of code units required to set this code new code point
        [[nodiscard]] constexpr stl::int_fast8_t required_code_units_of_len(stl::int_fast8_t const cp_len)
          noexcept(is_nothrow) {
            if constexpr (UTF32<unit_type>) {
                return 1; // always
            } else {
                stl::int_fast8_t count    = 0;
                auto             iter_cpy = istl::deref(iter());
                while (count <= cp_len) {
                    auto const cur_len  = required_length_of<unit_type, stl::int_fast8_t>(*iter_cpy);
                    iter_cpy           += cur_len;
                    count              += cur_len;
                    assert(iter_cpy <= reducer->endptr);
                }
                webpp_assume(count <= 6);
                return count;
            }
        }

        /// The number of code units required to set this code new code point
        [[nodiscard]] constexpr stl::int_fast8_t required_code_units(value_type const inp_cp)
          noexcept(is_nothrow) {
            if constexpr (UTF32<unit_type>) {
                return 1; // always
            } else {
                return required_code_units_of_len(utf_length_from_utf32<unit_type, stl::int_fast8_t>(inp_cp));
            }
        }

      public:
        constexpr explicit pin_type(reducer_type* inp_reducer) noexcept : reducer(inp_reducer) {}

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
            static_assert(OPinIndex > PinCount,
                          "You cannot go back; these are forward-only set of constructs.");

            if (this != &other) {
                // this is not a copy assignment operator, it's a pin act.
                assert(reducer == other.reducer);
                for (stl::size_t index = PinIndex; index != OPinIndex; ++index) {
                    operator++();
                }
            }
            return *this;
        }

        /// Pin Act: Forward
        constexpr pin_type& operator++() noexcept(is_nothrow) {
            if constexpr (UTF32<unit_type>) {
                ++iter();
            } else {
                auto const cur_state = state();
                if (cur_state < 0) {
                    goto_next_code_point();
                } else if (cur_state > 0) {
                    // state: extra
                    fill_right();
                    unchecked::append(iter(), reducer->code_points[PinIndex]);
                    reducer->states[PinIndex] = 0;
                }
                reducer->code_points[PinIndex] = next_code_point(iter());
            }
            test_state_correctness();
            return *this;
        }

        [[nodiscard]] constexpr iterator& iter() noexcept {
            return reducer->iters[PinIndex];
        }

        [[nodiscard]] constexpr auto state() noexcept {
            // if constexpr (UTF8<unit_type>) {
            //     assert(reducer->states[PinIndex] <= 4);
            // } else if constexpr (UTF16<unit_type>) {
            //     assert(reducer->states[PinIndex] <= 2);
            // }
            return reducer->states[PinIndex];
        }

        [[nodiscard]] constexpr value_type operator*() const noexcept {
            if constexpr (UTF32<unit_type>) {
                return *iter();
            } else {
                return reducer->code_points[PinIndex];
            }
        }

        [[nodiscard]] constexpr iterator operator->() const noexcept {
            return iter();
        }

        [[nodiscard]] constexpr bool operator==(iterator other) const noexcept {
            return iter() == other;
        }

        [[nodiscard]] constexpr bool operator!=(iterator other) const noexcept {
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
            set(other);
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

        /// Pin Act: Set
        constexpr void set(iterator other) noexcept(is_nothrow) {
            if constexpr (stl::is_pointer_v<iterator>) {
                assert(other != nullptr);
            } else {
                assert(other != reducer->endptr);
            }
            assert(iter() != reducer->endptr);
            if constexpr (UTF32<unit_type>) {
                *iter() = *other;
            } else {
                auto const state = state_cmp_size(required_length_of<unit_type, stl::int_fast8_t>(*other));

                // state: filled
                if (state == 0) [[likely]] {
                    auto iter_cpy = istl::deref(iter());
                    unchecked::append(iter_cpy, other);
                    test_state_correctness();
                    return;
                }

                // state: partial or deleted
                if (state < 0) {
                    auto iter_cpy = istl::deref(iter());
                    auto rem      = required_length_of<unit_type>(*iter_cpy);
                    unchecked::append(iter_cpy, other);
                    reducer->states[PinIndex] = state;

                    // zero out the remaining for use in other algorithms of utf_reducer
                    for (; rem > 0; --rem) {
                        *iter_cpy = static_cast<unit_type>('\0');
                    }
                    test_state_correctness();
                    return;
                }

                // state: extra
                {
                    reducer->code_points[PinIndex] = next_code_point_copy(other);
                    reducer->states[PinIndex]      = state;
                    test_state_correctness();
                }
            }
        }

        /// Pin Act: Set
        constexpr void set(value_type inp_code_point) noexcept(is_nothrow) {
            assert(iter() != reducer->endptr);
            if constexpr (UTF32<unit_type>) {
                *iter() = inp_code_point;
            } else {
                reducer->code_points[PinIndex] = inp_code_point;

                auto const state = state_cmp(inp_code_point);

                // state: filled
                if (state == 0) [[likely]] {
                    auto iter_cpy = istl::deref(iter());
                    unchecked::append(iter_cpy, inp_code_point);
                    reducer->states[PinIndex] = state;
                    test_state_correctness();
                    return;
                }

                // state: partial or deleted
                if (state < 0) {
                    auto iter_cpy = istl::deref(iter());
                    auto rem      = required_length_of<unit_type>(*iter_cpy);
                    unchecked::append(iter_cpy, inp_code_point);
                    reducer->states[PinIndex] = state;

                    // zero out the remaining for use in other algorithms of utf_reducer
                    for (; rem > 0; --rem) {
                        *iter_cpy = static_cast<unit_type>('\0');
                    }
                    test_state_correctness();
                    return;
                }

                // state: extra
                {
                    // reducer->code_points[PinIndex] = inp_code_point;
                    reducer->states[PinIndex] = state;
                    test_state_correctness();
                }
            }
        }

        /// Pin Act: Set
        template <stl::size_t PinIndex2, typename Iter2>
        constexpr void set(pin_type<PinIndex2, PinCount, Iter2, CodePointT> const& other_ptr)
          noexcept(is_nothrow) {
            set(other_ptr.iter());
        }

        /// Pin Act: Set Spillover
        constexpr void spillover_set(value_type inp_code_point) noexcept(is_nothrow) {
            assert(iter() != reducer->endptr);
            if constexpr (UTF32<unit_type>) {
                *iter() = inp_code_point;
            } else {
                reducer->code_points[PinIndex] = inp_code_point;

                auto const cp_len  = utf_length_from_utf32<unit_type, stl::int_fast8_t>(inp_code_point);
                auto const rep_len = required_code_units_of_len(cp_len);
                assert(rep_len >= cp_len);
                stl::int_fast8_t const state    = cp_len - rep_len;
                auto                   iter_cpy = istl::deref(iter());

                reducer->states[PinIndex] = state;
                unchecked::append(iter_cpy, inp_code_point);
                test_state_correctness();
            }
        }


      private:
        constexpr void fill_left() noexcept(is_nothrow) {
            // fill the gaps
            auto ptr = iter();
            stl::advance(ptr, -static_cast<difference_type>(state()));
            assert(ptr <= reducer->endptr);
            auto cur = ptr;
            ++ptr;
            for (; ptr != reducer->endptr; ++ptr, ++cur) {
                *cur = *ptr;
            }
            reducer->states[PinIndex] = 0;
        }

        constexpr void goto_next_code_point() noexcept(is_nothrow) {
            auto ptr = iter();
            stl::advance(ptr, -static_cast<difference_type>(state()));

            // end of the next code point
            auto endptr = stl::next(ptr, required_length_of<unit_type, difference_type>(*ptr));
            assert(ptr <= reducer->endptr);
            auto rep = ptr;
            ++ptr;
            for (; ptr != endptr; ++ptr, ++rep) {
                *rep = *ptr;
            }
        }

        /// find the first pin that from that pin to this pin, there's enough "Partial" spaces that we
        /// can use
        /// We can't return a pin_type since the PinIndex is not known, so we return the index itself
        [[nodiscard]] constexpr auto find_first_pin(difference_type stop_state) const noexcept {
            assert(stop_state > 0);
            for (auto index = PinIndex + 1; index < PinCount; ++index) {
                auto& cur_state  = reducer->states[index];
                stop_state       += static_cast<difference_type>(cur_state);
                if (stop_state <= 0) {
                    return stl::make_pair(index, stop_state);
                }
            }

            // if you reach here, utf_reducer cannot help you anymore, you need allocations for you algorithm,
            // so free launch is over now.
            assert(false);
            stl::unreachable();
        }

        constexpr void fill_right() noexcept(is_nothrow) {
            // 1. find the first pin that from that pin to this pin, there's enough "Partial" spaces that we
            //    can use
            // 2. If there is none, blow up
            // 3. fill the gaps, move the pointers, and set the new states
            // 4. set the new state for the current element
            // 5. place the new code point

            auto [cur_pin_index, remaining_units] = find_first_pin(state());

            webpp_assume(cur_pin_index < PinCount && cur_pin_index > PinIndex);
            auto rep = reducer->iters[cur_pin_index] +
                       (required_length_of<unit_type, difference_type>(*reducer->iters[cur_pin_index]) +
                        remaining_units);
            assert(reducer->states[cur_pin_index] < 0);
            auto prev_cur = rep;
            for (; cur_pin_index != PinIndex; --cur_pin_index) {
                auto& cur_state = reducer->states[cur_pin_index];
                auto  cur       = reducer->end_pin_iter(cur_pin_index);
                auto  endptr    = reducer->pin_iter(cur_pin_index - 1);

                // copy the residuals of the past:
                for (; rep != cur; --rep, --prev_cur) {
                    *prev_cur = *rep;
                }

                --cur;
                assert(rep >= cur);

                // copy the new stuff:
                for (; rep != endptr; --rep, --cur) {
                    *rep = *cur;
                }
                prev_cur = cur;
                if (cur_state < 0) {
                    cur_state                     = 0;
                    reducer->iters[cur_pin_index] = rep;
                }
            }
        }

      public:
        constexpr void reduce() noexcept(is_nothrow) {
            assert(iter() != reducer->endptr);

            auto const cur_state = state();
            if (cur_state == 0) [[likely]] {
                return;
            }
            if (cur_state < 0) {
                fill_left();
                return;
            }

            // state: extra
            {
                fill_right();
                unchecked::append(iter(), reducer->code_points[PinIndex]);
                reducer->states[PinIndex] = 0;
            }
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
        using unit_type       = typename stl::iterator_traits<IterT>::value_type;
        using difference_type = typename stl::iterator_traits<IterT>::difference_type;
        using size_type       = stl::size_t;

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

        [[no_unique_address]] if_not_utf32<stl::array<iterator, PinCount>>         iters;
        [[no_unique_address]] if_not_utf32<stl::array<stl::int_fast8_t, PinCount>> states{};
        [[no_unique_address]] if_not_utf32<stl::array<value_type, PinCount>>       code_points;

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
            assert(inp_pos != endptr);
            assert(is_code_unit_start(*inp_pos));
            if (!UTF32<unit_type>) {
                iters.fill(inp_pos);
                auto const first_cp = next_code_point_copy(inp_pos);
                code_points.fill(first_cp);
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
            assert(is_code_unit_start(*inp_pos));
            assert(inp_pos <= inp_endp);
            if (!UTF32<unit_type>) {
                iters.fill(inp_pos);
                auto const first_cp = next_code_point_copy(inp_pos);
                code_points.fill(first_cp);
            }
        }

        constexpr utf_reducer(utf_reducer const&)                = default;
        constexpr utf_reducer(utf_reducer&&) noexcept            = default;
        constexpr utf_reducer& operator=(utf_reducer const&)     = default;
        constexpr utf_reducer& operator=(utf_reducer&&) noexcept = default;
#ifdef NDEBUG
        constexpr ~utf_reducer() noexcept = default;
#else
        constexpr ~utf_reducer() noexcept {
            // User must call ".reduce()" to apply changes; this checks if they have or not:
            if (!UTF32<unit_type>) {
                for (auto const state : states) {
                    assert(state == 0);
                }
            }
        }
#endif

        template <stl::size_t Index = 0>
        [[nodiscard]] constexpr auto pin() noexcept {
            static_assert(Index < PinCount, "Index must be in range.");
            if constexpr (UTF32<unit_type>) {
                return beg;
            } else {
                return pin_type_of<Index>{this};
            }
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
            return iters[index];
        }

        [[nodiscard]] constexpr iterator end_pin_iter(difference_type index = 0) noexcept
            requires(!UTF32<unit_type>)
        {
            assert(index < static_cast<difference_type>(PinCount) && index >= 0);
            auto ptr = iters[index];
            return ptr + required_length_of<unit_type, difference_type>(*ptr);
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

        [[nodiscard]] constexpr size_type size() const noexcept {
            return newend - beg;
        }

        [[nodiscard]] constexpr difference_type available_space() const noexcept {
            if constexpr (!UTF32<unit_type>) {
                difference_type len = 0;
                for (auto const state : states) {
                    len -= static_cast<difference_type>(state);
                }
                assert(len >= 0);
                return len;
            } else {
                return 0;
            }
        }

        template <stl::size_t Index = 0>
        constexpr void reduce() noexcept(is_nothrow) {
            if constexpr (!UTF32<unit_type>) {
                if constexpr (Index == 0) {
                    // setting the new end iterator:
                    if (auto const diff_len = available_space(); diff_len > 0) {
                        newend = std::prev(endptr, diff_len);
                    }
                }
                pin<Index>().reduce();
                if constexpr (Index != PinCount - 1) {
                    reduce<Index + 1>();
                }
            }
        }
    };

    template <stl::size_t PinCount1,
              stl::size_t PinCount2,
              typename CharT1,
              typename CharT2,
              typename CP1,
              typename CP2>
    [[nodiscard]] static constexpr auto operator-(
      utf_reducer<PinCount1, CharT1, CP1> const& lhs,
      utf_reducer<PinCount2, CharT2, CP2> const& rhs) -> decltype(lhs.begin() - rhs.begin()) {
        return lhs.begin() - rhs.begin();
    }

    template <stl::size_t PinCount, typename CharT1, typename CP1>
    [[nodiscard]] static constexpr auto operator-(
      utf_reducer<PinCount, CharT1, CP1> const&             lhs,
      typename utf_reducer<PinCount, CharT1, CP1>::iterator rhs_iter) -> decltype(lhs.begin() - rhs_iter) {
        return lhs.begin() - rhs_iter;
    }

} // namespace webpp::unicode

#endif // WEBPP_UNICODE_CODE_POINT_ITERATOR_HPP
