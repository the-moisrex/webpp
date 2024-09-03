// Created by moisrex on 9/2/24.

#ifndef WEBPP_UNICODE_CODE_POINT_ITERATOR_HPP
#define WEBPP_UNICODE_CODE_POINT_ITERATOR_HPP

#include "../std/type_traits.hpp"
#include "./unicode.hpp"

namespace webpp::unicode {

    /**
     * An iterator-like for Unicode code points
     * We're assuming pointers can't be pointed to the middle of the Unicode code points.
     *
     * This class is not designed to be used everywhere, it's designed to simplify writing some algorithms.
     * There are many problems with this class that makes it less general as you'd like, including:
     *  - Multiple iterators of the same place will not be synced when one of them changes the value
     *  - *iter = new_value; will not work intentionally to make sure it's not a mistake, use set_value.
     *  - It's bidirectional, not random-access.
     *
     * @tparam CharT Your Character Type (char, char8_t, ...)
     * @tparam CodePointT The Code Point Type (Must be UTF-32)
     */
    template <typename CharT = char8_t const, UTF32 CodePointT = char32_t>
    struct code_point_iterator {
        using value_type        = CodePointT;
        using pointer           = stl::add_pointer_t<CharT>;
        using const_pointer     = stl::add_const_t<stl::add_pointer_t<stl::add_const_t<CharT>>>;
        using reference         = stl::add_lvalue_reference_t<value_type>;
        using const_reference   = stl::add_lvalue_reference_t<stl::add_const_t<value_type>>;
        using difference_type   = stl::ptrdiff_t;
        using iterator_category = stl::bidirectional_iterator_tag;
        using iterator_concept  = stl::bidirectional_iterator_tag;

        static constexpr bool is_mutable = !stl::is_const_v<CharT>;

      private:
        using non32_value_type = stl::conditional_t<UTF32<CharT>, istl::nothing_type, value_type>;
        pointer                                ptr;
        [[no_unique_address]] non32_value_type code_point;

      public:
        explicit constexpr code_point_iterator(pointer inp_pos = nullptr) : ptr{inp_pos} {
            if constexpr (!UTF32<CharT>) {
                code_point = next_code_point_copy(ptr);
            }
        }

        constexpr code_point_iterator(code_point_iterator const&)                = default;
        constexpr code_point_iterator(code_point_iterator&&) noexcept            = default;
        constexpr code_point_iterator& operator=(code_point_iterator const&)     = default;
        constexpr code_point_iterator& operator=(code_point_iterator&&) noexcept = default;
        constexpr ~code_point_iterator() noexcept                                = default;

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
        //     requires(is_mutable)
        // {
        //     return code_point;
        // }

        constexpr void set_value(value_type inp_code_point) noexcept(
          std::is_nothrow_copy_assignable_v<value_type>)
            requires(is_mutable)
        {
            if constexpr (UTF32<CharT>) {
                *ptr = inp_code_point;
            } else {
                auto ptr_cpy = ptr;
                unchecked::append(ptr_cpy, inp_code_point);
                code_point = inp_code_point;
            }
        }

        constexpr void set_value(const_pointer other_ptr) noexcept(
          std::is_nothrow_copy_assignable_v<value_type>)
            requires(is_mutable)
        {
            if constexpr (UTF32<CharT>) {
                *ptr = *other_ptr;
            } else {
                auto ptr_cpy = ptr;
                unchecked::append(ptr_cpy, other_ptr);
                code_point = next_code_point_copy(ptr);
            }
        }

        template <typename CharT2 = stl::add_const_t<CharT>>
        constexpr void set_value(code_point_iterator<CharT2, CodePointT> const& other_ptr) noexcept(
          std::is_nothrow_copy_assignable_v<value_type>)
            requires(is_mutable)
        {
            if constexpr (UTF32<CharT>) {
                *ptr = *other_ptr;
            } else {
                auto ptr_cpy = ptr;
                unchecked::append(ptr_cpy, other_ptr.base());
                code_point = *other_ptr;
            }
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

        [[nodiscard]] constexpr stl::strong_ordering operator<=>(
          code_point_iterator const& other) const noexcept {
            return ptr <=> other.base();
        }

        constexpr code_point_iterator& operator++() noexcept {
            if constexpr (UTF32<CharT>) {
                ++ptr;
            } else {
                code_point = next_code_point(ptr);
            }
            return *this;
        }

        constexpr code_point_iterator operator++(int) noexcept {
            return code_point_iterator{*this}.operator++();
        }

        constexpr code_point_iterator& operator--() noexcept {
            if constexpr (UTF32<CharT>) {
                --ptr;
            } else {
                code_point = prev_code_point(ptr);
            }
            return *this;
        }

        constexpr code_point_iterator operator--(int) noexcept {
            return code_point_iterator{*this}.operator--();
        }
    };

    template <typename CharT1, typename CharT2, typename CP1, typename CP2>
    [[nodiscard]] static constexpr auto operator-(
      code_point_iterator<CharT1, CP1> const& lhs,
      code_point_iterator<CharT2, CP2> const& rhs) -> decltype(lhs.base() - rhs.base()) {
        return lhs.base() - rhs.base();
    }

    template <typename CharT1, typename CP1>
    [[nodiscard]] static constexpr auto operator-(
      code_point_iterator<CharT1, CP1> const&                  lhs,
      typename code_point_iterator<CharT1, CP1>::const_pointer rhs_ptr) -> decltype(lhs.base() - rhs_ptr) {
        return lhs.base() - rhs_ptr;
    }

    template <typename PtrT>
    code_point_iterator(PtrT&) -> code_point_iterator<stl::remove_pointer_t<stl::remove_cvref_t<PtrT>>>;

    template <typename PtrT>
    code_point_iterator(PtrT const&)
      -> code_point_iterator<
        stl::add_const_t<stl::remove_cvref_t<stl::remove_pointer_t<stl::remove_cvref_t<PtrT>>>>>;

    /// wrap it if it's UTF-8 or UTF-16, but if it's UTF-32, don't wrap:
    template <typename CharT = char8_t const, UTF32 CodePointT = char32_t>
    using code_point_iterator_t =
      stl::conditional_t<UTF32<CharT>, CharT*, code_point_iterator<CharT, CodePointT>>;

} // namespace webpp::unicode

#endif // WEBPP_UNICODE_CODE_POINT_ITERATOR_HPP
