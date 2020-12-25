// Created by moisrex on 12/21/20.

#ifndef WEBPP_GLYPH_HPP
#define WEBPP_GLYPH_HPP

#include "../std/type_traits.hpp"

#include <compare>

namespace webpp {

    template <typename T>
    concept UTF8 = (sizeof(T) == sizeof(char8_t)) && !stl::same_as<T, char>;

    template <typename T>
    concept UTF16 = (sizeof(T) == sizeof(char16_t));

    template <typename T>
    concept WChar = (sizeof(T) == sizeof(char32_t));

    namespace details {

        template <typename T>
        concept is_value = stl::is_integral_v<T>;

        template <typename T>
        concept is_pointer = stl::is_pointer_v<T>&& is_value<stl::remove_pointer_t<T>>;



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
     * This is a reference_wrapper type for glyph
     */
    template <typename GlyphType, typename ContainerType>
    struct glyph_reference_wrapper {
        using glyph_type      = GlyphType;
        using container_type  = ContainerType;
        using container_ref   = stl::add_lvalue_reference_t<container_type>;
        using value_type      = typename glyph_type::value_type;
        using code_point_type = typename glyph_type::code_point_type;
        using pointer         = stl::add_pointer_t<value_type>;

        container_ref container;
        pointer       start;

        constexpr stl::strong_ordering operator<=>(auto val) const noexcept {
            return *start <=> val;
        }
    };


    template <typename StorageUnitType>
    struct glyph {
        using storage_unit_type = StorageUnitType;
        using value_type        = typename storage_unit_type::value_type;
        using code_point_type   = typename storage_unit_type::code_point_type;
        using pointer           = stl::add_pointer_t<value_type>;
        using reference         = code_point_type;
        using difference_type   = stl::ptrdiff_t;

        static constexpr bool is_utf8  = UTF8<value_type>;
        static constexpr bool is_utf16 = UTF16<value_type>;

      private:
        pointer start{nullptr};

        constexpr reference value() const noexcept {
            return *start;
        }

        constexpr glyph& next() noexcept {
            ++start;
            return *this;
        }

        constexpr glyph& next(difference_type n) noexcept {
            start += n;
            return *this;
        }

        constexpr glyph& prev() noexcept {
            --start;
            return *this;
        }

        constexpr glyph& prev(difference_type n) noexcept {
            start -= n;
            return *this;
        }


      public:
        // todo
        constexpr pointer operator->() const noexcept {
            return start;
        }

        // Random access iterator requirements
        constexpr reference operator[](difference_type n) const noexcept {
            return start[n];
        }


        constexpr reference operator*() const noexcept {
            auto val = value();
            if constexpr (is_utf16) {
                if ((val & 0xFC00u) == 0xD800u) {
                    // we have two chars
                    val <<= sizeof(char16_t) * 8u;
                    auto const next_val = operator++(1);
                    val |= *next_val;
                    return val;
                }
                return static_cast<code_point_type>(val); // this is the only char
            } else if constexpr (is_utf8) {
                constexpr auto shift_bit_count = sizeof(char8_t) * 8u;
                if ((val & 0x80u) == 0) {
                    // we have one char
                    return static_cast<code_point_type>(val);
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

        constexpr glyph& operator++() noexcept {
            return next(details::count_bytes<value_type>(this->value()));
        }

        constexpr glyph operator++(int) noexcept {
            glyph ret{*this};
            ret.  operator++();
            return ret;
        }

        constexpr glyph& operator--() noexcept {
            while (true) {
                prev();
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

        constexpr glyph operator--(int) noexcept {
            glyph ret{*this};
            ret.  operator--();
            return ret;
        }

        constexpr glyph& operator+=(difference_type n) noexcept {
            for (; n != 0; --n)
                this->operator++();
            return *this;
        }

        constexpr glyph& operator-=(difference_type n) noexcept {
            for (; n != 0; --n)
                this->operator--();
            return *this;
        }

        constexpr glyph operator+(difference_type n) const noexcept {
            glyph ret{*this};
            for (; n != 0; --n)
                ret.operator++();
            return ret;
        }

        constexpr glyph operator-(difference_type n) const noexcept {
            glyph ret{*this};
            for (; n != 0; --n)
                ret.operator--();
            return ret;
        }

        constexpr reference operator[](difference_type n) noexcept {
            glyph ret{*this};
            ret += n;
            return *ret;
        }
    };

    /**
     * This type can handle both a pointer and a value
     */
    template <details::is_value CharT = char8_t, details::is_value CodePointType = char32_t>
    struct storage_unit {
        using value_type      = CharT;
        using code_point_type = CodePointType;
        using type            = storage_unit<CharT, CodePointType>;

        // the std::iterator_traits uses this to get the pointer type
        using pointer = glyph<type>;

        static_assert(sizeof(CodePointType) >= sizeof(CharT),
                      "The specified wide char type is smaller than the unicode char type");

        static constexpr bool is_utf16 = UTF16<value_type>;
        static constexpr bool is_utf8  = UTF8<value_type>;
        static constexpr bool is_wchar = WChar<value_type>;

        value_type value;

        template <typename C>
        requires(details::is_value<stl::remove_cvref_t<C>> &&
                 !stl::is_same_v<stl::remove_cvref_t<C>, value_type>) constexpr explicit(false)
          storage_unit(C c) noexcept
          : value(static_cast<value_type>(c)) {}

        constexpr explicit(false) storage_unit(value_type val) noexcept : value(val) {}
        constexpr storage_unit() noexcept               = default;
        constexpr storage_unit(storage_unit const&)     = default;
        constexpr storage_unit(storage_unit&&) noexcept = default;
        storage_unit& operator=(storage_unit const& val) = default;
        storage_unit& operator=(storage_unit&& val) noexcept = default;

        template <typename C>
        requires(stl::is_integral_v<stl::remove_cvref_t<C>>) storage_unit& operator=(C c) {
            value = static_cast<value_type>(c);
            return *this;
        }

        /**
         * Let it to be converted to another integer type with static_cast
         */
        template <typename IntType>
        requires(stl::is_integral_v<stl::remove_cvref_t<IntType>>) constexpr explicit
        operator IntType() const noexcept {
            return static_cast<IntType>(value);
        }

        template <typename IntT>
        constexpr stl::strong_ordering operator<=>(storage_unit<IntT> val) const noexcept {
            return value <=> static_cast<value_type>(val.value);
        }

        template <typename IntT>
        requires(stl::is_integral_v<stl::remove_cvref_t<IntT>>) constexpr stl::strong_ordering
        operator<=>(IntT&& val) const noexcept {
            return value <=> val;
        }
    };

    using utf8_storage_unit  = storage_unit<char8_t, char32_t>;
    using utf16_storage_unit = storage_unit<char16_t, char32_t>;
    using utf32_storage_unit = storage_unit<char32_t, char32_t>;

    static_assert(stl::is_standard_layout_v<storage_unit<>>, "The glyph won't work in std::basic_string");
    static_assert(stl::is_trivial_v<storage_unit<>>, "The glyph needs to be a trivial type");
} // namespace webpp

#endif // WEBPP_GLYPH_HPP
