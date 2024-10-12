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

    // enum struct pin_states {
    //     filled,  // [X|'|']      All code units are filled
    //     extra,   // [X|'|'] [']  There are extra code units left that can't be fit
    //     partial, // [X|'| ]      We've only used parts of the previous code units' space available
    //     // deleted  // [ | | ]      All code units are empty now
    // };

    /**
     * Pin is a forward-iterator-like type that is designed to be used inside utf_reducer.
     *
     * Pin Acts (capabilities of pins):
     *   - Get: get the code point
     *   - Set (or Replace): replace a code point inplace if possible
     *   - Change Pin position to another pin location
     *   - Forward: go to the next code point
     */
    template <std::size_t PinIndex = 0,
              std::size_t PinCount = 1,
              typename IterT       = char8_t*,
              UTF32 CodePointT     = char32_t>
    struct pin_type {
        static_assert(PinIndex < PinCount, "Either Pin count is invalid or Pin index.");

        using reducer_type  = utf_reducer<PinCount, IterT, CodePointT>;
        using iterator_type = typename reducer_type::iterator_type;

      private:
        reducer_type* reducer;

        /// guarantee the correctness of the state that we're in
        void test() noexcept {
            if constexpr (PinIndex != PinCount - 1) {
                // guarantee that each pin's position will be less than or equal to the next one:
                assert(reducer->ptrs[PinIndex + 1] >= ptr());
            }
            if constexpr (PinIndex != 0) {
                // guarantee that each pin's position will be more than or equal to the previous one:
                assert(reducer->ptrs[PinIndex - 1] >= ptr());
            }

            // early blow up in case we did not find the correct ptr position:
            assert(is_code_unit_start(*ptr()));
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
            auto const state = reducer->states[PinIndex];
            if (state == 0) [[likely]] {
                // state: filled
                /* code_point = */ next_code_point(beg());
            } else if (state > 0) {
                // state: extra
            } else {
                // state: partial or deleted
                // copy next code units to current empty code units
                auto& cur    = beg();
                auto  cp2ptr = cur + state;
                auto  endptr = reducer->template pin_iter<PinIndex + 1>();
                for (; cp2ptr != endptr; ++cur, ++cp2ptr) {
                    *cur = *cp2ptr;
                }
            }
            test();
            return *this;
        }

        /// Pin Act: Set
        constexpr pin_type& operator=(CodePointT const code_point) noexcept {
            // todo
            return *this;
        }

        [[nodiscard]] constexpr iterator_type& beg() noexcept {
            return reducer->iters[PinIndex];
        }

        [[nodiscard]] constexpr iterator_type operator->() const noexcept {
            return beg();
        }

        [[nodiscard]] constexpr bool operator==(iterator_type other) const noexcept {
            return beg() == other;
        }

        [[nodiscard]] constexpr bool operator!=(iterator_type other) const noexcept {
            return beg() != other;
        }

        [[nodiscard]] constexpr stl::strong_ordering operator<=>(iterator_type other) const noexcept {
            // this might have unintentional consequences if other is in the middle of a UTF-8 code point
            // and the ptr is in the beginning of that code point. we're assuming pointers can't be
            // pointed to the middle of the Unicode code points.
            return beg() <=> other;
        }

        [[nodiscard]] constexpr stl::strong_ordering operator<=>(pin_type const& other) const noexcept {
            return beg() <=> other.beg();
        }

        constexpr pin_type operator++(int) noexcept {
            return pin_type{*this}.operator++();
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

        static constexpr stl::size_t pin_count = PinCount;

        static_assert(pin_count != 0, "This class is useless where you don't need the pins.");

      private:
        template <std::size_t, std::size_t, typename, UTF32>
        friend struct pin_type;

        using non32_value_type = stl::conditional_t<UTF32<IterT>, istl::nothing_type, value_type>;
        iterator_type                          beg;
        iterator_type                          endptr;
        [[no_unique_address]] non32_value_type code_point;

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
            if constexpr (!UTF32<IterT>) {
                code_point = next_code_point_copy(beg);
            }
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

        // we intentionally don't allow dereferencing because the user of this class may do something like
        // *iter = new_value;
        // [[nodiscard]] constexpr reference operator*() noexcept
        // {
        //     return code_point;
        // }

        constexpr stl::size_t set_code_point(value_type inp_code_point, size_type length)
          noexcept(std::is_nothrow_copy_assignable_v<value_type>) {
            if constexpr (UTF32<IterT>) {
                *beg = inp_code_point;
                return 1U;
            } else {
                auto const orig_len = required_length_of(*beg);
                auto const rep_len  = utf8_length_from_utf32(inp_code_point);
                assert(rep_len <= orig_len); // It'll replace content, most-likely not what you need.
                assert(rep_len <= length);   // not enough room
                if (rep_len < orig_len) {
                    stl::rotate(beg + rep_len, beg + orig_len, beg + length);
                }
                set_code_point(inp_code_point);
                return rep_len;
            }
        }

        constexpr stl::size_t set_code_point(iterator_type other_ptr, size_type length)
          noexcept(std::is_nothrow_copy_assignable_v<value_type>) {
            if constexpr (UTF32<IterT>) {
                *beg = *other_ptr;
                return 1U;
            } else {
                auto const orig_len = required_length_of(*beg);
                auto const rep_len  = required_length_of(*other_ptr);
                assert(rep_len <= orig_len); // It'll replace content, most-likely not what you need.
                assert(rep_len <= length);   // not enough room
                if (rep_len < orig_len) {
                    stl::rotate(beg + rep_len, beg + orig_len, beg + length);
                }
                set_code_point(other_ptr);
                return rep_len;
            }
        }

        constexpr stl::size_t set_code_point(iterator_type other_ptr)
          noexcept(std::is_nothrow_copy_assignable_v<value_type>) {
            if constexpr (UTF32<IterT>) {
                *beg = *other_ptr;
                return 1U;
            } else {
                auto       ptr_cpy     = beg;
                auto const changed_len = unchecked::append(ptr_cpy, other_ptr);
                code_point             = next_code_point_copy(beg);
                return changed_len;
            }
        }

        constexpr stl::size_t set_code_point(value_type inp_code_point)
          noexcept(std::is_nothrow_copy_assignable_v<value_type>) {
            if constexpr (UTF32<IterT>) {
                *beg = inp_code_point;
                return 1U;
            } else {
                auto ptr_cpy = beg;
                code_point   = inp_code_point;
                return unchecked::append(ptr_cpy, inp_code_point);
            }
        }

        template <typename CharT2 = IterT>
        constexpr stl::size_t set_code_point(
          utf_reducer<PinCount, CharT2, CodePointT> const& other_ptr,
          iterator_type end) noexcept(std::is_nothrow_copy_assignable_v<value_type>) {
            return set_code_point(other_ptr.base(), end - beg);
        }

        template <typename CharT2 = IterT>
        constexpr stl::size_t set_code_point(utf_reducer<PinCount, CharT2, CodePointT> const& other_ptr)
          noexcept(std::is_nothrow_copy_assignable_v<value_type>) {
            return set_code_point(other_ptr.base());
        }

        template <typename CharT2 = IterT>
        constexpr stl::size_t set_code_point(value_type                                       other,
                                             utf_reducer<PinCount, CharT2, CodePointT> const& end)
          noexcept(std::is_nothrow_copy_assignable_v<value_type>) {
            return set_code_point(other, end.base() - beg);
        }

        template <typename CharT2 = stl::add_const_t<IterT>>
        constexpr stl::size_t set_code_point(iterator_type                                    other_ptr,
                                             utf_reducer<PinCount, CharT2, CodePointT> const& end)
          noexcept(std::is_nothrow_copy_assignable_v<value_type>) {
            return set_code_point(other_ptr.base(), end.base() - beg);
        }

        template <typename CharT2 = stl::add_const_t<IterT>>
        constexpr stl::size_t set_code_point(utf_reducer<PinCount, CharT2, CodePointT> const& other_ptr,
                                             utf_reducer<PinCount, CharT2, CodePointT> const& end)
          noexcept(std::is_nothrow_copy_assignable_v<value_type>) {
            return set_code_point(other_ptr.base(), end.base() - beg);
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
      typename utf_reducer<PinCount, CharT1, CP1>::const_pointer rhs_ptr) -> decltype(lhs.base() - rhs_ptr) {
        return lhs.base() - rhs_ptr;
    }

    // template <stl::size_t PinCount, typename PtrT>
    // utf_reducer(PtrT) -> utf_reducer<PinCount, stl::remove_pointer_t<stl::remove_cvref_t<PtrT>>>;

    // template <typename PtrT>
    // code_point_iterator(PtrT const&)
    //   -> code_point_iterator<
    //     stl::add_const_t<stl::remove_cvref_t<stl::remove_pointer_t<stl::remove_cvref_t<PtrT>>>>>;

    /// wrap it if it's UTF-8 or UTF-16, but if it's UTF-32, don't wrap:
    template <stl::size_t PinCount = 1, typename CharT = char8_t, UTF32 CodePointT = char32_t>
    using utf_reducer_t = stl::conditional_t<UTF32<CharT>, CharT*, utf_reducer<PinCount, CharT, CodePointT>>;

} // namespace webpp::unicode

#endif // WEBPP_UNICODE_CODE_POINT_ITERATOR_HPP
