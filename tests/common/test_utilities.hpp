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

// Build-mode selection macros:
// - If PERF_COUNTERS_FORCE_DEBUG is defined, treat as debug.
// - If PERF_COUNTERS_FORCE_RELEASE is defined, treat as release.
// - Otherwise, follow the standard NDEBUG convention: NDEBUG => release, else debug.
#    if defined(PERF_COUNTERS_FORCE_DEBUG)
#        define PERF_COUNTERS_DEBUG_BUILD 1
#    elif defined(PERF_COUNTERS_FORCE_RELEASE)
#        define PERF_COUNTERS_RELEASE_BUILD 1
#    elif defined(NDEBUG)
#        define PERF_COUNTERS_RELEASE_BUILD 1
#    else
#        define PERF_COUNTERS_DEBUG_BUILD 1
#    endif

// Thresholds controlled by build mode (macros pick one set).
#    if defined(PERF_COUNTERS_DEBUG_BUILD)
        // Debug: lenient thresholds, avoid noisy reports.
        static constexpr double PERF_TC_MIN_IPC                   = 0.10;
        static constexpr double PERF_TC_MAX_IPC                   = 8.00;
        static constexpr double PERF_TC_CACHE_MISS_THRESH         = 0.25; // 25% - very permissive
        static constexpr double PERF_TC_BRANCH_MISS_THRESH        = 0.20; // 20% - permissive
        static constexpr double PERF_TC_REF_CYC_LOW               = 0.25;
        static constexpr double PERF_TC_REF_CYC_HIGH              = 4.00;
        // Require at least this many different anomaly conditions to print in debug to avoid trivial noise.
        static constexpr int PERF_TC_DEBUG_MIN_ANOMALIES_TO_PRINT = 2;
#    else
        // Release: stricter thresholds to surface real issues.
        static constexpr double PERF_TC_MIN_IPC                   = 0.50;
        static constexpr double PERF_TC_MAX_IPC                   = 4.00;
        static constexpr double PERF_TC_CACHE_MISS_THRESH         = 0.05; // 5%
        static constexpr double PERF_TC_BRANCH_MISS_THRESH        = 0.05; // 5%
        static constexpr double PERF_TC_REF_CYC_LOW               = 0.50;
        static constexpr double PERF_TC_REF_CYC_HIGH              = 1.50;
        // Print any anomaly in release builds.
        static constexpr int PERF_TC_DEBUG_MIN_ANOMALIES_TO_PRINT = 1;
#    endif

        // Print only counters/derived metrics that are suspicious / out-of-balance.
        // This function uses simple, conservative heuristics:
        //  - IPC (instructions / cycles) outside [min_ipc, max_ipc]
        //  - Cache miss rate (cache_misses / cache_refs) above cache_miss_threshold
        //  - Branch miss rate (branch_misses / branch_instructions) above branch_miss_threshold
        //  - Inconsistent zeros (e.g., instructions > 0 but cycles == 0)
        //  - Large discrepancy between cycles and ref-cpu-cycles (ratio outside [0.5, 1.5])
        void print_anomalies(std::ostream& oss = std::cout) const noexcept {
            constexpr std::size_t IDX_CYCLES = 0;
            constexpr std::size_t IDX_INS    = 1;
            constexpr std::size_t IDX_CREF   = 2;
            constexpr std::size_t IDX_CMISS  = 3;
            constexpr std::size_t IDX_BRINS  = 4;
            constexpr std::size_t IDX_BRMISS = 5;
            constexpr std::size_t IDX_REFC   = 6;

            auto const cycles = counters[IDX_CYCLES].value;
            auto const ins    = counters[IDX_INS].value;
            auto const cref   = counters[IDX_CREF].value;
            auto const cmiss  = counters[IDX_CMISS].value;
            auto const brins  = counters[IDX_BRINS].value;
            auto const brmiss = counters[IDX_BRMISS].value;
            auto const refc   = counters[IDX_REFC].value;

            // Quick no-data check
            bool any_nonzero = false;
            for (auto const& c : counters) {
                if (c.value != 0) {
                    any_nonzero = true;
                    break;
                }
            }
            if (!any_nonzero) {
                // oss << "No performance data collected (all counters are zero).";
                return;
            }

            int              anomaly_count = 0;
            constexpr double eps           = 1e-12;

            // 1) inconsistent zero checks
            if (cycles == 0 && ins > 0) {
                oss << "[ANOMALY] instructions > 0 but CPU cycles == 0 (ins=" << ins << ", cycles=" << cycles << ")";
                ++anomaly_count;
            }
            if (ins == 0 && cycles > 0) {
                oss << "[ANOMALY] CPU cycles > 0 but instructions == 0 (cycles=" << cycles << ", ins=" << ins << ")";
                ++anomaly_count;
            }

            // 2) IPC range check
            if (cycles > 0 && ins > 0) {
                double const ipc = static_cast<double>(ins) / static_cast<double>(cycles);
                if (!(ipc >= PERF_TC_MIN_IPC && ipc <= PERF_TC_MAX_IPC)) {
                    oss << "[ANOMALY] IPC out of range: " << ipc << " (ins=" << ins << ", cycles=" << cycles << ")";
                    ++anomaly_count;
                }
            }

            // 3) Cache miss rate
            if (cref == 0 && cmiss > 0) {
                oss << "[ANOMALY] cache_misses > 0 but cache_references == 0 (cmiss=" << cmiss << ", cref=" << cref
                    << ")";
                ++anomaly_count;
            } else if (cref > 0) {
                double const miss_rate = static_cast<double>(cmiss) / static_cast<double>(cref);
                if (miss_rate > (PERF_TC_CACHE_MISS_THRESH + eps)) {
                    oss << "[ANOMALY] High cache miss rate: " << (miss_rate * 100.0) << "% (misses=" << cmiss
                        << ", refs=" << cref << ")";
                    ++anomaly_count;
                }
            }

            // 4) Branch miss rate
            if (brins == 0 && brmiss > 0) {
                oss << "[ANOMALY] branch_misses > 0 but branch_instructions == 0 (brmiss=" << brmiss
                    << ", brins=" << brins << ")";
                ++anomaly_count;
            } else if (brins > 0) {
                double const br_miss_rate = static_cast<double>(brmiss) / static_cast<double>(brins);
                if (br_miss_rate > (PERF_TC_BRANCH_MISS_THRESH + eps)) {
                    oss << "[ANOMALY] High branch miss rate: " << (br_miss_rate * 100.0) << "% (misses=" << brmiss
                        << ", branch_inst=" << brins << ")";
                    ++anomaly_count;
                }
            }

            // 5) cycles vs ref-cpu-cycles discrepancy
            if (refc > 0 && cycles > 0) {
                double const ratio = static_cast<double>(cycles) / static_cast<double>(refc);
                if (!(ratio >= PERF_TC_REF_CYC_LOW && ratio <= PERF_TC_REF_CYC_HIGH)) {
                    oss << "[ANOMALY] cycles vs ref-cpu-cycles ratio suspicious: " << ratio << " (cycles=" << cycles
                        << ", ref_cycles=" << refc << ")";
                    ++anomaly_count;
                }
            }

            // 6) member zero while leader nonzero (quick heuristic)
            for (std::size_t i = 0; i < counters.size(); ++i) {
                if (i != IDX_CYCLES && counters[IDX_CYCLES].value > 0 && counters[i].value == 0) {
                    oss << "[ANOMALY] '" << counters[i].name << "' is zero while CPU Cycles > 0";
                    ++anomaly_count;
                }
            }

            // Decide whether to actually print summary vs being quiet (debug builds prefer being quiet).
#    if defined(PERF_COUNTERS_DEBUG_BUILD)
            const int required = PERF_TC_DEBUG_MIN_ANOMALIES_TO_PRINT;
#    else
            const int required = PERF_TC_DEBUG_MIN_ANOMALIES_TO_PRINT;
#    endif

            if (anomaly_count < required) {
                // In debug we avoid printing trivial results; in release required==1 so this will not suppress.
                // oss << "No significant anomalies detected (anomaly_count=" << anomaly_count << ").";
            }
        }

        void print_verbose(std::ostream& oss = std::cout) const noexcept {
            for (auto const& info : counters) {
                oss << std::setw(25) << std::left << info.name << " : " << info.value;
            }
        }
    };

#endif // __linux__


} // namespace testing

#endif // WEBPP_TESTS_TEST_UTILITIES_HPP
