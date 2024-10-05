// Created by moisrex on 9/2/24.

#ifndef WEBPP_UNICODE_CODE_POINT_ITERATOR_HPP
#define WEBPP_UNICODE_CODE_POINT_ITERATOR_HPP

#include "../std/algorithm.hpp"
#include "../std/type_traits.hpp"
#include "./unicode.hpp"

namespace webpp::unicode {

    /**
     * An iterator-like class for Unicode Code Points
     * We're assuming pointers can't be pointed to the middle of the Unicode code points.
     *
     * @tparam CharT Your Character Type (char, char8_t, ...)
     * @tparam CodePointT The Code Point Type (Must be UTF-32)
     */
    template <typename CharT = char8_t const, UTF32 CodePointT = char32_t>
    struct utf_reducer {
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
        explicit constexpr utf_reducer(pointer inp_pos = nullptr) : ptr{inp_pos} {
            if constexpr (!UTF32<CharT>) {
                code_point = next_code_point_copy(ptr);
            }
        }

        constexpr utf_reducer(utf_reducer const&)                = default;
        constexpr utf_reducer(utf_reducer&&) noexcept            = default;
        constexpr utf_reducer& operator=(utf_reducer const&)     = default;
        constexpr utf_reducer& operator=(utf_reducer&&) noexcept = default;
        constexpr ~utf_reducer() noexcept                        = default;

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

        constexpr stl::size_t set_code_point(value_type inp_code_point, difference_type length)
          noexcept(std::is_nothrow_copy_assignable_v<value_type>)
            requires(is_mutable)
        {
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
          noexcept(std::is_nothrow_copy_assignable_v<value_type>)
            requires(is_mutable)
        {
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
          noexcept(std::is_nothrow_copy_assignable_v<value_type>)
            requires(is_mutable)
        {
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
          noexcept(std::is_nothrow_copy_assignable_v<value_type>)
            requires(is_mutable)
        {
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
        constexpr stl::size_t set_code_point(utf_reducer<CharT2, CodePointT> const& other_ptr, pointer end)
          noexcept(std::is_nothrow_copy_assignable_v<value_type>)
            requires(is_mutable)
        {
            return set_code_point(other_ptr.base(), end - ptr);
        }

        template <typename CharT2 = CharT>
        constexpr stl::size_t set_code_point(utf_reducer<CharT2, CodePointT> const& other_ptr)
          noexcept(std::is_nothrow_copy_assignable_v<value_type>)
            requires(is_mutable)
        {
            return set_code_point(other_ptr.base());
        }

        template <typename CharT2 = CharT>
        constexpr stl::size_t set_code_point(value_type other, utf_reducer<CharT2, CodePointT> const& end)
          noexcept(std::is_nothrow_copy_assignable_v<value_type>)
            requires(is_mutable)
        {
            return set_code_point(other, end.base() - ptr);
        }

        template <typename CharT2 = stl::add_const_t<CharT>>
        constexpr stl::size_t set_code_point(
          const_pointer                          other_ptr,
          utf_reducer<CharT2, CodePointT> const& end) noexcept(std::is_nothrow_copy_assignable_v<value_type>)
            requires(is_mutable)
        {
            return set_code_point(other_ptr.base(), end.base() - ptr);
        }

        template <typename CharT2 = stl::add_const_t<CharT>>
        constexpr stl::size_t set_code_point(
          utf_reducer<CharT2, CodePointT> const& other_ptr,
          utf_reducer<CharT2, CodePointT> const& end) noexcept(std::is_nothrow_copy_assignable_v<value_type>)
            requires(is_mutable)
        {
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

    template <typename CharT1, typename CharT2, typename CP1, typename CP2>
    [[nodiscard]] static constexpr auto operator-(
      utf_reducer<CharT1, CP1> const& lhs,
      utf_reducer<CharT2, CP2> const& rhs) -> decltype(lhs.base() - rhs.base()) {
        return lhs.base() - rhs.base();
    }

    template <typename CharT1, typename CP1>
    [[nodiscard]] static constexpr auto operator-(
      utf_reducer<CharT1, CP1> const&                  lhs,
      typename utf_reducer<CharT1, CP1>::const_pointer rhs_ptr) -> decltype(lhs.base() - rhs_ptr) {
        return lhs.base() - rhs_ptr;
    }

    template <typename PtrT>
    utf_reducer(PtrT) -> utf_reducer<stl::remove_pointer_t<stl::remove_cvref_t<PtrT>>>;

    // template <typename PtrT>
    // code_point_iterator(PtrT const&)
    //   -> code_point_iterator<
    //     stl::add_const_t<stl::remove_cvref_t<stl::remove_pointer_t<stl::remove_cvref_t<PtrT>>>>>;

    /// wrap it if it's UTF-8 or UTF-16, but if it's UTF-32, don't wrap:
    template <typename CharT = char8_t const, UTF32 CodePointT = char32_t>
    using code_point_iterator_t = stl::conditional_t<UTF32<CharT>, CharT*, utf_reducer<CharT, CodePointT>>;

} // namespace webpp::unicode

#endif // WEBPP_UNICODE_CODE_POINT_ITERATOR_HPP
