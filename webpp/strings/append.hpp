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
        return stl::to_chars(ptr,
                             ptr + _size, // NOLINT(*-pointer-arithmetic)
                             value,
                             stl::forward<R>(args)...);
#else
        // todo: use floating point format options feom "args"
        fmt::format_to(ptr, "{}", value);
        return true;
#endif
    }

    template <typename ValueType, typename... R>
    constexpr bool append_to(istl::String auto& str, ValueType value, R&&... args) noexcept {
        using value_type = stl::remove_cvref_t<ValueType>;

        if constexpr (istl::StringViewifiable<value_type>) {
            str.append(value);
            (append_to(str, stl::forward<R>(args)), ...);
            return true;
        } else {
#ifdef __cpp_lib_to_chars
            constexpr stl::size_t value_size = sizeof(value_type);
            // converting any value type to 3 possible float value types
            using float_type =
              stl::conditional_t<(value_size <= sizeof(float)),
                                 float,
                                 stl::conditional_t<(value_size == sizeof(double)), double, long double>>;

            constexpr stl::size_t   _size = ascii::digit_count<value_type>() + 1;
            stl::array<char, _size> chars;
            if (auto res = stl::to_chars(chars.data(),
                                         chars.data() + _size,
                                         static_cast<float_type>(value), // to remove ambiguity
                                         stl::forward<R>(args)...);
                res.ec == stl::errc())
            {
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

    /**
     * This is almost has the same idea as C++23's std::string::resize_and_overwrite, but it appends the
     * string to the end of the string instead of overwriting the whole thing
     */
    template <typename Op>
    constexpr void resize_and_append(istl::String auto& out, stl::size_t counts, Op&& operation) noexcept(
      noexcept(operation(out.data()))) {
        auto const str_size = out.size();
        out.resize(out.size() + counts); // unfortunately resize writes zeros!!
        auto const ret    = stl::forward<Op>(operation)(out.data() + str_size);
        using return_type = stl::remove_cvref_t<decltype(ret)>;
        if constexpr (stl::is_integral_v<return_type>) {
            out.erase(ret);
        } else {
            out.erase(static_cast<stl::size_t>(ret - out.data()));
        }
    }

    /**
     * The purpose of this utility is to choose between these 2 types and take action accordingly:
     *   - String:       append
     *   - String View:  replace
     * This means if you `output` is a string, it'll add `input` to it, but if `output` is a string view,
     * then it'll simply put the `input` into `output`
     *
     * String Views simply don't have the ability to append something to them
     */
    template <istl::StringView StrT, typename InputStr>
        requires(!istl::String<StrT>)
    static constexpr void set_string(StrT& output, InputStr&& input) noexcept {
        output = stl::forward<InputStr>(input);
    }

    template <istl::String StrT, typename InputStr>
    static constexpr void set_string(StrT& output, InputStr&& input) {
        output += stl::forward<InputStr>(input);
    }

} // namespace webpp

#endif // WEBPP_STRINGS_APPEND_HPP
