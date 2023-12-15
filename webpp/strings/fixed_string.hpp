#ifndef WEBPP_FIXED_STRING_H
#define WEBPP_FIXED_STRING_H

#if __has_include(<fixed_string.hpp>)
#    include <fixed_string.hpp>

namespace webpp::istl {
    using namespace fixstr;
}
#else



#    include "../std/string_view.hpp"

#    include <cstddef>
#    include <cstdint>
#    include <utility>

#    if !((__cpp_nontype_template_parameter_class || (__cpp_nontype_template_args >= 201'911L)))
#        warning \
          "Your compiler doesn't support non-type template parameters. Some compile-time features may not work"
#    else
#        define FIXED_STRING_SUPPORT
#    endif

namespace webpp {

    // NOLINTBEGIN(*-magic-numbers)

    struct length_value_t {
        uint32_t value;
        uint8_t  length;
    };

    constexpr length_value_t length_and_value_of_utf8_code_point(uint8_t const first_unit) noexcept {
        if ((first_unit & 0b1000'0000) == 0b0000'0000) {
            return {static_cast<uint32_t>(first_unit), 1};
        }
        if ((first_unit & 0b1110'0000) == 0b1100'0000) {
            return {static_cast<uint32_t>(first_unit & 0b0001'1111), 2};
        }
        if ((first_unit & 0b1111'0000) == 0b1110'0000) {
            return {static_cast<uint32_t>(first_unit & 0b0000'1111), 3};
        }
        if ((first_unit & 0b1111'1000) == 0b1111'0000) {
            return {static_cast<uint32_t>(first_unit & 0b0000'0111), 4};
        }
        if ((first_unit & 0b1111'1100) == 0b1111'1000) {
            return {static_cast<uint32_t>(first_unit & 0b0000'0011), 5};
        }
        if ((first_unit & 0b1111'1100) == 0b1111'1100) {
            return {static_cast<uint32_t>(first_unit & 0b0000'0001), 6};
        }
        return {0, 0};
    }

    constexpr char32_t value_of_trailing_utf8_code_point(uint8_t const unit, bool& correct) noexcept {
        if ((unit & 0b1100'0000) == 0b1000'0000) {
            return unit & 0b0011'1111;
        }
        correct = false;
        return 0;
    }

    constexpr length_value_t length_and_value_of_utf16_code_point(uint16_t const first_unit) noexcept {
        if ((first_unit & 0b1111'1100'0000'0000) == 0b1101'1000'0000'0000) {
            return {static_cast<uint32_t>(first_unit & 0b0000'0011'1111'1111), 2};
        }
        return {first_unit, 1};
    }

    template <size_t N>
    struct fixed_string {
        char32_t content[N]{};
        size_t   real_size{0};
        bool     correct_flag{true};

        template <typename T>
        explicit constexpr fixed_string(T const (&input)[N + 1]) noexcept {
            if constexpr (stl::is_same_v<T, char>) {
#    ifdef WEBPP_STRING_IS_UTF8
                size_t out{0};
                for (size_t i{0}; i < N; ++i) {
                    if ((i == (N - 1)) && (input[i] == 0)) {
                        break;
                    }
                    length_value_t info = length_and_value_of_utf8_code_point(input[i]);
                    switch (info.length) {
                        case 6:
                            if (++i < N) {
                                info.value = (info.value << 6) |
                                             value_of_trailing_utf8_code_point(input[i], correct_flag);
                            }
                            [[fallthrough]];
                        case 5:
                            if (++i < N) {
                                info.value = (info.value << 6) |
                                             value_of_trailing_utf8_code_point(input[i], correct_flag);
                            }
                            [[fallthrough]];
                        case 4:
                            if (++i < N) {
                                info.value = (info.value << 6) |
                                             value_of_trailing_utf8_code_point(input[i], correct_flag);
                            }
                            [[fallthrough]];
                        case 3:
                            if (++i < N) {
                                info.value = (info.value << 6) |
                                             value_of_trailing_utf8_code_point(input[i], correct_flag);
                            }
                            [[fallthrough]];
                        case 2:
                            if (++i < N) {
                                info.value = (info.value << 6) |
                                             value_of_trailing_utf8_code_point(input[i], correct_flag);
                            }
                            [[fallthrough]];
                        case 1:
                            content[out++] = static_cast<char32_t>(info.value);
                            real_size++;
                            break;
                        default: correct_flag = false; return;
                    }
                }
#    else
                for (size_t i{0}; i < N; ++i) {
                    content[i] = static_cast<uint8_t>(input[i]);
                    if ((i == (N - 1)) && (input[i] == 0)) {
                        break;
                    }
                    real_size++;
                }
#    endif
#    ifdef __cpp_char8_t
            } else if constexpr (stl::is_same_v<T, char8_t>) {
                size_t out{0};
                for (size_t i{0}; i < N; ++i) {
                    if ((i == (N - 1)) && (input[i] == 0)) {
                        break;
                    }
                    length_value_t info = length_and_value_of_utf8_code_point(input[i]);
                    switch (info.length) {
                        case 6:
                            if (++i < N) {
                                info.value = (info.value << 6) |
                                             value_of_trailing_utf8_code_point(input[i], correct_flag);
                            }
                            [[fallthrough]];
                        case 5:
                            if (++i < N) {
                                info.value = (info.value << 6) |
                                             value_of_trailing_utf8_code_point(input[i], correct_flag);
                            }
                            [[fallthrough]];
                        case 4:
                            if (++i < N) {
                                info.value = (info.value << 6) |
                                             value_of_trailing_utf8_code_point(input[i], correct_flag);
                            }
                            [[fallthrough]];
                        case 3:
                            if (++i < N) {
                                info.value = (info.value << 6) |
                                             value_of_trailing_utf8_code_point(input[i], correct_flag);
                            }
                            [[fallthrough]];
                        case 2:
                            if (++i < N) {
                                info.value = (info.value << 6) |
                                             value_of_trailing_utf8_code_point(input[i], correct_flag);
                            }
                            [[fallthrough]];
                        case 1:
                            content[out++] = static_cast<char32_t>(info.value);
                            real_size++;
                            break;
                        default: correct_flag = false; return;
                    }
                }
#    endif
            } else if constexpr (stl::is_same_v<T, char16_t>) {
                size_t out{0};
                for (size_t i{0}; i < N; ++i) {
                    length_value_t info = length_and_value_of_utf16_code_point(input[i]);
                    if (info.length == 2) {
                        if (++i < N) {
                            if ((input[i] & 0b1111'1100'0000'0000) == 0b1101'1100'0000'0000) {
                                content[out++] = (info.value << 10) | (input[i] & 0b0000'0011'1111'1111);
                            } else {
                                correct_flag = false;
                                break;
                            }
                        }
                    } else {
                        if ((i == (N - 1)) && (input[i] == 0)) {
                            break;
                        }
                        content[out++] = info.value;
                    }
                }
                real_size = out;
            } else if constexpr (stl::is_same_v<T, wchar_t> || stl::is_same_v<T, char32_t>) {
                for (size_t i{0}; i < N; ++i) {
                    content[i] = input[i];
                    if ((i == (N - 1)) && (input[i] == 0)) {
                        break;
                    }
                    real_size++;
                }
            }
        }

        constexpr fixed_string(fixed_string const& other) noexcept {
            for (size_t i{0}; i < N; ++i) {
                content[i] = other.content[i];
            }
            real_size    = other.real_size;
            correct_flag = other.correct_flag;
        }

        [[nodiscard]] constexpr bool correct() const noexcept {
            return correct_flag;
        }

        [[nodiscard]] constexpr size_t size() const noexcept {
            return real_size;
        }

        [[nodiscard]] constexpr char32_t* begin() const noexcept {
            return content;
        }

        [[nodiscard]] constexpr char32_t* end() const noexcept {
            return content + size();
        }

        constexpr char32_t operator[](size_t i) const noexcept {
            return content[i];
        }

        template <size_t M>
        [[nodiscard]] constexpr bool is_ss(fixed_string<M> const& rhs) const noexcept {
            if (real_size != rhs.size()) {
                return false;
            }
            for (size_t i{0}; i != real_size; ++i) {
                if (content[i] != rhs[i]) {
                    return false;
                }
            }
            return true;
        }

        [[nodiscard]] explicit constexpr operator stl::basic_string_view<char32_t>() const noexcept {
            return stl::basic_string_view<char32_t>{content, size()};
        }
    };

    template <>
    struct fixed_string<0> {
      private:
        static constexpr char32_t empty[1]{0};

      public:
        template <typename T>
        explicit constexpr fixed_string(T const*) noexcept {}

        explicit constexpr fixed_string(stl::initializer_list<char32_t>) noexcept {}

        constexpr fixed_string(fixed_string const&) noexcept {}

        [[nodiscard]] constexpr bool correct() const noexcept {
            return true;
        }

        [[nodiscard]] constexpr size_t size() const noexcept {
            return 0;
        }

        [[nodiscard]] constexpr char32_t* begin() const noexcept {
            return empty;
        }

        [[nodiscard]] constexpr char32_t* end() const noexcept {
            return empty + size();
        }

        constexpr char32_t operator[](size_t) const noexcept {
            return 0;
        }

        [[nodiscard]] explicit constexpr operator stl::basic_string_view<char32_t>() const noexcept {
            return stl::basic_string_view<char32_t>{empty, 0};
        }
    };

    template <typename CharT, size_t N>
    fixed_string(CharT const (&)[N]) -> fixed_string<N - 1>;

    template <size_t N>
    fixed_string(fixed_string<N>) -> fixed_string<N>;

    // NOLINTEND(*-magic-numbers)

} // namespace webpp


#endif // has_include

#endif // WEBPP_FIXED_STRING_H
