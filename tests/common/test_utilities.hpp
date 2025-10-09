#ifndef WEBPP_TESTS_TEST_UTILITIES_HPP
#define WEBPP_TESTS_TEST_UTILITIES_HPP

#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <type_traits>
#include <typeinfo>
#include <utility>

#if defined(__GNUG__)
#    include <cxxabi.h>
#endif

#if defined(__linux__)
#    include <asm/unistd.h>
#    include <linux/perf_event.h>
#    include <sys/ioctl.h>
#    include <unistd.h>
#endif

#if defined(_REENTRANT) || defined(_MT) || defined(__MT__) || defined(_PTHREADS) || defined(__STDCPP_THREADS__)
#    define WEBPP_MULTI_THREADING_ENABLED 1
#else
#    define WEBPP_MULTI_THREADING_ENABLED 0
#endif


namespace testing {

    // -------------------- Utilities --------------------
    inline std::string demangle(std::string_view name) {
#if defined(__GNUG__)
        int   status = 0;
        char* dem    = abi::__cxa_demangle(name.data(), nullptr, nullptr, &status);
        if (dem != nullptr) {
            std::string copy(dem);
            ::free(dem);
            return copy;
        }
        return std::string(name);
#else
        return std::string(name);
#endif
    }

    template <typename T>
    inline std::string type_name() {
        return demangle(typeid(T).name());
    }

    // -------------------- Traits --------------------
    // standard integer types (no bool or char-like types)
    template <typename T>
    struct is_standard_integer
      : std::bool_constant<std::is_integral_v<T> && !std::is_same_v<T, bool> && !std::is_same_v<T, char> &&
                           !std::is_same_v<T, signed char> && !std::is_same_v<T, unsigned char> &&
                           !std::is_same_v<T, wchar_t> &&
#ifdef __cpp_char8_t
                           !std::is_same_v<T, char8_t> &&
#endif
                           !std::is_same_v<T, char16_t> && !std::is_same_v<T, char32_t>> {
    };

    template <typename T>
    inline constexpr bool is_standard_integer_v = is_standard_integer<T>::value;

    template <typename T, typename U>
    inline constexpr bool both_standard_integers_v = is_standard_integer_v<T> && is_standard_integer_v<U>;

    // -------------------- Helper --------------------
    template <typename L, typename R, typename Op>
    constexpr bool safe_compare_op(L const& lhs, R const& rhs, Op&& op) noexcept {
        if constexpr (both_standard_integers_v<L, R>) {
            // Use std::cmp_* for safe mixed signedness integer comparison
            return op(lhs, rhs);
        } else if constexpr (std::is_arithmetic_v<L> && std::is_arithmetic_v<R>) {
            // Safe numeric fallback for char32_t, float, etc.
            using Common = std::common_type_t<L, R>;
            return op(static_cast<Common>(lhs), static_cast<Common>(rhs));
        } else {
            // Generic types (strings, etc.)
            return op(lhs, rhs);
        }
    }

    // -------------------- Comparison Functors --------------------

    struct cmp_equal {
        template <typename L, typename R>
        constexpr bool operator()(L const& lhs, R const& rhs) const noexcept {
            return safe_compare_op(lhs, rhs, [](auto a, auto b) {
                if constexpr (both_standard_integers_v<decltype(a), decltype(b)>) {
                    return std::cmp_equal(a, b);
                } else {
                    return a == b;
                }
            });
        }
    };

    struct cmp_not_equal {
        template <typename L, typename R>
        constexpr bool operator()(L const& lhs, R const& rhs) const noexcept {
            return safe_compare_op(lhs, rhs, [](auto a, auto b) {
                if constexpr (both_standard_integers_v<decltype(a), decltype(b)>) {
                    return std::cmp_not_equal(a, b);
                } else {
                    return a != b;
                }
            });
        }
    };

    struct cmp_less {
        template <typename L, typename R>
        constexpr bool operator()(L const& lhs, R const& rhs) const noexcept {
            return safe_compare_op(lhs, rhs, [](auto a, auto b) {
                if constexpr (both_standard_integers_v<decltype(a), decltype(b)>) {
                    return std::cmp_less(a, b);
                } else {
                    return a < b;
                }
            });
        }
    };

    struct cmp_greater {
        template <typename L, typename R>
        constexpr bool operator()(L const& lhs, R const& rhs) const noexcept {
            return safe_compare_op(lhs, rhs, [](auto a, auto b) {
                if constexpr (both_standard_integers_v<decltype(a), decltype(b)>) {
                    return std::cmp_greater(a, b);
                } else {
                    return a > b;
                }
            });
        }
    };

    struct cmp_less_equal {
        template <typename L, typename R>
        constexpr bool operator()(L const& lhs, R const& rhs) const noexcept {
            return safe_compare_op(lhs, rhs, [](auto a, auto b) {
                if constexpr (both_standard_integers_v<decltype(a), decltype(b)>) {
                    return std::cmp_less_equal(a, b);
                } else {
                    return a <= b;
                }
            });
        }
    };

    struct cmp_greater_equal {
        template <typename L, typename R>
        constexpr bool operator()(L const& lhs, R const& rhs) const noexcept {
            return safe_compare_op(lhs, rhs, [](auto a, auto b) {
                if constexpr (both_standard_integers_v<decltype(a), decltype(b)>) {
                    return std::cmp_greater_equal(a, b);
                } else {
                    return a >= b;
                }
            });
        }
    };

    // ANSI colors
    namespace color {
        constexpr std::string_view RESET  = "\033[0m";
        constexpr std::string_view RED    = "\033[31m";
        constexpr std::string_view GREEN  = "\033[32m";
        constexpr std::string_view YELLOW = "\033[33m";
        constexpr std::string_view BLUE   = "\033[34m";
        constexpr std::string_view CYAN   = "\033[36m";
        constexpr std::string_view GREY   = "\033[38;5;8m";
    } // namespace color

    // -------------------- Stream Helper --------------------
    template <typename T>
    std::string stream_to_string(T const& value) {
        std::ostringstream oss;
        oss << std::boolalpha;

        // Detect types with .data() and .size()
        if constexpr (requires {
                          value.data();
                          value.size();
                      })
        {
            using CharT = std::remove_cv_t<std::remove_pointer_t<decltype(value.data())>>;

            if constexpr (std::is_same_v<CharT, char>) {
                // Regular string-like data
                oss << std::string(value.data(), value.size());
            } else if constexpr (std::is_same_v<CharT, char8_t>) {
                // UTF-8
                oss << std::string(reinterpret_cast<char const*>(value.data()), value.size());
            } else if constexpr (std::is_same_v<CharT, char16_t> || std::is_same_v<CharT, char32_t>) {
                // UTF-16 / UTF-32: print as hex code units
                oss << "[";
                for (size_t i = 0; i < value.size(); ++i) {
                    if (i) {
                        oss << " ";
                    }
                    oss << "0x" << std::hex << std::uppercase << static_cast<uint32_t>(value[i]) << std::dec;
                }
                oss << "]";
            } else {
                // Generic container of streamable elements
                oss << "[";
                for (size_t i = 0; i < value.size(); ++i) {
                    if (i) {
                        oss << ", ";
                    }
                    if constexpr (requires { oss << value[i]; }) {
                        oss << value[i];
                    } else {
                        oss << "?";
                    }
                }
                oss << "]";
            }

        } else if constexpr (std::is_same_v<T, char8_t const*> || std::is_same_v<T, char8_t*>) {
            oss << reinterpret_cast<char const*>(value);
        } else if constexpr (std::is_same_v<T, std::u8string> || std::is_same_v<T, std::u8string_view>) {
            oss << std::string(reinterpret_cast<std::string_view>(value.data()), value.size());
        } else if constexpr (std::is_same_v<T, std::u16string> || std::is_same_v<T, std::u16string_view> ||
                             std::is_same_v<T, std::u32string> || std::is_same_v<T, std::u32string_view>)
        {
            // Print UTF-16/32 string as hex sequence
            oss << "[";
            for (auto ch_val : value) {
                oss << "0x" << std::hex << std::uppercase << static_cast<uint32_t>(ch_val) << " ";
            }
            oss << "]";
        } else if constexpr (requires { oss << value; }) {
            // Fallback: streamable type
            oss << value;
        } else {
            // Final fallback: unprintable type
            oss << "<unprintable type: " << typeid(T).name() << ">";
        }

        return oss.str();
    }

    inline std::string format_duration(std::chrono::nanoseconds dur) {
        using namespace std::chrono;

        // Candidate units in increasing order
        struct alignas(64) Unit {
            std::string_view name;
            double           factor; // how many nanoseconds per unit
            std::string_view color;
        };

        static constexpr std::array<Unit, 6> units{
          Unit{ .name = "ns",                      .factor = 1.0,  .color = color::GREEN},
          { .name = "µs",                  .factor = 1'000.0,   .color = color::CYAN},
          { .name = "ms",              .factor = 1'000'000.0, .color = color::YELLOW},
          {  .name = "s",          .factor = 1'000'000'000.0,    .color = color::RED},
          {.name = "min",   .factor = 60.0 * 1'000'000'000.0,    .color = color::RED},
          {  .name = "h", .factor = 3600.0 * 1'000'000'000.0,    .color = color::RED},
        };

        auto value = static_cast<double>(dur.count());
        auto unit  = units.begin();

        for (auto& cur_unit : units) {
            double const val = value / cur_unit.factor;
            if (std::fabs(val) < 1.0) {
                break; // too small to switch to this unit
            }
            value = val;
            unit  = &cur_unit;
        }

        std::ostringstream oss;
        oss << unit->color << "(" << std::fixed << std::setprecision(value < 10 ? 3 : (value < 100.0 ? 2 : 1)) << value
            << " " << unit->name << ")" << color::RESET;
        return oss.str();
    }


#if defined(__linux__)
    struct perf_counter {
        int      fd{-1};
        uint64_t start_value{0};

        perf_counter() {
            struct perf_event_attr pe{};
            std::memset(&pe, 0, sizeof(pe));
            pe.type           = PERF_TYPE_HARDWARE;
            pe.size           = sizeof(pe);
            pe.config         = PERF_COUNT_HW_INSTRUCTIONS;
            pe.disabled       = 1;
            pe.exclude_kernel = 1;
            pe.exclude_hv     = 1;

            fd = static_cast<int>(syscall(__NR_perf_event_open, &pe, 0, -1, -1, 0));
            if (fd == -1) {
                return; // fallback to disabled state
            }

            ioctl(fd, PERF_EVENT_IOC_RESET, 0);
            ioctl(fd, PERF_EVENT_IOC_ENABLE, 0);
            read(fd, &start_value, sizeof(start_value));
        }

        uint64_t stop() {
            if (fd == -1) {
                return 0;
            }
            uint64_t end_value = 0;
            ioctl(fd, PERF_EVENT_IOC_DISABLE, 0);
            read(fd, &end_value, sizeof(end_value));
            close(fd);
            return end_value - start_value;
        }

        ~perf_counter() {
            if (fd != -1) {
                close(fd);
            }
        }
    };
#endif


} // namespace testing

#endif // WEBPP_TESTS_TEST_UTILITIES_HPP
