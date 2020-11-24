// Created by moisrex on 9/5/20.

#ifndef WEBPP_CONSTEXPR_FORMAT_HPP
#define WEBPP_CONSTEXPR_FORMAT_HPP

#include "../std/string_view.hpp"

#include <tuple>   // size_t forward_as_tuple
#include <utility> // std::index_sequence

namespace webpp {


    constexpr auto constexpr_format(istl::StringViewifiable auto&& format_str,
                                    auto&&... args) noexcept {
        auto format_string = istl::string_viewify(format_str);
        using strv_t       = stl::remove_cvref_t<decltype(format_string)>;

        strv_t _to_str = [](auto&& arg) constexpr noexcept {
            using arg_t = stl::remove_cvref_t<decltype(arg)>;
            if constexpr (stl::is_integral_v<arg_t>) {}
        };
    }

    template <char... Chars>
    struct string {
        static constexpr char   c_str[sizeof...(Chars) + 1] = {Chars..., '\0'};
        static constexpr string instance                    = {};
        static constexpr size_t size                        = sizeof...(Chars);

        constexpr bool operator==(string<Chars...> const&) const {
            return true;
        }

        template <char... OChars>
        constexpr bool operator==(string<OChars...> const&) const {
            return false;
        }

        template <char... OChars>
        constexpr string<Chars..., OChars...> operator+(string<OChars...> const&) const {
            return {};
        }

        template <size_t Offset, size_t... Indexs>
        constexpr auto section(std::index_sequence<Indexs...>) const {
            return string<c_str[Indexs + Offset]...>{};
        }

        template <size_t From, size_t To>
        constexpr auto section() const {
            return section<From>(std::make_index_sequence<To - From>{});
        }

        constexpr char operator[](int index) const {
            return c_str[index];
        }

        template <size_t I = 0>
        constexpr bool contains(char value) const {
            if constexpr (I >= size)
                return false;
            else if (c_str[I] == value)
                return true;
            else
                return contains<I + 1>(value);
        }
    };

    template <class>
    struct is_compile_string {
        static constexpr bool value = false;
    };

    template <char... Chars>
    struct is_compile_string<string<Chars...>> {
        static constexpr bool value = true;
    };


    // string literal to string class
    template <class Char, Char... Chars>
    constexpr auto operator""_s() {
        return string<static_cast<char>(Chars)...>{};
    }

    // number to string class
    template <char... Chars>
    constexpr auto operator""_s() {
        return string<Chars...>{};
    }





    template <class String, size_t I = 0, bool started = false, int value = 0, int sign = 1>
    constexpr int atoi() {
        if constexpr (I >= String::size)
            return value * sign;
        else if constexpr (!started) {
            if constexpr (!"-0123456789"_s.contains(String::instance[I]))
                return atoi<String, I + 1, false, value, sign>();
            else if (String::instance[I] == '-')
                return atoi<String, I + 1, false, value, sign * -1>();
            else
                return atoi<String, I + 1, true, String::instance[I] - '0', sign>();
        } else if constexpr (!"0123456789"_s.contains(String::instance[I]))
            return value * sign;
        else
            return atoi<String, I + 1, true, value * 10 + String::instance[I] - '0', sign>();
    }





    template <class String>
    class format_string {
        template <size_t str_begin, size_t format_begin, size_t end, size_t index>
        struct substring {
            template <class... Args>
            static constexpr auto parse(std::tuple<Args...> const& t) {
                return String::instance.template section<str_begin, (format_begin - 1)>() +
                       std::get<index>(t);
            }
        };

        template <size_t begin, size_t end>
        static constexpr bool is_number() {
            if constexpr (begin >= end)
                return false;
            else if constexpr ("0123456789"_s.contains(String::instance[begin]))
                return true;
            else
                return is_number<begin + 1, end>();
        }

        template <size_t begin, size_t end, size_t index>
        static constexpr size_t get_index() {
            if constexpr (!is_number<begin, end>())
                return index;
            else
                return atoi<String, begin>();
        }

        template <size_t max = 0, size_t lastEnd = 0, class... Args>
        struct sublist {
            template <size_t str_begin, size_t format_begin, size_t end, size_t index>
            using push_format = sublist<(max < index ? index : max), end, Args...,
                                        substring<str_begin, format_begin, end, index>>;

            static constexpr size_t size = sizeof...(Args);
            static constexpr size_t Max  = max;
        };

        template <bool is_in = false, size_t B = 0, size_t NB = 0, size_t I = 0, size_t Index = 0,
                  class Results = sublist<>>
        static constexpr auto parser() {
            if constexpr (I >= String::size)
                return Results{};
            else {
                constexpr char c = String::instance[I];
                if constexpr (!is_in) {
                    if constexpr (c == '/')
                        return parser<false, B, B, I + 2, Index, Results>();
                    else if constexpr (c == '{')
                        return parser<true, B, I + 1, I + 1, Index, Results>();
                    else
                        return parser<false, B, B, I + 1, Index, Results>();
                } else {
                    if constexpr (c == '}')
                        return parser<
                          false, I + 1, I + 1, I + 1, Index + (is_number<NB, I>() ? 0 : 1),
                          typename Results::template push_format<B, NB, I, get_index<NB, I, Index>()>>();
                    else if constexpr (!" 0123456789"_s.contains(c))
                        return parser<false, B, B, I + 1, Index, Results>();
                    else
                        return parser<true, B, NB, I + 1, Index, Results>();
                }
            }
        }

        static constexpr auto formats = parser();

        template <size_t Max, size_t LastEnd, class... Formats, class... Args>
        constexpr auto apply_args(sublist<Max, LastEnd, Formats...>, std::tuple<Args...> const& t) const {
            return (... + Formats::parse(t)) + String::instance.template section<LastEnd + 1, String::size>();
        }

      public:
        template <class... Args>
        constexpr auto operator()(Args... args) const {
            static_assert((... && is_compile_string<Args>::value),
                          "All argument must be compile-time strings");
            static_assert(sizeof...(Args) >= formats.Max, "Not enough arguments");
            return apply_args(formats, std::forward_as_tuple(args...));
        }
    };

    template <class Char, Char... Chars>
    constexpr auto operator""_format() {
        return format_string<string<static_cast<char>(Chars)...>>{};
    }


} // namespace webpp

#endif // WEBPP_CONSTEXPR_FORMAT_HPP
