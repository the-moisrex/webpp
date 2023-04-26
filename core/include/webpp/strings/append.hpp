#ifndef WEBPP_STRINGS_APPEND_HPP
#define WEBPP_STRINGS_APPEND_HPP

#include "../std/string.hpp"
#include "../std/string_view.hpp"
#include "../strings/size.hpp"

#include <charconv>
#include <stdexcept>

#ifndef __cpp_lib_to_chars
#    include "../std/format.hpp"
#endif

namespace webpp {


    // todo: add allocator support here and remove the traits:
    //
    //    template <Traits TraitsType, typename ValueType, typename... R>
    //    constexpr auto to_str_copy(ValueType value, R&&... args) noexcept {
    //        using traits_type         = TraitsType;
    //        using char_type           = traits::char_type<traits_type>;
    //        using str_t               = traits::string<traits_type>;
    //        using size_type           = typename str_t::size_type;
    //        constexpr size_type _size = ascii::digit_count<ValueType>() + 1;
    //        if constexpr (stl::is_same_v<char_type, char>) {
    //            str_t str(_size, '\0');
    //            auto [p, _] = stl::to_chars(str.data(), str.data() + _size, value,
    //            stl::forward<R>(args)...); str.resize(p - str.data()); return str;
    //        } else {
    //            char str[_size];
    //            auto [p, _]    = stl::to_chars(str, str + _size, value, stl::forward<R>(args)...);
    //            auto  the_size = static_cast<size_type>(p - str);
    //            str_t res(the_size, '\0');
    //            auto  it = res.begin();
    //            for (auto _c = str; *_c; ++_c) {
    //                *it++ = static_cast<char_type>(*_c);
    //            }
    //            return res;
    //        }
    //    }

    template <typename ValueType>
        requires(istl::StringViewifiable<ValueType>)
    constexpr void append_to(istl::String auto& out, ValueType&& value) {
        out.append(istl::string_data(value), ascii::size(value));
    }

    template <typename ValueType, typename... R>
    constexpr auto append_to(char* ptr, ValueType value, [[maybe_unused]] R&&... args) noexcept {
#ifdef __cpp_lib_to_chars
        constexpr stl::size_t _size = ascii::digit_count<ValueType>() + 1;
        return stl::to_chars(ptr, ptr + _size, value, stl::forward<R>(args)...);
#else
        // todo: use floating point format options feom "args"
        fmt::format_to(ptr, "{}", value);
        return true;
#endif
    }

    template <typename ValueType, typename... R>
    constexpr bool append_to(istl::String auto& str, ValueType value, R&&... args) noexcept {
        using value_type                 = stl::remove_cvref_t<ValueType>;
        constexpr stl::size_t value_size = sizeof(value_type);

        // converting any value type to 3 possible float value types
        using float_type =
          stl::conditional_t<(value_size <= sizeof(float)),
                             float,
                             stl::conditional_t<(value_size == sizeof(double)), double, long double>>;


        if constexpr (istl::StringViewifiable<value_type>) {
            str.append(value);
            (append_to(str, stl::forward<R>(args)), ...);
            return true;
        } else {
#ifdef __cpp_lib_to_chars
            constexpr stl::size_t   _size = ascii::digit_count<value_type>() + 1;
            stl::array<char, _size> chars;
            if (auto res = stl::to_chars(chars.data(),
                                         chars.data() + _size,
                                         static_cast<float_type>(value), // to remove ambiguity
                                         stl::forward<R>(args)...);
                res.ec == stl::errc()) {
                str.append(chars.data(), static_cast<stl::size_t>(res.ptr - chars.data()));
                return true;
            }
            return false;
#else
            fmt::format_to(stl::back_inserter(str), "{}", value);
            return true;
#endif
        }
    }



} // namespace webpp

#endif // WEBPP_STRINGS_APPEND_HPP
