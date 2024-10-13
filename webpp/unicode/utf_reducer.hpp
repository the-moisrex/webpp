// Created by moisrex on 9/2/24.

#ifndef WEBPP_UNICODE_CODE_POINT_ITERATOR_HPP
#define WEBPP_UNICODE_CODE_POINT_ITERATOR_HPP

#include "../std/type_traits.hpp"
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

        using reducer_type  = utf_reducer<PinCount, IterT, CodePointT>;
        using iterator_type = typename reducer_type::iterator_type;
        using size_type     = typename reducer_type::size_type;
        using value_type    = typename reducer_type::value_type;
        using unit_type     = typename reducer_type::unit_type;

        static constexpr bool is_nothrow = stl::is_nothrow_copy_assignable_v<unit_type>;

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

        /// Return type is like the return type of strcmp
        ///   - 0 if both are the same length (Filled)
        ///   - positive value if the new value requires more space (Extra)
        ///   - negative value if the new value requires less space (Partial)
        [[nodiscard]] constexpr stl::int_fast8_t state_cmp(value_type const inp_cp) noexcept {
            if constexpr (UTF32<unit_type>) {
                return 0; // state: filled
            } else {
                return utf_length_from_utf32<unit_type, stl::int_fast8_t>(inp_cp) -
                       required_length_of<unit_type, stl::int_fast8_t>(*iter());
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
                /* code_point = */ next_code_point(iter());
            } else {
                auto const state = state();
                if (state == 0) [[likely]] {
                    // state: filled
                    /* code_point = */ next_code_point(iter());
                } else if (state > 0) {
                    // state: extra
                } else {
                    // state: partial or deleted
                    // copy next code units to current empty code units
                    auto& cur    = iter();
                    auto  cp2ptr = cur + state;
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
        constexpr stl::size_t set(iterator_type other) noexcept(is_nothrow) {
            if constexpr (UTF32<unit_type>) {
                *iter = *other;
                return 1U;
            } else {
                auto       ptr_cpy     = iter;
                auto const changed_len = unchecked::append(ptr_cpy, other);
                /* code_point             = */ next_code_point_copy(iter);
                return changed_len;
            }
        }

        /// Pin Act: Set
        constexpr stl::size_t set(value_type inp_code_point) noexcept(is_nothrow) {
            if constexpr (UTF32<unit_type>) {
                *iter = inp_code_point;
                return 1U;
            } else {
                auto const state = state_cmp(inp_code_point);
                if (state == 0) [[likely]] {
                    // state: filled
                    // todo
                } else if (state > 0) {
                    // state: extra
                } else {
                    // state: partial or deleted
                }
            }
            test_state_correctness();
            return 0;
        }

        /// Pin Act: Set
        template <stl::size_t PinIndex2, typename Iter2>
        constexpr stl::size_t set(pin_type<PinIndex2, PinCount, Iter2, CodePointT> const& other_ptr)
          noexcept(is_nothrow) {
            return set(other_ptr.iter());
        }
    };

    /**
     * An iterator-like class for Unicode Code Points
     * We're assuming pointers can't be pointed to the middle of the Unicode code points.
     *
     * @tparam IterT Iterator type
     * @tparam CodePointT The Code Point Type (Must be UTF-32)
     */
    template <stl::size_t PinCount, typename IterT, UTF32 CodePointT>
    struct utf_reducer {
        using value_type    = CodePointT;
        using iterator_type = IterT;
        using size_type     = stl::size_t;
        using unit_type     = typename stl::iterator_traits<IterT>::value_type;

        static constexpr stl::size_t pin_count = PinCount;

        static_assert(pin_count != 0, "This class is useless where you don't need the pins.");

      private:
        template <std::size_t, std::size_t, typename, UTF32>
        friend struct pin_type;

        using non32_value_type = stl::conditional_t<UTF32<unit_type>, istl::nothing_type, value_type>;
        iterator_type beg;
        iterator_type endptr;

        stl::array<iterator_type, PinCount>    iters;
        stl::array<stl::int_fast8_t, PinCount> states;

        template <stl::size_t Index>
        using pin_type_of = pin_type<Index, PinCount, IterT, CodePointT>;

      public:
        // the reason why we take length instead of end position is because the end might be a const, but
        // we don't want the end to be a const internally.
        explicit constexpr utf_reducer(iterator_type inp_pos, size_type const inp_length)
          : beg{inp_pos},
            endptr{inp_pos + inp_length} {
            assert(inp_pos != nullptr);
            assert(is_code_unit_start(*inp_pos));
        }

        constexpr utf_reducer(utf_reducer const&)                = default;
        constexpr utf_reducer(utf_reducer&&) noexcept            = default;
        constexpr utf_reducer& operator=(utf_reducer const&)     = default;
        constexpr utf_reducer& operator=(utf_reducer&&) noexcept = default;
        constexpr ~utf_reducer() noexcept                        = default;

        template <stl::size_t Index = 0>
        [[nodiscard]] constexpr pin_type_of<Index> pin() noexcept {
            static_assert(Index < PinCount, "Index must be in range.");
            return pin_type_of<Index>{this};
        }

        template <stl::size_t Index = 0>
        [[nodiscard]] constexpr iterator_type pin_iter() noexcept {
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

        [[nodiscard]] constexpr iterator_type base() const noexcept {
            return beg;
        }

        constexpr void reduce() noexcept {
            // todo
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
      utf_reducer<PinCount2, CharT2, CP2> const& rhs) -> decltype(lhs.base() - rhs.base()) {
        return lhs.base() - rhs.base();
    }

    template <stl::size_t PinCount, typename CharT1, typename CP1>
    [[nodiscard]] static constexpr auto operator-(
      utf_reducer<PinCount, CharT1, CP1> const&                  lhs,
      typename utf_reducer<PinCount, CharT1, CP1>::iterator_type rhs_iter)
      -> decltype(lhs.base() - rhs_iter) {
        return lhs.base() - rhs_iter;
    }

} // namespace webpp::unicode

#endif // WEBPP_UNICODE_CODE_POINT_ITERATOR_HPP
