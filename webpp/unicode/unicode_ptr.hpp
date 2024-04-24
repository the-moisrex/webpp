// Created by moisrex on 12/21/20.

#ifndef WEBPP_UNICODE_PTR_HPP
#define WEBPP_UNICODE_PTR_HPP

#include "../std/concepts.hpp"
#include "../std/type_traits.hpp"
#include "unicode.hpp"

#include <compare>

namespace webpp::unicode {

    namespace details {

        template <typename T>
        concept is_value = stl::is_integral_v<T>;

        template <typename T>
        concept is_pointer = stl::is_pointer_v<T> && is_value<stl::remove_pointer_t<T>>;


    } // namespace details

    /**
     * This is a reference_wrapper type for glyph
     */
    template <typename GlyphType, typename ContainerType>
    struct glyph_reference_wrapper {
        using glyph_type      = GlyphType;
        using container_type  = ContainerType;
        using container_ref   = stl::add_lvalue_reference_t<container_type>;
        using char_type       = typename glyph_type::char_type;
        using code_point_type = typename glyph_type::code_point_type;
        using pointer         = typename glyph_type::pointer;

        container_ref container;
        pointer       start;

        template <typename T>
            requires(stl::same_as<T, char_type> || stl::same_as<T, code_point_type>)
        constexpr stl::strong_ordering operator<=>(T val) const noexcept {
            if constexpr (stl::same_as<T, char_type>) {
                return *start <=> val;
            } else {
                return unicode::code_point<char_type, code_point_type>(start);
            }
        }
    };

    /**
     * This is a "fancy pointer" for storage_unit
     * Fancy Pointer: (https://en.cppreference.com/w/cpp/named_req/Allocator#Fancy_pointers)
     * An example of fancy pointer:
     * https://www.boost.org/doc/libs/1_78_0/doc/html/boost/interprocess/offset_ptr.html
     */
    template <typename StorageUnitType>
    struct unicode_ptr {
        using storage_unit_type       = StorageUnitType;
        using char_type               = typename storage_unit_type::char_type;
        using code_point_type         = typename storage_unit_type::code_point_type;
        using pointer                 = char_type*;
        using const_pointer           = char_type const*;
        using difference_type         = stl::ptrdiff_t;
        using const_storage_unit_type = typename storage_unit_type::const_storage_unit_type;
        using iterator_category       = stl::random_access_iterator_tag;
        using iterator_concept        = stl::random_access_iterator_tag;

        static constexpr bool is_storage_const = stl::is_const_v<storage_unit_type>;

        using element_type    = storage_unit_type;
        using reference       = stl::conditional_t<is_storage_const, element_type const&, element_type&>;
        using const_reference = element_type const&;
        // using const_pointer   = unicode_ptr<const_storage_unit_type>;

        using element_ptr =
          stl::add_pointer_t<stl::conditional_t<is_storage_const, element_type const, element_type>>;
        using const_element_ptr = stl::add_const_t<element_ptr>;


        static constexpr bool is_utf8  = unicode::UTF8<char_type>;
        static constexpr bool is_utf16 = unicode::UTF16<char_type>;

      private:
        pointer start{nullptr};

      public:
        constexpr unicode_ptr() = default;

        template <typename T>
            requires(
              sizeof(T) == sizeof(char_type) && !stl::same_as<T, char_type>) // same size but not char_type
        explicit constexpr unicode_ptr(T* inp_ptr) noexcept : start{reinterpret_cast<pointer>(inp_ptr)} {}

        explicit constexpr unicode_ptr(pointer inp_ptr) noexcept : start{inp_ptr} {}

        explicit constexpr unicode_ptr(reference inp_ref) noexcept : start{&inp_ref.value} {}

        // this is used by std::pointer_traits<T>::pointer_to
        static constexpr unicode_ptr pointer_to(reference inp_ref) noexcept {
            return inp_ref;
        }

        // this is used by std::pointer_traits<T>::to_address
        static element_type* to_address(pointer ptr) noexcept {
            return reinterpret_cast<element_type*>(ptr);
        }

        static constexpr element_type* to_address(element_type* ptr) noexcept {
            return ptr;
        }

        [[nodiscard]] constexpr explicit operator pointer() noexcept {
            return start;
        }

        [[nodiscard]] explicit constexpr operator element_type() noexcept {
            return {*start};
        }

        [[nodiscard]] explicit constexpr operator element_type const*() const noexcept {
            return reinterpret_cast<element_type const*>(start);
        }

        [[nodiscard]] explicit constexpr operator element_type*() noexcept {
            return reinterpret_cast<element_type*>(start);
        }

        constexpr const_element_ptr operator->() const noexcept {
            return reinterpret_cast<const_element_ptr>(start);
        }

        // Random access iterator requirements
        constexpr const_reference operator[](difference_type n) const noexcept {
            return reinterpret_cast<const_reference>(start[n]);
        }

        constexpr reference operator[](difference_type n) noexcept {
            return reinterpret_cast<reference>(start[n]);
        }

        // constexpr code_point_type operator[](difference_type n) noexcept {
        //     return operator+(n).operator*();
        // }

        // constexpr stl::strong_ordering operator<=>(unicode_ptr const&) const noexcept = default;
        constexpr stl::strong_ordering operator<=>(pointer const& p) const noexcept {
            return start <=> p;
        }

        constexpr code_point_type operator*() const noexcept {
            return unicode::code_point<char_type, code_point_type>(start);
        }

        constexpr unicode_ptr& operator++() noexcept {
            unicode::unchecked::next_char(start);
            return *this;
        }

        constexpr unicode_ptr operator++(int) noexcept { // NOLINT(cert-dcl21-cpp)
            return unicode_ptr{*this}.operator++();
        }

        constexpr unicode_ptr& operator--() noexcept {
            unicode::unchecked::prev_char(start);
            return *this;
        }

        constexpr unicode_ptr operator--(int) noexcept { // NOLINT(cert-dcl21-cpp)
            return unicode_ptr{*this}.operator--();
        }

        constexpr unicode_ptr& operator+=(difference_type n) noexcept {
            for (; n != 0; --n) {
                this->operator++();
            }
            return *this;
        }

        constexpr unicode_ptr& operator-=(difference_type n) noexcept {
            for (; n != 0; --n) {
                this->operator--();
            }
            return *this;
        }

        constexpr unicode_ptr operator+(difference_type n) const noexcept {
            unicode_ptr ret{*this};
            for (; n != 0; --n) {
                ret.operator++();
            }
            return ret;
        }

        constexpr unicode_ptr operator-(difference_type n) const noexcept {
            unicode_ptr ret{*this};
            for (; n != 0; --n) {
                ret.operator--();
            }
            return ret;
        }
    };

    /**
     * This type can handle both a pointer and a value
     */
    template <details::is_value CharT = char8_t, details::is_value CodePointType = char32_t>
    struct storage_unit {
        using char_type               = CharT;
        using code_point_type         = CodePointType;
        using type                    = storage_unit<CharT, CodePointType>;
        using const_storage_unit_type = storage_unit<CharT, CodePointType> const;

        // the std::iterator_traits uses this to get the pointer type
        using pointer = unicode_ptr<type>;

        static_assert(sizeof(CodePointType) >= sizeof(CharT),
                      "The specified wide char type is smaller than the unicode char type");

        static constexpr bool is_utf16 = unicode::UTF16<char_type>;
        static constexpr bool is_utf8  = unicode::UTF8<char_type>;
        static constexpr bool is_wchar = unicode::WChar<char_type>;

        char_type value; // NOLINT(misc-non-private-member-variables-in-classes)

        template <typename C>
            requires(
              details::is_value<stl::remove_cvref_t<C>> && !stl::is_same_v<stl::remove_cvref_t<C>, char_type>)
        constexpr explicit(false) storage_unit(C inp_char) noexcept
          : value(static_cast<char_type>(inp_char)) {}

        constexpr explicit(false) storage_unit(char_type val) noexcept : value(val) {}

        constexpr storage_unit() noexcept                    = default;
        constexpr storage_unit(storage_unit const&)          = default;
        constexpr storage_unit(storage_unit&&) noexcept      = default;
        storage_unit& operator=(storage_unit const& val)     = default;
        storage_unit& operator=(storage_unit&& val) noexcept = default;
        constexpr ~storage_unit() noexcept                   = default;

        template <typename C>
            requires(stl::is_integral_v<stl::remove_cvref_t<C>> && sizeof(C) == sizeof(char_type))
        storage_unit& operator=(C c) {
            value = static_cast<char_type>(c);
            return *this;
        }

        /**
         * Let it to be converted to another integer type with static_cast
         */
        template <typename IntType>
            requires(stl::is_integral_v<stl::remove_cvref_t<IntType>>)
        [[nodiscard]] explicit constexpr operator IntType() const noexcept {
            return static_cast<IntType>(value);
        }

        constexpr bool operator==(storage_unit const& val) const noexcept = default;

        template <typename IntT>
            requires(details::is_value<stl::remove_cvref_t<IntT>>)
        constexpr stl::strong_ordering operator<=>(IntT val) const noexcept {
            return value <=> val;
        }

        template <typename IntT>
            requires(details::is_value<stl::remove_cvref_t<IntT>>)
        constexpr stl::strong_ordering operator<=>(storage_unit<IntT> val) const noexcept {
            return value <=> static_cast<char_type>(val.value);
        }
    };

    template <typename CharT, typename CodePointT>
    constexpr bool operator==(CharT lhs, storage_unit<CharT, CodePointT> const& rhs) noexcept {
        return rhs == lhs;
    }

    template <typename ChT, typename CharT, typename CodePointT>
        requires requires(ChT str, CharT val) { static_cast<CharT>(str) <=> val; }
    constexpr auto operator<=>(ChT str, storage_unit<CharT, CodePointT> const& unit) noexcept {
        return static_cast<CharT>(str) == unit.value;
    }

    using utf8_storage_unit   = storage_unit<char8_t, char32_t>;
    using utf16_storage_unit  = storage_unit<char16_t, char32_t>;
    using utf32_storage_unit  = storage_unit<char32_t, char32_t>;
    using wchar_strorage_unit = storage_unit<wchar_t, wchar_t>;


    template <typename T>
    using is_storage_unit = istl::is_specialization_of<T, storage_unit>;

    template <typename T>
    static constexpr bool is_storage_unit_v = istl::is_specialization_of_v<T, storage_unit>;

    template <typename T>
    concept StorageUnit = is_storage_unit_v<T>;

#ifdef DEBUG
    static_assert(stl::is_standard_layout_v<storage_unit<>>, "The glyph won't work in std::basic_string");
    static_assert(stl::is_trivial_v<storage_unit<>>, "The glyph needs to be a trivial type");
#endif

} // namespace webpp::unicode

#endif // WEBPP_UNICODE_PTR_HPP
