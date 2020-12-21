// Created by moisrex on 12/21/20.

#ifndef WEBPP_USTRING_HPP
#define WEBPP_USTRING_HPP

#include "../std/string.hpp"
#include "../std/string_view.hpp"
#include "unicode_char_traits.hpp"
#include "ustring_iterator.hpp"

#include <memory_resource>

// testing area: http://localhost:10240/z/z39ErG

namespace webpp {


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


    template <istl::CharType     CharT          = char8_t,
              istl::CharTraits   CharTraitsType = unicode_char_traits<CharT>,
              AllocatorOf<CharT> AllocType      = stl::allocator<CharT>>
    struct ustring {
      private:
        using alloc_traits = stl::allocator_traits<AllocType>;

      public:
        using value_type             = CharT;
        using allocator_type         = typename alloc_traits::template rebind_alloc<value_type>;
        using traits_type            = CharTraitsType;
        using reference              = value_type&;
        using const_reference        = value_type const&;
        using size_type              = typename stl::allocator_traits<allocator_type>::size_type;
        using difference_type        = typename std::allocator_traits<allocator_type>::difference_type;
        using pointer                = typename stl::allocator_traits<allocator_type>::pointer;
        using const_pointer          = typename stl::allocator_traits<allocator_type>::const_pointer;
        using iterator               = unicode_iterator<value_type>;
        using const_iterator         = const iterator;
        using reverse_iterator       = stl::reverse_iterator<iterator>;
        using const_reverse_iterator = stl::reverse_iterator<const_iterator>;

      private:
        // the reason I'm choosing data_end over size is that we're implementing a unicode string and not
        // ascii string; the difference is that "data_start + size != data_end";
        // todo: I need to think if we need a "size" field as well or not!
        pointer                              data_starts;
        pointer                              data_ends;
        [[no_unique_address]] allocator_type alloc;

        static constexpr auto local_capacity = 15 / sizeof(value_type);
        union {
            value_type local_buf[local_capacity + 1];
            size_type  allocated_capacity;
        };

        void set_length(size_type) noexcept {}

      public:
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
                    // todo: fix this
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


    using utf8  = ustring<char8_t>;
    using utf16 = ustring<char16_t>;

    namespace pmr {
        using namespace std::pmr;

        using utf8  = ustring<char8_t, unicode_char_traits<char8_t>, polymorphic_allocator<char8_t>>;
        using utf16 = ustring<char16_t, unicode_char_traits<char16_t>, polymorphic_allocator<char16_t>>;
    } // namespace pmr

} // namespace webpp


#endif // WEBPP_USTRING_HPP
