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
#    include <array>
#    include <asm/unistd.h>
#    include <iomanip>
#    include <linux/perf_event.h>
#    include <string>
#    include <sys/ioctl.h>
#    include <sys/syscall.h>
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
        constexpr std::string_view PURPLE = "\033[38;5;5m";
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
#    define WEBPP_SUPPORTS_PERF_COUNTERS

    class perf_counters {
      public:
        struct alignas(64) counter_info {
            std::string_view name;
            std::uint32_t    type;
            std::uint64_t    config;
            std::uint64_t    value{0};
        };

      private:
        static constexpr std::size_t           num_counters = 7;
        std::array<counter_info, num_counters> counters{
          {
           {.name = "CPU Cycles", .type = PERF_TYPE_HARDWARE, .config = PERF_COUNT_HW_CPU_CYCLES},
           {.name = "Instructions", .type = PERF_TYPE_HARDWARE, .config = PERF_COUNT_HW_INSTRUCTIONS},
           {.name = "Cache References", .type = PERF_TYPE_HARDWARE, .config = PERF_COUNT_HW_CACHE_REFERENCES},
           {.name = "Cache Misses", .type = PERF_TYPE_HARDWARE, .config = PERF_COUNT_HW_CACHE_MISSES},
           {.name = "Branch Instructions", .type = PERF_TYPE_HARDWARE, .config = PERF_COUNT_HW_BRANCH_INSTRUCTIONS},
           {.name = "Branch Misses", .type = PERF_TYPE_HARDWARE, .config = PERF_COUNT_HW_BRANCH_MISSES},
           {.name = "Ref CPU Cycles", .type = PERF_TYPE_HARDWARE, .config = PERF_COUNT_HW_REF_CPU_CYCLES},
           }
        };

        std::array<int, num_counters> fds_{};
        int                           group_fd_{-1};

      public:
        perf_counters() noexcept = default;

        // RAII: cleanup
        ~perf_counters() noexcept {
            if (group_fd_ != -1) {
                ::close(group_fd_);
            }
        }

        perf_counters(perf_counters const&)                = delete;
        perf_counters(perf_counters&&) noexcept            = delete;
        perf_counters& operator=(perf_counters const&)     = delete;
        perf_counters& operator=(perf_counters&&) noexcept = delete;

        [[nodiscard]] bool valid() const noexcept {
            return group_fd_ != -1;
        }

        void start() noexcept {
            struct perf_event_attr attr{};
            std::memset(&attr, 0, sizeof(attr));
            attr.size           = sizeof(attr);
            attr.disabled       = 1;
            attr.exclude_kernel = 1;
            attr.exclude_hv     = 1;
            attr.read_format    = PERF_FORMAT_GROUP | PERF_FORMAT_ID; // ✅ required for grouped read

            // Open group leader
            attr.type   = counters[0].type;
            attr.config = counters[0].config;
            fds_[0]     = static_cast<int>(syscall(__NR_perf_event_open, &attr, 0, -1, -1, 0));
            group_fd_   = fds_[0];

            if (group_fd_ == -1) {
                return;
            }

            // Open remaining counters in the same group (keep fds open!)
            for (std::size_t i = 1; i < counters.size(); ++i) {
                attr.type   = counters[i].type;
                attr.config = counters[i].config;
                fds_[i]     = static_cast<int>(syscall(__NR_perf_event_open, &attr, 0, -1, group_fd_, 0));
            }

            ioctl(group_fd_, PERF_EVENT_IOC_RESET, PERF_IOC_FLAG_GROUP);
            ioctl(group_fd_, PERF_EVENT_IOC_ENABLE, PERF_IOC_FLAG_GROUP);
        }

        void stop() noexcept {
            if (group_fd_ == -1) {
                return;
            }

            ioctl(group_fd_, PERF_EVENT_IOC_DISABLE, PERF_IOC_FLAG_GROUP);

            struct {
                std::uint64_t nr;

                struct {
                    std::uint64_t value;
                    std::uint64_t id;
                } values[num_counters];
            } data{};

            ssize_t const bytes = ::read(group_fd_, &data, sizeof(data));
            if (bytes <= 0) {
                return;
            }

            for (std::size_t i = 0; i < data.nr && i < counters.size(); ++i) {
                counters[i].value = data.values[i].value;
            }
        }

        void print_short(std::ostream& oss = std::cout) const noexcept {
            bool first = true;
            for (auto const& c : counters) {
                if (!first) {
                    oss << " | ";
                }
                oss << c.name << ": " << c.value;
                first = false;
            }
        }

        void print_verbose(std::ostream& oss = std::cout) const noexcept {
            for (auto const& info : counters) {
                oss << std::setw(25) << std::left << info.name << " : " << info.value << '\n';
            }
        }
    };

#endif // __linux__


} // namespace testing

#endif // WEBPP_TESTS_TEST_UTILITIES_HPP
