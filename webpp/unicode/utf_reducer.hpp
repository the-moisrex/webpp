// Created by moisrex on 9/2/24.

#ifndef WEBPP_UNICODE_CODE_POINT_ITERATOR_HPP
#define WEBPP_UNICODE_CODE_POINT_ITERATOR_HPP

#include "../std/type_traits.hpp"
#include "./unicode.hpp"

#include <array>
#include <cassert>

namespace webpp::unicode {

    template <stl::size_t PinCount = 1, istl::CharType CharT = char8_t, UTF32 CodePointT = char32_t>
    struct utf_reducer;

    namespace details {
        // enum struct pin_states {
        //     filled,  // [X|'|']      All code units are filled
        //     extra,   // [X|'|'] [']  There are extra code units left that can't be fit
        //     partial, // [X|'| ]      We've only used parts of the previous code units' space available
        //     // deleted  // [ | | ]      All code units are empty now
        // };

        /**
         * Pin Acts:
         *   - Get
         *   - Set (or Replace)
         *   - Change Pin position to another pin location
         *   - Forward
         */
        template <std::size_t    PinIndex   = 0,
                  std::size_t    PinCount   = 1,
                  istl::CharType CharT      = char8_t,
                  UTF32          CodePointT = char32_t>
        struct pin_type {
            static_assert(PinIndex < PinCount, "Either Pin count is invalid or Pin index.");

            using reducer_type = utf_reducer<PinCount, CharT, CodePointT>;
            using pointer      = typename reducer_type::pointer;

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
              pin_type<OPinIndex, PinCount, CharT, CodePointT> const& other) noexcept {
                static_assert(OPinIndex > PinCount,
                              "You cannot go back; these are forward-only set of constructs.");

                if (this != &other) {
                    // this is not a copy assignment operator, it's a pin act.
                    assert(reducer != other.reducer);
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
                    /* code_point = */ next_code_point(ptr());
                } else if (state > 0) {
                    // state: extra
                } else {
                    // state: partial or deleted
                    // copy next code units to current empty code units
                }
                test(reducer);
                return *this;
            }

            [[nodiscard]] constexpr pointer& ptr() noexcept {
                return reducer->ptrs[PinIndex];
            }
        };
    } // namespace details

    /**
     * An iterator-like class for Unicode Code Points
     * We're assuming pointers can't be pointed to the middle of the Unicode code points.
     *
     * @tparam CharT Your Character Type (char, char8_t, ...)
     * @tparam CodePointT The Code Point Type (Must be UTF-32)
     */
    template <stl::size_t PinCount, istl::CharType CharT, UTF32 CodePointT>
    struct utf_reducer {
        using value_type        = CodePointT;
        using pointer           = stl::add_pointer_t<CharT>;
        using const_pointer     = stl::add_const_t<stl::add_pointer_t<stl::add_const_t<CharT>>>;
        using reference         = stl::add_lvalue_reference_t<value_type>;
        using const_reference   = stl::add_lvalue_reference_t<stl::add_const_t<value_type>>;
        using difference_type   = stl::ptrdiff_t;
        using iterator_category = stl::forward_iterator_tag;
        using iterator_concept  = stl::forward_iterator_tag;

        static constexpr stl::size_t pin_count = PinCount;

        static_assert(pin_count != 0, "This class is useless where you don't need the pins.");

      private:
        template <std::size_t, std::size_t, istl::CharType, UTF32>
        friend struct details::pin_type;

        using non32_value_type = stl::conditional_t<UTF32<CharT>, istl::nothing_type, value_type>;
        pointer                                ptr;
        [[no_unique_address]] non32_value_type code_point;

        stl::array<pointer, PinCount>          ptrs;
        stl::array<stl::int_fast8_t, PinCount> states;

        template <stl::size_t Index>
        using pin_type_of = details::pin_type<Index, PinCount, CharT, CodePointT>;

      public:
        explicit constexpr utf_reducer(pointer inp_pos = nullptr) : ptr{inp_pos} {
            assert(is_code_unit_start(*inp_pos));
            if constexpr (!UTF32<CharT>) {
                code_point = next_code_point_copy(ptr);
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
            return {this};
        }

        /// Get all pins in a tuple construct
        /// Usage:
        ///   auto [pin1, pin2, pin3] = reducer.pins();
        [[nodiscard]] constexpr auto pins() noexcept {
            return ([this]<stl::size_t... I>() {
                return stl::make_tuple(pin<I>()...);
            })(std::make_index_sequence<PinCount>{});
        }

        [[nodiscard]] constexpr const_reference operator*() const noexcept {
            if constexpr (UTF32<CharT>) {
                return *ptr;
            } else {
                return code_point;
            }
        }

        [[nodiscard]] constexpr pointer base() const noexcept {
            return ptr;
        }

        // we intentionally don't allow dereferencing because the user of this class may do something like
        // *iter = new_value;
        // [[nodiscard]] constexpr reference operator*() noexcept
        // {
        //     return code_point;
        // }

        constexpr stl::size_t set_code_point(value_type inp_code_point, difference_type length)
          noexcept(std::is_nothrow_copy_assignable_v<value_type>) {
            if constexpr (UTF32<CharT>) {
                *ptr = inp_code_point;
                return 1U;
            } else {
                auto const orig_len = required_length_of(*ptr);
                auto const rep_len  = utf8_length_from_utf32(inp_code_point);
                assert(rep_len <= orig_len); // It'll replace content, most-likely not what you need.
                assert(rep_len <= length);   // not enough room
                if (rep_len < orig_len) {
                    stl::rotate(ptr + rep_len, ptr + orig_len, ptr + length);
                }
                set_code_point(inp_code_point);
                return rep_len;
            }
        }

        constexpr stl::size_t set_code_point(const_pointer other_ptr, difference_type length)
          noexcept(std::is_nothrow_copy_assignable_v<value_type>) {
            if constexpr (UTF32<CharT>) {
                *ptr = *other_ptr;
                return 1U;
            } else {
                auto const orig_len = required_length_of(*ptr);
                auto const rep_len  = required_length_of(*other_ptr);
                assert(rep_len <= orig_len); // It'll replace content, most-likely not what you need.
                assert(rep_len <= length);   // not enough room
                if (rep_len < orig_len) {
                    stl::rotate(ptr + rep_len, ptr + orig_len, ptr + length);
                }
                set_code_point(other_ptr);
                return rep_len;
            }
        }

        constexpr stl::size_t set_code_point(const_pointer other_ptr)
          noexcept(std::is_nothrow_copy_assignable_v<value_type>) {
            if constexpr (UTF32<CharT>) {
                *ptr = *other_ptr;
                return 1U;
            } else {
                auto       ptr_cpy     = ptr;
                auto const changed_len = unchecked::append(ptr_cpy, other_ptr);
                code_point             = next_code_point_copy(ptr);
                return changed_len;
            }
        }

        constexpr stl::size_t set_code_point(value_type inp_code_point)
          noexcept(std::is_nothrow_copy_assignable_v<value_type>) {
            if constexpr (UTF32<CharT>) {
                *ptr = inp_code_point;
                return 1U;
            } else {
                auto ptr_cpy = ptr;
                code_point   = inp_code_point;
                return unchecked::append(ptr_cpy, inp_code_point);
            }
        }

        template <typename CharT2 = CharT>
        constexpr stl::size_t set_code_point(
          utf_reducer<PinCount, CharT2, CodePointT> const& other_ptr,
          pointer end) noexcept(std::is_nothrow_copy_assignable_v<value_type>) {
            return set_code_point(other_ptr.base(), end - ptr);
        }

        template <typename CharT2 = CharT>
        constexpr stl::size_t set_code_point(utf_reducer<PinCount, CharT2, CodePointT> const& other_ptr)
          noexcept(std::is_nothrow_copy_assignable_v<value_type>) {
            return set_code_point(other_ptr.base());
        }

        template <typename CharT2 = CharT>
        constexpr stl::size_t set_code_point(value_type                                       other,
                                             utf_reducer<PinCount, CharT2, CodePointT> const& end)
          noexcept(std::is_nothrow_copy_assignable_v<value_type>) {
            return set_code_point(other, end.base() - ptr);
        }

        template <typename CharT2 = stl::add_const_t<CharT>>
        constexpr stl::size_t set_code_point(const_pointer                                    other_ptr,
                                             utf_reducer<PinCount, CharT2, CodePointT> const& end)
          noexcept(std::is_nothrow_copy_assignable_v<value_type>) {
            return set_code_point(other_ptr.base(), end.base() - ptr);
        }

        template <typename CharT2 = stl::add_const_t<CharT>>
        constexpr stl::size_t set_code_point(utf_reducer<PinCount, CharT2, CodePointT> const& other_ptr,
                                             utf_reducer<PinCount, CharT2, CodePointT> const& end)
          noexcept(std::is_nothrow_copy_assignable_v<value_type>) {
            return set_code_point(other_ptr.base(), end.base() - ptr);
        }

        [[nodiscard]] constexpr const_pointer operator->() const noexcept {
            return ptr;
        }

        [[nodiscard]] constexpr bool operator==(const_pointer other) const noexcept {
            return ptr == other;
        }

        [[nodiscard]] constexpr bool operator!=(const_pointer other) const noexcept {
            return ptr != other;
        }

        [[nodiscard]] constexpr stl::strong_ordering operator<=>(const_pointer other) const noexcept {
            // this might have unintentional consequences if other is in the middle of a UTF-8 code point and
            // the ptr is in the beginning of that code point.
            // we're assuming pointers can't be pointed to the middle of the Unicode code points.
            return ptr <=> other;
        }

        [[nodiscard]] constexpr stl::strong_ordering operator<=>(utf_reducer const& other) const noexcept {
            return ptr <=> other.base();
        }

        constexpr utf_reducer& operator++() noexcept {
            if constexpr (UTF32<CharT>) {
                ++ptr;
            } else {
                code_point = next_code_point(ptr);
            }
            return *this;
        }

        constexpr utf_reducer operator++(int) noexcept {
            return utf_reducer{*this}.operator++();
        }

        constexpr utf_reducer& operator--() noexcept {
            if constexpr (UTF32<CharT>) {
                --ptr;
            } else {
                code_point = prev_code_point(ptr);
            }
            return *this;
        }

        constexpr utf_reducer operator--(int) noexcept {
            return utf_reducer{*this}.operator--();
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
