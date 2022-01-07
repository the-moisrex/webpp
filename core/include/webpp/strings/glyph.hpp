// Created by moisrex on 12/21/20.

#ifndef WEBPP_GLYPH_HPP
#define WEBPP_GLYPH_HPP

#include "../std/type_traits.hpp"
#include "unicode.hpp"

#include <compare>

namespace webpp {

    namespace details {

        template <typename T>
        concept is_value = stl::is_integral_v<T>;

        template <typename T>
        concept is_pointer = stl::is_pointer_v<T>&& is_value<stl::remove_pointer_t<T>>;


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

        template <typename T>
        requires (stl::same_as<T, value_type> || stl::same_as<T, code_point_type>)
        constexpr stl::strong_ordering operator<=>(T val) const noexcept {
            if constexpr (stl::same_as<T, value_type>) {
                return *start <=> val;
            } else {
                return unicode::code_point<value_type, code_point_type>(start);
            }
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

        static constexpr bool is_utf8  = unicode::UTF8<value_type>;
        static constexpr bool is_utf16 = unicode::UTF16<value_type>;

      private:
        pointer start{nullptr};

        constexpr reference value() const noexcept { // todo: should return reference?
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


        constexpr stl::strong_ordering operator<=>(glyph const&) const noexcept = default;
        constexpr stl::strong_ordering operator<=>(pointer const&p) const noexcept {
            return start <=> p;
        }

        constexpr reference operator*() const noexcept {
            return unicode::code_point<value_type, code_point_type>(start);
        }

        constexpr glyph& operator++() noexcept {
            // todo:
            // unicode::advance(start);
            return *this;
        }

        constexpr glyph operator++(int) noexcept {
            glyph ret{*this};
            ret.  operator++();
            return ret;
        }

        constexpr glyph& operator--() noexcept {
            // while ((static_cast<unsigned char>(*--pos_) & 0xC0u) == 0x80);

            while (true) {
                prev();
                auto const val = this->value();
                if constexpr (is_utf16) {
                    if ((val & 0xFC00u) != 0xDC00u) {
                        // we're at the beginning of a char
                        break;
                    }
                } else if constexpr (is_utf8) {
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

        static constexpr bool is_utf16 = unicode::UTF16<value_type>;
        static constexpr bool is_utf8  = unicode::UTF8<value_type>;
        static constexpr bool is_wchar = unicode::WChar<value_type>;

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

    using utf8_storage_unit   = storage_unit<char8_t, char32_t>;
    using utf16_storage_unit  = storage_unit<char16_t, char32_t>;
    using utf32_storage_unit  = storage_unit<char32_t, char32_t>;
    using wchar_strorage_unit = storage_unit<wchar_t, wchar_t>;

    static_assert(stl::is_standard_layout_v<storage_unit<>>, "The glyph won't work in std::basic_string");
    static_assert(stl::is_trivial_v<storage_unit<>>, "The glyph needs to be a trivial type");
} // namespace webpp

#endif // WEBPP_GLYPH_HPP
