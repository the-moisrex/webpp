// Created by moisrex on 12/21/20.

#ifndef WEBPP_USTRING_ITERATOR_HPP
#define WEBPP_USTRING_ITERATOR_HPP

#include "../std/type_traits.hpp"

#include <iterator>

namespace webpp {

    namespace details {
        template <typename value_type>
        requires(stl::is_integral_v<value_type>) static constexpr stl::size_t
          count_bytes(value_type value) noexcept {
            if constexpr (sizeof(value_type) == sizeof(char16_t)) {
                if ((value & 0xFC00u) == 0xD800u)
                    return 2;
                return 1;
            } else if constexpr (sizeof(value_type) == sizeof(char8_t)) {
                if ((value & 0x80u) == 0) {
                    return 1;
                } else if ((value & 0xE0u) == 0xC0u) {
                    return 2;
                } else if ((value & 0xF0u) == 0xE0u) {
                    return 3;
                } else if ((value & 0xF8u) == 0xF0u) {
                    return 4;
                }
                return 1;
            } else {
                return 1;
            }
        }
    } // namespace details

    /**
     * Satisfies:
     *   - [Random Access Iterator](https://en.cppreference.com/w/cpp/iterator/random_access_iterator)
     *   - [LegacyRandomAccessIterator](https://en.cppreference.com/w/cpp/named_req/RandomAccessIterator)
     *
     */
    template <typename CharT = char8_t, typename WCharT = char32_t>
    struct unicode_iterator : stl::random_access_iterator_tag {
        using iterator_type     = unicode_iterator<CharT, WCharT>;
        using traits_type       = stl::iterator_traits<iterator_type>;
        using iterator_category = typename traits_type::iterator_category;
        using value_type        = typename traits_type::value_type;
        using difference_type   = typename traits_type::difference_type;
        using reference         = typename traits_type::reference;
        using pointer           = typename traits_type::pointer;
        using iterator_concept  = stl::random_access_iterator_tag;
        using wide_char_type    = WCharT;

        static_assert(sizeof(WCharT) >= sizeof(CharT),
                      "The specified wide char type is smaller than the unicode char type");

        static constexpr bool is_utf16 = sizeof(value_type) == sizeof(char16_t);
        static constexpr bool is_utf8  = sizeof(value_type) == sizeof(char8_t);
        static constexpr bool is_wchar = sizeof(value_type) >= sizeof(wide_char_type);

      private:
        pointer current{};

      public:
        constexpr unicode_iterator() noexcept = default;
        explicit constexpr unicode_iterator(const pointer& i) noexcept : current(i) {}

        // Allow iterator to const_iterator conversion
        constexpr unicode_iterator(const unicode_iterator<CharT>& i) noexcept : current(i.base()) {}

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


        constexpr auto operator*() const noexcept {
            auto val = value();
            if constexpr (is_utf16) {
                if ((val & 0xFC00u) == 0xD800u) {
                    // we have two chars
                    val <<= sizeof(char16_t) * 8u;
                    auto const next_val = operator++(1);
                    val |= *next_val;
                    return val;
                }
                return static_cast<wide_char_type>(val); // this is the only char
            } else if constexpr (is_utf8) {
                constexpr auto shift_bit_count = sizeof(char8_t) * 8u;
                if ((val & 0x80u) == 0) {
                    // we have one char
                    return static_cast<wide_char_type>(val);
                } else if ((val & 0xE0u) == 0xC0u) {
                    // we have 2 chars
                    val <<= shift_bit_count;
                    auto const next_val = operator++(1);
                    val |= *next_val;
                    return val;
                } else if ((val & 0xF0u) == 0xE0u) {
                    // we have 3 chars
                    val <<= shift_bit_count;
                    auto next_val = operator++(1);
                    val |= *next_val;
                    val <<= shift_bit_count;
                    ++next_val;
                    val |= *next_val;
                    return val;
                } else if ((val & 0xF8u) == 0xF0u) {
                    // we have 4 chars
                    val <<= shift_bit_count;
                    auto next_val = operator++(1);
                    val |= *next_val;
                    val <<= shift_bit_count;
                    ++next_val;
                    val |= *next_val;
                    val <<= shift_bit_count;
                    ++next_val;
                    val |= *next_val;
                    return val;
                }
                return val; // return this one anyway
            } else {
                return val;
            }
        }

        constexpr unicode_iterator& operator++() noexcept {
            return iter_type::operator+=(details::count_bytes<value_type>(this->value()));
        }

        constexpr unicode_iterator operator++(int) noexcept {
            unicode_iterator ret{*this};
            ret.             operator++();
            return ret;
        }

        constexpr unicode_iterator& operator--() noexcept {
            while (true) {
                iter_type::operator--();
                auto const val = this->value();
                if constexpr (is_utf16) {
                    if ((val & 0xFC00u) != 0xDC00u) {
                        // we're at the beginning of a char
                        break;
                    }
                } else if (is_utf8) {
                    if ((val & 0xC0u) != 0x80u)
                        break;
                } else {
                    break; // one char at a time
                }
            }
            return *this;
        }

        constexpr unicode_iterator operator--(int) noexcept {
            unicode_iterator ret{*this};
            ret.             operator--();
            return ret;
        }

        constexpr unicode_iterator& operator+=(difference_type n) noexcept {
            for (; n != 0; --n)
                this->operator++();
            return *this;
        }

        constexpr unicode_iterator& operator-=(difference_type n) noexcept {
            for (; n != 0; --n)
                this->operator--();
            return *this;
        }

        constexpr unicode_iterator operator+(difference_type n) const noexcept {
            unicode_iterator ret{*this};
            for (; n != 0; --n)
                ret.operator++();
            return ret;
        }

        constexpr unicode_iterator operator-(difference_type n) const noexcept {
            unicode_iterator ret{*this};
            for (; n != 0; --n)
                ret.operator--();
            return ret;
        }

        constexpr reference operator[](difference_type n) noexcept {
            unicode_iterator ret{*this};
            ret += n;
            return *ret;
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
