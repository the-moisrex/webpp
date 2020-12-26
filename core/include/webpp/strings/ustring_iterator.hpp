// Created by moisrex on 12/21/20.

#ifndef WEBPP_USTRING_ITERATOR_HPP
#define WEBPP_USTRING_ITERATOR_HPP

#include "../std/type_traits.hpp"
#include "glyph.hpp"

#include <iterator>

namespace webpp {

    /**
     * Satisfies:
     *   - [Random Access Iterator](https://en.cppreference.com/w/cpp/iterator/random_access_iterator)
     *   - [LegacyRandomAccessIterator](https://en.cppreference.com/w/cpp/named_req/RandomAccessIterator)
     *
     */
    template <typename StorageUnitType = storage_unit<>> // todo: use a pointer instead of a type
    struct unicode_iterator : stl::random_access_iterator_tag {
        using storage_unit_type = StorageUnitType;
        using code_point_type   = typename storage_unit_type::code_point_type;
        using iterator_type     = code_point_type; // todo: this should be a pointer or a const pointer
        using traits_type       = stl::iterator_traits<iterator_type>;
        using iterator_category = typename traits_type::iterator_category;
        using value_type        = typename traits_type::value_type;
        using difference_type   = typename traits_type::difference_type;
        using reference         = typename traits_type::reference; // todo
        using pointer           = typename traits_type::pointer;   // todo
        using iterator_concept  = stl::random_access_iterator_tag;

      private:
        pointer current{};

      public:
        constexpr unicode_iterator() noexcept = default;
        explicit constexpr unicode_iterator(const pointer& i) noexcept : current(i) {}

        // Allow iterator to const_iterator conversion
        constexpr unicode_iterator(const unicode_iterator<storage_unit_type>& i) noexcept
          : current(i.base()) {}

        // Forward iterator requirements
        constexpr reference operator*() const noexcept {
            return *current;
        }

        constexpr pointer operator->() const noexcept {
            return current;
        }

        constexpr unicode_iterator& operator++() noexcept {
            ++current;
            return *this;
        }

        constexpr unicode_iterator operator++(int) noexcept {
            return unicode_iterator(current++);
        }

        // Bidirectional iterator requirements
        constexpr unicode_iterator& operator--() noexcept {
            --current;
            return *this;
        }

        constexpr unicode_iterator operator--(int) noexcept {
            return unicode_iterator(current--);
        }

        // Random access iterator requirements
        constexpr reference operator[](difference_type n) const noexcept {
            return current[n];
        }

        constexpr unicode_iterator& operator+=(difference_type n) noexcept {
            current += n;
            return *this;
        }

        constexpr unicode_iterator operator+(difference_type n) const noexcept {
            return unicode_iterator(current + n);
        }

        constexpr unicode_iterator& operator-=(difference_type n) noexcept {
            current -= n;
            return *this;
        }

        constexpr unicode_iterator operator-(difference_type n) const noexcept {
            return unicode_iterator(current - n);
        }

        constexpr const pointer& base() const noexcept {
            return current;
        }

        // todo: add comparison operators

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



} // namespace webpp

#endif // WEBPP_USTRING_ITERATOR_HPP
