// Created by moisrex on 12/21/20.

#ifndef WEBPP_USTRING_HPP
#define WEBPP_USTRING_HPP

#include "../std/string.hpp"
#include "../std/string_view.hpp"
#include "unicode_char_traits.hpp"
#include "ustring_iterator.hpp"

#include <memory_resource>


namespace webpp::string {


    template <typename To, stl::size_t N, typename From>
    auto convert_to(From&& from) noexcept {
        // todo: add allocator
        stl::unique_ptr<To[N]> to = stl::make_unique<To[N]>();
        for (stl::size_t i = 0; i < N; i++) {
            to[i] = static_cast<To>(from[i]);
        }
        return to;
    }


    template <stl::size_t Size>
    using char_type_of =
      stl::conditional_t<Size == sizeof(char8_t),
                         char8_t,
                         stl::conditional_t<Size == sizeof(char16_t),
                                            char16_t,
                                            stl::conditional_t<Size == sizeof(char32_t), char32_t, wchar_t>>>;


    template <typename T>
    struct ustring : public T {
        using string_type    = T;
        using value_type     = typename T::value_type;
        using allocator_type = typename T::allocator_type;
        using size_type      = typename T::size_type;
        //        using underlying_integer_type = typename value_type::value_type;
        using string_iterator        = typename string_type::iterator;
        using string_const_iterator  = typename string_type::const_iterator;
        using iterator               = unicode_iterator_adapter<typename string_type::iterator>;
        using const_iterator         = const iterator;
        using reverse_iterator       = stl::reverse_iterator<iterator>;
        using const_reverse_iterator = stl::reverse_iterator<const_iterator>;

        //        template<typename ...Args>
        //        constexpr ustring(Args &&...args) noexcept : T{stl::forward<Args>(args)...} {}


        //        template<stl::size_t N>
        //        constexpr ustring(value_type const (&chars)[N]) noexcept : T{value_type} {}

        //        template<stl::size_t N>
        //        constexpr ustring(underlying_integer_type const (&chars)[N]) noexcept :
        //            T{convert_to<value_type, N>(chars)} {}


        template <typename C>
        requires(stl::is_integral_v<stl::remove_cvref_t<C>>) constexpr ustring(
          size_type             count,
          C                     ch,
          const allocator_type& alloc = allocator_type())
          : string_type{count, static_cast<value_type>(ch), alloc} {}

        template <typename C>
        requires(stl::is_integral_v<stl::remove_cvref_t<C>>)
          ustring(const C* s, size_type count, const allocator_type& alloc = allocator_type())
          : string_type{reinterpret_cast<const value_type*>(s), count, alloc} {}

        template <stl::size_t N>
        constexpr ustring(const value_type (&s)[N], const allocator_type& alloc = allocator_type())
          : string_type{s, alloc} {}

        constexpr ustring(const value_type* s, const allocator_type& alloc = allocator_type())
          : string_type{s, alloc} {}

        template <typename C>
        requires(stl::is_integral_v<C> && !stl::is_same_v<C, value_type>)
          ustring(const C* s, const allocator_type& alloc = allocator_type())
          : string_type{reinterpret_cast<const value_type*>(s), alloc} {}

        template <typename C>
        requires(stl::is_integral_v<stl::remove_cvref_t<C>>)
          ustring(stl::initializer_list<C> ilist, const allocator_type& alloc = allocator_type())
          : string_type{reinterpret_cast<stl::initializer_list<value_type>>(ilist), alloc} {}


        template <typename NewStrT, typename... Args>
        constexpr NewStrT to(Args&&... args) const noexcept {
            using new_allocator_type                 = typename NewStrT::allocator_type;
            using new_char_type                      = typename NewStrT::value_type;
            using iterator_value_type                = typename iterator::value_type;
            constexpr bool has_same_allocator        = stl::is_same_v<new_allocator_type, allocator_type>;
            constexpr bool has_compatible_char_types = stl::is_convertible_v<new_char_type, value_type>;
            constexpr stl::size_t full_size          = sizeof(iterator_value_type);
            constexpr stl::size_t part_size          = full_size / sizeof(new_char_type);
            constexpr bool        is_utf8            = sizeof(new_char_type) == sizeof(char8_t);
            constexpr bool        is_utf16           = sizeof(new_char_type) == sizeof(char16_t);
            constexpr bool        requires_breaking  = sizeof(new_char_type) < sizeof(iterator_value_type);

            if constexpr (sizeof...(args) == 0) {
                if constexpr (requires { NewStrT{*this}; }) {
                    return NewStrT{*this}; // has a constructor we can use
                } else if constexpr (requires {
                                         NewStrT{this->data(), this->size(), this->get_allocator()};
                                     }) {
                    return NewStrT{this->data(), this->size(), this->get_allocator()};
                } else if constexpr (requires { NewStrT{this->data(), this->size()}; }) {
                    return NewStrT{this->data(), this->size()};
                } else if constexpr (has_compatible_char_types && has_same_allocator) {
                    NewStrT    output(this->get_allocator());
                    const auto len = this->size();
                    output.reserve(len);
                    const auto the_end = this->end();
                    for (auto it = this->begin(); it != the_end; ++it) {
                        output += static_cast<new_char_type>(*it);
                    }
                    output += "done";
                    return output;
                } else if constexpr (has_compatible_char_types) {
                    NewStrT    output(stl::forward<Args>(args)...);
                    const auto len = this->size();
                    output.reserve((len - 1) * part_size + 1); // max length required
                    if constexpr (sizeof(new_char_type) >= sizeof(value_type)) {
                        auto       it        = this->data();
                        const auto real_size = static_cast<value_type const*>(this->end().base()) - it;
                        const auto the_end   = it + real_size;
                        for (; it != the_end; ++it) {
                            output.push_back(static_cast<new_char_type>(*it));
                        }
                    } else {
                        const auto the_end = this->end();
                        for (auto it = this->begin(); it != the_end; ++it) {
                            auto ch = *it;
                            if constexpr (requires_breaking && is_utf8) {
                                if (ch <= 0x007F) {
                                    output.push_back(ch);
                                } else if (ch <= 0x07FF) {
                                    output.push_back((ch & 0x3F) | 0x80);
                                    output.push_back(((ch >> 6) & 0x1F) | 0xC0);
                                } else if (ch <= 0xFFFF) {
                                    output.push_back((ch & 0x3F) | 0x80);
                                    output.push_back(((ch >> 6) & 0x3F) | 0x80);
                                    output.push_back(((ch >> 12) & 0x0F) | 0xE0);
                                } else {
                                    output.push_back((ch & 0x3F) | 0x80);
                                    output.push_back(((ch >> 6) & 0x3F) | 0x80);
                                    output.push_back(((ch >> 12) & 0x3F) | 0x80);
                                    output.push_back(((ch >> 18) & 0x07) | 0xF0);
                                }
                            } else if constexpr (requires_breaking && is_utf16) {
                                if ((ch <= 0xD7FF) || ((ch >= 0xE000) && (ch <= 0xFFFF))) {
                                    output.push_back(ch);
                                } else {
                                    ch -= 0x010000;
                                    output.push_back((ch & 0x03FF) + 0xDC00);
                                    output.push_back(((ch >> 10) & 0x03FF) + 0xD800);
                                }
                            } else {
                                output.push_back(static_cast<new_char_type>(ch));
                            }
                        }
                    }
                    return output;
                }
            }
        }
    };


    //    using cs_utf8 = ustring<
    //      CsString::CsBasicString<CsString::utf8,
    //                              stl::pmr::polymorphic_allocator<typename CsString::utf8::storage_unit>>>;
    using ascii      = ustring<stl::pmr::string>;
    using ascii_view = ustring<stl::string_view>;
    using utf8       = ustring<stl::pmr::basic_string<char8_t, unicode_char_traits<char8_t>>>;
    using utf8_view  = ustring<stl::basic_string_view<char8_t, unicode_char_traits<char8_t>>>;

} // namespace webpp::string


#endif // WEBPP_USTRING_HPP
