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
        using iterator_type   = typename reducer_type::iterator_type;
        using size_type       = typename reducer_type::size_type;
        using value_type      = typename reducer_type::value_type;
        using unit_type       = typename reducer_type::unit_type;
        using difference_type = typename reducer_type::difference_type;

        static constexpr bool is_nothrow = reducer_type::is_nothrow;

      private:
        reducer_type* reducer;

        /// guarantee the correctness of the state that we're in
        constexpr void test_state_correctness() noexcept {
            if constexpr (PinIndex != PinCount - 1) {
                // guarantee that each pin's position will be less than or equal to the next one:
                assert(reducer->begs[PinIndex + 1] >= beg());
            }
            if constexpr (PinIndex != 0) {
                // guarantee that each pin's position will be more than or equal to the previous one:
                assert(reducer->begs[PinIndex - 1] >= beg());
            }

            // early blow up in case we did not find the correct ptr position:
            assert(is_code_unit_start(*beg()));
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
        constexpr pin_type& operator++() noexcept {
            if constexpr (UTF32<unit_type>) {
                ++iter();
            } else if constexpr (PinIndex == PinCount - 1) {
                reducer->code_points[PinIndex] = next_code_point(iter());
            } else {
                auto const cur_state = state();
                if (cur_state == 0) [[likely]] {
                    // state: filled
                    reducer->code_points[PinIndex] = next_code_point(iter());
                } else if (cur_state > 0) {
                    // state: extra
                    // todo
                } else {
                    // state: partial or deleted
                    // copy next code units to current empty code units
                    auto& cur    = iter();
                    auto  cp2ptr = cur + cur_state;
                    auto  endptr = reducer->template pin_iter<PinIndex + 1>();
                    for (; cp2ptr != endptr; ++cur, ++cp2ptr) {
                        *cur = *cp2ptr;
                    }
                }
            }
            test_state_correctness();
            return *this;
        }

        constexpr pin_type operator++(int) noexcept {
            return pin_type{reducer}.operator++();
        }

        [[nodiscard]] constexpr iterator_type& iter() noexcept {
            return reducer->iters[PinIndex];
        }

        [[nodiscard]] constexpr auto state() noexcept {
            if constexpr (UTF8<unit_type>) {
                assert(reducer->states[PinIndex] <= 4);
            } else if constexpr (UTF16<unit_type>) {
                assert(reducer->states[PinIndex] <= 2);
            }
            return reducer->states[PinIndex];
        }

        [[nodiscard]] constexpr iterator_type operator->() const noexcept {
            return iter();
        }

        [[nodiscard]] constexpr bool operator==(iterator_type other) const noexcept {
            return iter() == other;
        }

        [[nodiscard]] constexpr bool operator!=(iterator_type other) const noexcept {
            return iter() != other;
        }

        [[nodiscard]] constexpr stl::strong_ordering operator<=>(iterator_type other) const noexcept {
            // this might have unintentional consequences if other is in the middle of a UTF-8 code point
            // and the ptr is in the beginning of that code point. we're assuming pointers can't be
            // pointed to the middle of the Unicode code points.
            return iter() <=> other;
        }

        [[nodiscard]] constexpr stl::strong_ordering operator<=>(pin_type const& other) const noexcept {
            return iter() <=> other.iter();
        }

        constexpr pin_type& operator=(iterator_type other) noexcept(is_nothrow) {
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
        constexpr void set(iterator_type other) noexcept(is_nothrow) {
            assert(other != nullptr);
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
                    unchecked::append(iter_cpy, other);
                    reducer->states[PinIndex] = state;
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
                auto const state = state_cmp(inp_code_point);

                // state: filled
                if (state == 0) [[likely]] {
                    auto iter_cpy = istl::deref(iter());
                    unchecked::append(iter_cpy, inp_code_point);
                    test_state_correctness();
                    return;
                }

                // state: partial or deleted
                if (state < 0) {
                    auto iter_cpy = istl::deref(iter());
                    unchecked::append(iter_cpy, inp_code_point);
                    reducer->states[PinIndex] = state;
                    test_state_correctness();
                    return;
                }

                // state: extra
                {
                    reducer->code_points[PinIndex] = inp_code_point;
                    reducer->states[PinIndex]      = state;
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

        constexpr void reduce() noexcept(is_nothrow) {
            assert(iter() != reducer->endptr);

            auto const cur_state = state();
            if (cur_state == 0) [[likely]] {
                return;
            }
            if (cur_state < 0) {
                // fill the gaps
                auto& endptr = reducer->template pin_iter<PinIndex + 1>();
                auto  ptr    = iter();
                stl::advance(ptr, -static_cast<difference_type>(cur_state));
                if constexpr (stl::random_access_iterator<iterator_type>) {
                    assert(ptr <= reducer->endptr);
                }
                auto cur = ptr;
                ++ptr;
                for (; ptr != endptr; ++ptr, ++cur) {
                    *cur = *ptr;
                }
                reducer->states[PinIndex] = 0;
                // if constexpr (PinIndex == PinCount - 1) {
                //     // change the end pointer
                //     endptr = ptr;
                // }
                return;
            }

            // state: extra
            {
                // todo
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
        using iterator_type   = IterT;
        using unit_type       = typename stl::iterator_traits<IterT>::value_type;
        using difference_type = typename stl::iterator_traits<IterT>::difference_type;
        using size_type       = stl::size_t;

        static constexpr stl::size_t pin_count = PinCount;
        static constexpr bool        is_nothrow =
          stl::is_nothrow_copy_assignable_v<unit_type> && requires(iterator_type iter, unit_type unit) {
              {
                  *iter = unit
              } noexcept;
          };

        static_assert(pin_count != 0, "This class is useless where you don't need the pins.");
        static_assert(stl::forward_iterator<iterator_type>, "Iterator must at least be a forward iterator");

      private:
        template <std::size_t, std::size_t, typename, UTF32>
        friend struct pin_type;

        using non32_value_type = stl::conditional_t<UTF32<unit_type>, istl::nothing_type, value_type>;
        iterator_type beg;
        iterator_type endptr;

        stl::array<iterator_type, PinCount>    iters;
        stl::array<stl::int_fast8_t, PinCount> states;
        stl::array<value_type, PinCount>       code_points;

        template <stl::size_t Index>
        using pin_type_of = pin_type<Index, PinCount, IterT, CodePointT>;

      public:
        // the reason why we take length instead of end position is because the end might be a const, but
        // we don't want the end to be a const internally.
        explicit constexpr utf_reducer(iterator_type inp_pos, size_type const inp_length)
          : beg{inp_pos},
            endptr{stl::next(inp_pos, static_cast<difference_type>(inp_length))} {
            assert(inp_pos != nullptr);
            assert(is_code_unit_start(*inp_pos));
            static_assert(stl::random_access_iterator<iterator_type>,
                          "Has to be random access if you're using length-based constructor");
            iters.fill(inp_pos);
        }

        // NOLINTNEXTLINE(*-easily-swappable-parameters)
        explicit constexpr utf_reducer(iterator_type inp_pos, iterator_type inp_endp)
          : beg{inp_pos},
            endptr{inp_endp} {
            assert(inp_pos != nullptr);
            assert(endptr != nullptr);
            assert(is_code_unit_start(*inp_pos));
            if constexpr (stl::random_access_iterator<iterator_type>) {
                assert(inp_pos <= nullptr);
            }
            iters.fill(inp_pos);
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
            for (auto const state : states) {
                assert(state == 0);
            }
        }
#endif

        template <stl::size_t Index = 0>
        [[nodiscard]] constexpr pin_type_of<Index> pin() noexcept {
            static_assert(Index < PinCount, "Index must be in range.");
            return pin_type_of<Index>{this};
        }

        template <stl::size_t Index = 0>
        [[nodiscard]] constexpr iterator_type& pin_iter() noexcept {
            // Create a compile time error and not allow bad code:
            static_assert(Index <= PinCount, "Index must be in range, or the last element.");
            if constexpr (Index == PinCount) {
                return endptr;
            } else {
                return iters[Index];
            }
        }

        /// Get all pins in a tuple construct
        /// Usage:
        ///   auto [pin1, pin2, pin3] = reducer.pins();
        [[nodiscard]] constexpr auto pins() noexcept {
            return ([&]<stl::size_t... I>(stl::index_sequence<I...>) {
                return stl::make_tuple(pin<I>()...);
            })(stl::make_index_sequence<PinCount>{});
        }

        [[nodiscard]] constexpr iterator_type begin() const noexcept {
            return beg;
        }

        [[nodiscard]] constexpr iterator_type end() const noexcept {
            return endptr;
        }

        template <stl::size_t Index = 0>
        constexpr void reduce() noexcept(is_nothrow) {
            pin<Index>().reduce();
            if constexpr (Index != PinCount - 1) {
                reduce<Index + 1>();
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
      utf_reducer<PinCount, CharT1, CP1> const&                  lhs,
      typename utf_reducer<PinCount, CharT1, CP1>::iterator_type rhs_iter)
      -> decltype(lhs.begin() - rhs_iter) {
        return lhs.begin() - rhs_iter;
    }

} // namespace webpp::unicode

#endif // WEBPP_UNICODE_CODE_POINT_ITERATOR_HPP
