// Created by moisrex on 12/21/20.

#ifndef WEBPP_USTRING_ITERATOR_HPP
#define WEBPP_USTRING_ITERATOR_HPP

#include "../std/type_traits.hpp"
namespace webpp::string {

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

    template <typename IterType, typename CharT = char32_t>
    using default_iterator_char_type =
    stl::conditional_t<sizeof(typename IterType::value_type) == sizeof(CharT),
      typename IterType::value_type,
      CharT>;

    template <typename IterType, typename CharT = default_iterator_char_type<IterType>>
    struct unicode_iterator_adapter : public IterType {
        using IterType::IterType;
        using iter_type = IterType;
        using wide_char = CharT;
        //        using size_type         = typename iter_type::size_type;
        using iterator_type     = typename iter_type::iterator_type;
        using iterator_category = typename iter_type::iterator_category;
        using value_type        = wide_char;
        using difference_type   = typename iter_type::difference_type;
        using reference         = typename iter_type::reference;
        using pointer           = typename iter_type::pointer;

        static_assert(sizeof(CharT) > sizeof(char16_t),
                      "The specified character type is not valid for iterator");
        static_assert(sizeof(CharT) >= sizeof(value_type),
                      "The specified character type is smaller than the original iterator type size.");

        static constexpr bool is_utf16 = sizeof(value_type) == sizeof(char16_t);
        static constexpr bool is_utf8  = sizeof(value_type) == sizeof(char8_t);
        static constexpr bool is_wchar = sizeof(value_type) >= sizeof(char32_t);

        // get the current character value (the one that the parent iterator is pointing to)
        constexpr auto value() const noexcept {
            return iter_type::operator*();
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
                return static_cast<wide_char>(val); // this is the only char
            } else if constexpr (is_utf8) {
                constexpr auto shift_bit_count = sizeof(char8_t) * 8u;
                if ((val & 0x80u) == 0) {
                    // we have one char
                    return static_cast<wide_char>(val);
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

        constexpr unicode_iterator_adapter& operator++() noexcept {
            return iter_type::operator+=(count_bytes<value_type>(this->value()));
        }

        constexpr unicode_iterator_adapter operator++(int) noexcept {
            unicode_iterator_adapter ret{*this};
            ret.                     operator++();
            return ret;
        }

        constexpr unicode_iterator_adapter& operator--() noexcept {
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

        constexpr unicode_iterator_adapter operator--(int) noexcept {
            unicode_iterator_adapter ret{*this};
            ret.                     operator--();
            return ret;
        }

        constexpr unicode_iterator_adapter& operator+=(difference_type n) noexcept {
            for (; n != 0; --n)
                this->operator++();
            return *this;
        }

        constexpr unicode_iterator_adapter& operator-=(difference_type n) noexcept {
            for (; n != 0; --n)
                this->operator--();
            return *this;
        }

        constexpr unicode_iterator_adapter operator+(difference_type n) const noexcept {
            unicode_iterator_adapter ret{*this};
            for (; n != 0; --n)
                ret.operator++();
            return ret;
        }

        constexpr unicode_iterator_adapter operator-(difference_type n) const noexcept {
            unicode_iterator_adapter ret{*this};
            for (; n != 0; --n)
                ret.operator--();
            return ret;
        }

        constexpr reference operator[](difference_type n) noexcept {
            unicode_iterator_adapter ret{*this};
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



} // namespace webpp::string

#endif // WEBPP_USTRING_ITERATOR_HPP
