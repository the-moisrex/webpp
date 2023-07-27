// Created by moisrex on 12/21/20.

#ifndef WEBPP_USTRING_ITERATOR_HPP
#define WEBPP_USTRING_ITERATOR_HPP

#include "../std/type_traits.hpp"
#include "unicode_ptr.hpp"

#include <iterator>

namespace webpp {

    /**
     * Satisfies:
     *   - [Random Access Iterator](https://en.cppreference.com/w/cpp/iterator/random_access_iterator)
     *   - [LegacyRandomAccessIterator](https://en.cppreference.com/w/cpp/named_req/RandomAccessIterator)
     *
     */
    template <typename CodePointType, typename CharT>
    struct unicode_iterator {
        using code_point_type   = CodePointType;
        using iterator_type     = unicode_iterator<CodePointType, CharT>;
        using value_type        = CharT;
        using difference_type   = stl::ptrdiff_t;
        using reference         = stl::add_lvalue_reference_t<value_type>;
        using pointer           = typename stl::iterator_traits<value_type>::pointer;
        using iterator_category = stl::random_access_iterator_tag;
        using iterator_concept  = stl::random_access_iterator_tag;

      private:
        pointer current{};

      public:
        constexpr unicode_iterator() noexcept = default;
        explicit constexpr unicode_iterator(const pointer& i) noexcept : current(i) {}

        // Allow iterator to const_iterator conversion
        constexpr unicode_iterator(const iterator_type& i) noexcept : current(i.content) {}

        // Forward iterator requirements
        constexpr reference operator*() const noexcept {
            return *current;
        }

        constexpr pointer operator->() const noexcept {
            return current;
        }

        constexpr iterator_type& operator++() noexcept {
            ++current;
            return *this;
        }

        constexpr iterator_type operator++(int) const noexcept {
            return iterator_type{current}.operator++();
        }

        // Bidirectional iterator requirements
        constexpr iterator_type& operator--() noexcept {
            --current;
            return *this;
        }

        constexpr iterator_type operator--(int) const noexcept {
            return iterator_type(current).operator--();
        }

        // Random access iterator requirements
        constexpr reference operator[](difference_type n) const noexcept {
            return current[n];
        }

        constexpr iterator_type& operator+=(difference_type n) noexcept {
            current += n;
            return *this;
        }

        constexpr iterator_type operator+(difference_type n) const noexcept {
            return iterator_type(current + n);
        }

        constexpr iterator_type& operator-=(difference_type n) noexcept {
            current -= n;
            return *this;
        }

        constexpr iterator_type operator-(difference_type n) const noexcept {
            return iterator_type(current - n);
        }

        constexpr const pointer& base() const noexcept {
            return current;
        }


        //        constexpr size_type operator-(difference_type other) const noexcept {
        //            uni_iterator_adapter a = *this;
        //            uni_iterator_adapter b = other;
        //
        //            size_type ret = 0;
        //
        //            for (auto iter = iter_begin; iter != iter_end; ++iter) {
        //                value_type value = *iter;
        //
        //                if (value < 0xDC00 || value > 0xDFFF) {
        //                    // not a surrogate
        //                    ++ret;
        //                }
        //            }
        //
        //            return retval;
        //            if (a < b) {
        //                return 0 - E::distance(a.m_iter, b.m_iter);
        //
        //            } else {
        //                return E::distance(b.m_iter, a.m_iter);
        //            }
        //        }
    };

    template <typename CPL, typename CPR, typename CharTL, typename CharTR>
        requires requires(CharTL lhs, CharTR rhs) {
            { lhs == rhs } -> stl::convertible_to<bool>;
        }
    constexpr bool
    operator==(const unicode_iterator<CPL, CharTL>& lhs,
               const unicode_iterator<CPR, CharTR>& rhs) noexcept(noexcept(lhs.base() == rhs.base())) {
        return lhs.base() == rhs.base();
    }

    template <typename CPL, typename CPR, typename CharTL, typename CharTR>
    constexpr auto
    operator<=>(const unicode_iterator<CPL, CharTL>& lhs,
                const unicode_iterator<CPR, CharTR>& rhs) noexcept(noexcept(lhs.base() <=> rhs.base())) {
        return lhs.base() <=> rhs.base();
    }


} // namespace webpp

#endif // WEBPP_USTRING_ITERATOR_HPP
