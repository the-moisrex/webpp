#ifndef WEBPP_TESTS_TEST_UTILITIES_HPP
#define WEBPP_TESTS_TEST_UTILITIES_HPP

#include <version>
#if defined(__cpp_lib_expected) && !defined(WEBPP_EXPECTED_IMPL)
#    define WEBPP_EXPECTED_IMPL
#    include <expected>
#endif

#include <array>
#include <cctype>
#include <chrono>
#include <cmath>
#include <concepts>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <optional>
#include <sstream>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <typeinfo>
#include <utility>

#ifdef __GNUG__
#    include <cxxabi.h>
#endif

#ifdef __linux__
#    include <asm/unistd.h>
#    include <linux/perf_event.h>
#    include <sys/ioctl.h>
#    include <unistd.h>
#endif

namespace testing {

    // -------------------- Utilities --------------------
    inline std::string demangle(std::string_view name) {
#ifdef __GNUG__
        int status = 0;
        // NOLINTNEXTLINE(*-data-usage)
        char* dem  = abi::__cxa_demangle(name.data(), nullptr, nullptr, &status);
        if (dem != nullptr) {
            std::string copy(dem);
            ::free(dem); // NOLINT(*-malloc, *-memory)
            return copy;
        }
        return std::string(name);
#else
        return std::string(name);
#endif
    }

    template <typename T>
    std::string type_name() {
        return demangle(typeid(T).name());
    }

    // Replace all occurrences of `from` with `to` in-place
    static void replace_all(std::string& str, std::string_view const from, std::string_view const to_str) {
        auto const  fromLen = from.size();
        auto const  toLen   = to_str.size();
        std::size_t pos     = 0;

        while ((pos = str.find(from, pos)) != std::string::npos) {
            str.replace(pos, fromLen, to_str);
            pos += toLen;
        }
    }

    inline std::string cleanup_typename(std::string typeName) {
        // 1. Remove known useless namespace prefixes
        static std::array<std::string_view, 3> builtin_namespaces{
          {"std::__cxx11::", "std::", "webpp::"}
        };
        for (auto const ns_str : builtin_namespaces) {
            replace_all(typeName, ns_str, "");
        }

        // 2. Replace common verbose STL template types with shorthand
        // --- strings ---
        replace_all(typeName, "basic_string<char, char_traits<char>, allocator<char> >", "string");
        replace_all(typeName, "basic_string<wchar_t, char_traits<wchar_t>, allocator<wchar_t> >", "wstring");
        replace_all(typeName, "basic_string<char8_t, char_traits<char8_t>, allocator<char8_t> >", "u8string");
        replace_all(typeName, "basic_string<char16_t, char_traits<char16_t>, allocator<char16_t> >", "u16string");
        replace_all(typeName, "basic_string<char32_t, char_traits<char32_t>, allocator<char32_t> >", "u32string");

        // --- string_views ---
        replace_all(typeName, "basic_string_view<char, char_traits<char> >", "string_view");
        replace_all(typeName, "basic_string_view<wchar_t, char_traits<wchar_t> >", "wstring_view");
        replace_all(typeName, "basic_string_view<char8_t, char_traits<char8_t> >", "u8string_view");
        replace_all(typeName, "basic_string_view<char16_t, char_traits<char16_t> >", "u16string_view");
        replace_all(typeName, "basic_string_view<char32_t, char_traits<char32_t> >", "u32string_view");

        // --- stringstream/iostream types ---
        replace_all(typeName, "basic_ostringstream<char, char_traits<char>, allocator<char> >", "ostringstream");
        replace_all(typeName, "basic_istringstream<char, char_traits<char>, allocator<char> >", "istringstream");
        replace_all(typeName, "basic_stringstream<char, char_traits<char>, allocator<char> >", "stringstream");
        replace_all(typeName, "basic_ostream<char, char_traits<char> >", "ostream");
        replace_all(typeName, "basic_istream<char, char_traits<char> >", "istream");

        // --- common containers (if you allow) ---
        replace_all(typeName, "std::vector<", "vector<");
        replace_all(typeName, "std::optional<", "optional<");
        replace_all(typeName, "std::unique_ptr<", "unique_ptr<");
        replace_all(typeName, "std::shared_ptr<", "shared_ptr<");

        // 3. Clean up simple whitespace (optional, minimal)
        while (!typeName.empty() && typeName.front() == ' ') {
            typeName.erase(typeName.begin());
        }
        while (!typeName.empty() && typeName.back() == ' ') {
            typeName.pop_back();
        }

        return typeName;
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
    inline constexpr bool is_standard_integer_v = is_standard_integer<std::remove_cvref_t<T>>::value;

    template <typename T, typename U>
    inline constexpr bool both_standard_integers_v = is_standard_integer_v<T> && is_standard_integer_v<U>;

    // -------------------- Helper --------------------
    template <typename L, typename R, typename Op>
    constexpr bool safe_compare_op(L const& lhs, R const& rhs, Op&& opr) noexcept {
        if constexpr (!both_standard_integers_v<L, R> && std::is_arithmetic_v<L> && std::is_arithmetic_v<R>) {
            // Safe numeric fallback for char32_t, float, etc.
            using Common = std::common_type_t<L, R>;
            return std::forward<Op>(opr)(static_cast<Common>(lhs), static_cast<Common>(rhs));
        } else {
            // Generic types (strings, etc.)
            return std::forward<Op>(opr)(lhs, rhs);
        }
    }

    // -------------------- Comparison Functors --------------------

    struct cmp_equal {
        template <typename L, typename R>
        constexpr bool operator()(L const& lhs, R const& rhs) const noexcept {
            return safe_compare_op(lhs, rhs, [](auto const& left, auto const& right) {
                if constexpr (both_standard_integers_v<decltype(left), decltype(right)>) {
                    return std::cmp_equal(left, right);
                } else {
                    return left == right;
                }
            });
        }
    };

    struct cmp_not_equal {
        template <typename L, typename R>
        constexpr bool operator()(L const& lhs, R const& rhs) const noexcept {
            return safe_compare_op(lhs, rhs, [](auto const& left, auto const& right) {
                if constexpr (both_standard_integers_v<decltype(left), decltype(right)>) {
                    return std::cmp_not_equal(left, right);
                } else {
                    return left != right;
                }
            });
        }
    };

    struct cmp_less {
        template <typename L, typename R>
        constexpr bool operator()(L const& lhs, R const& rhs) const noexcept {
            return safe_compare_op(lhs, rhs, [](auto const& left, auto const& right) {
                if constexpr (both_standard_integers_v<decltype(left), decltype(right)>) {
                    return std::cmp_less(left, right);
                } else {
                    return left < right;
                }
            });
        }
    };

    struct cmp_greater {
        template <typename L, typename R>
        constexpr bool operator()(L const& lhs, R const& rhs) const noexcept {
            return safe_compare_op(lhs, rhs, [](auto const& left, auto const& right) {
                if constexpr (both_standard_integers_v<decltype(left), decltype(right)>) {
                    return std::cmp_greater(left, right);
                } else {
                    return left > right;
                }
            });
        }
    };

    struct cmp_less_equal {
        template <typename L, typename R>
        constexpr bool operator()(L const& lhs, R const& rhs) const noexcept {
            return safe_compare_op(lhs, rhs, [](auto const& left, auto const& right) {
                if constexpr (both_standard_integers_v<decltype(left), decltype(right)>) {
                    return std::cmp_less_equal(left, right);
                } else {
                    return left <= right;
                }
            });
        }
    };

    struct cmp_greater_equal {
        template <typename L, typename R>
        constexpr bool operator()(L const& lhs, R const& rhs) const noexcept {
            return safe_compare_op(lhs, rhs, [](auto const& left, auto const& right) {
                if constexpr (both_standard_integers_v<decltype(left), decltype(right)>) {
                    return std::cmp_greater_equal(left, right);
                } else {
                    return left >= right;
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

    inline void format_duration(std::ostream& oss, std::chrono::nanoseconds const dur) {
        using namespace std::chrono;

        // Candidate units in increasing order
        struct alignas(64) Unit {
            std::string_view name;
            double           factor; // how many nanoseconds per unit
            std::string_view color;
        };

        static constexpr std::array<Unit, 6> units{
          {
           {.name = "ns", .factor = 1.0, .color = color::GREEN},
           {.name = "µs", .factor = 1'000.0, .color = color::CYAN},
           {.name = "ms", .factor = 1'000'000.0, .color = color::YELLOW},
           {.name = "s", .factor = 1'000'000'000.0, .color = color::RED},
           {.name = "min", .factor = 60.0 * 1'000'000'000.0, .color = color::RED},
           {.name = "h", .factor = 3600.0 * 1'000'000'000.0, .color = color::RED},
           }
        };

        auto        value = static_cast<double>(dur.count());
        Unit const* unit  = &units.front();

        for (auto const& cur_unit : units) {
            double const scaled = value / cur_unit.factor;
            if (scaled >= 1.0) { // switch if >=1
                unit = &cur_unit;
            }
        }
        value /= unit->factor;

        // NOLINTNEXTLINE(*-nested-*)
        oss << unit->color << "(" << std::fixed << std::setprecision(value < 10 ? 3 : (value < 100.0 ? 2 : 1)) << value
            << " " << unit->name << ")" << color::RESET;
    }


#ifdef __linux__
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
            // NOLINTBEGIN(*-vararg)
            struct perf_event_attr attr{};
            std::memset(&attr, 0, sizeof(attr));
            attr.size           = sizeof(attr);
            attr.disabled       = 1;
            attr.exclude_kernel = 1;
            attr.exclude_hv     = 1;
            attr.read_format    = PERF_FORMAT_GROUP | PERF_FORMAT_ID; // ✅ required for grouped read

            // Open group leader
            attr.type   = counters.at(0).type;
            attr.config = counters.at(0).config;
            fds_.at(0)  = static_cast<int>(syscall(__NR_perf_event_open, &attr, 0, -1, -1, 0));
            group_fd_   = fds_.at(0);

            if (group_fd_ == -1) {
                return;
            }

            // Open remaining counters in the same group (keep fds open!)
            for (std::size_t i = 1; i < counters.size(); ++i) {
                attr.type   = counters.at(i).type;
                attr.config = counters.at(i).config;
                fds_.at(i)  = static_cast<int>(syscall(__NR_perf_event_open, &attr, 0, -1, group_fd_, 0));
            }

            ioctl(group_fd_, PERF_EVENT_IOC_RESET, PERF_IOC_FLAG_GROUP);
            ioctl(group_fd_, PERF_EVENT_IOC_ENABLE, PERF_IOC_FLAG_GROUP);
            // NOLINTEND(*-vararg)
        }

        void stop() noexcept {
            // NOLINTBEGIN(*-vararg)
            if (group_fd_ == -1) {
                return;
            }

            ioctl(group_fd_, PERF_EVENT_IOC_DISABLE, PERF_IOC_FLAG_GROUP);

            struct alignas(128) {
                std::uint64_t nr;

                struct alignas(16) value_type {
                    std::uint64_t value;
                    std::uint64_t id;
                };

                std::array<value_type, num_counters> values;
            } data{};

            ssize_t const bytes = ::read(group_fd_, &data, sizeof(data));
            if (bytes <= 0) {
                return;
            }

            for (std::size_t i = 0; i < data.nr && i < counters.size(); ++i) {
                counters.at(i).value = data.values.at(i).value;
            }
            // NOLINTEND(*-vararg)
        }

        void print_short(std::ostream& oss = std::cout) const noexcept {
            bool first = true;
            for (auto const& info : counters) {
                if (!first) {
                    oss << " | ";
                }
                oss << info.name << ": " << info.value;
                first = false;
            }
        }

// Build-mode selection macros:
// - If PERF_COUNTERS_FORCE_DEBUG is defined, treat as debug.
// - If PERF_COUNTERS_FORCE_RELEASE is defined, treat as release.
// - Otherwise, follow the standard NDEBUG convention: NDEBUG => release, else debug.
#    ifdef PERF_COUNTERS_FORCE_DEBUG
#        define PERF_COUNTERS_DEBUG_BUILD 1
#    elif defined(PERF_COUNTERS_FORCE_RELEASE)
#        define PERF_COUNTERS_RELEASE_BUILD 1
#    elif defined(NDEBUG)
#        define PERF_COUNTERS_RELEASE_BUILD 1
#    else
#        define PERF_COUNTERS_DEBUG_BUILD 1
#    endif

// Thresholds controlled by build mode (macros pick one set).
#    ifdef PERF_COUNTERS_DEBUG_BUILD
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

            auto const cycles = counters.at(IDX_CYCLES).value;
            auto const ins    = counters.at(IDX_INS).value;
            auto const cref   = counters.at(IDX_CREF).value;
            auto const cmiss  = counters.at(IDX_CMISS).value;
            auto const brins  = counters.at(IDX_BRINS).value;
            auto const brmiss = counters.at(IDX_BRMISS).value;
            auto const refc   = counters.at(IDX_REFC).value;

            // Quick no-data check
            bool any_nonzero = false;
            for (auto const& info : counters) {
                if (info.value != 0) {
                    any_nonzero = true;
                    break;
                }
            }
            if (!any_nonzero) {
                // oss << "No performance data collected (all counters are zero).";
                return;
            }

            constexpr double eps = 1e-12;

            // 1) inconsistent zero checks
            if (cycles == 0 && ins > 0) {
                oss << "[ANOMALY] instructions > 0 but CPU cycles == 0 (ins=" << ins << ", cycles=" << cycles << ")";
            }
            if (ins == 0 && cycles > 0) {
                oss << "[ANOMALY] CPU cycles > 0 but instructions == 0 (cycles=" << cycles << ", ins=" << ins << ")";
            }

            // 2) IPC range check
            if (cycles > 0 && ins > 0) {
                double const ipc = static_cast<double>(ins) / static_cast<double>(cycles);
                if (ipc < PERF_TC_MIN_IPC || ipc > PERF_TC_MAX_IPC) {
                    oss << "[ANOMALY] IPC out of range: " << ipc << " (ins=" << ins << ", cycles=" << cycles << ")";
                }
            }

            // 3) Cache miss rate
            if (cref == 0 && cmiss > 0) {
                oss << "[ANOMALY] cache_misses > 0 but cache_references == 0 (cmiss=" << cmiss << ", cref=" << cref
                    << ")";
            } else if (cref > 0) {
                double const miss_rate = static_cast<double>(cmiss) / static_cast<double>(cref);
                if (miss_rate > (PERF_TC_CACHE_MISS_THRESH + eps)) {
                    oss << "[ANOMALY] High cache miss rate: " << (miss_rate * 100.0) << "% (misses=" << cmiss
                        << ", refs=" << cref << ")";
                }
            }

            // 4) Branch miss rate
            if (brins == 0 && brmiss > 0) {
                oss << "[ANOMALY] branch_misses > 0 but branch_instructions == 0 (brmiss=" << brmiss
                    << ", brins=" << brins << ")";
            } else if (brins > 0) {
                double const br_miss_rate = static_cast<double>(brmiss) / static_cast<double>(brins);
                if (br_miss_rate > (PERF_TC_BRANCH_MISS_THRESH + eps)) {
                    oss << "[ANOMALY] High branch miss rate: " << (br_miss_rate * 100.0) << "% (misses=" << brmiss
                        << ", branch_inst=" << brins << ")";
                }
            }

            // 5) cycles vs ref-cpu-cycles discrepancy
            if (refc > 0 && cycles > 0) {
                double const ratio = static_cast<double>(cycles) / static_cast<double>(refc);
                if (ratio < PERF_TC_REF_CYC_LOW || ratio > PERF_TC_REF_CYC_HIGH) {
                    oss << "[ANOMALY] cycles vs ref-cpu-cycles ratio suspicious: " << ratio << " (cycles=" << cycles
                        << ", ref_cycles=" << refc << ")";
                }
            }

            // 6) member zero while leader nonzero (quick heuristic)
            for (std::size_t i = 0; i < counters.size(); ++i) {
                if (i != IDX_CYCLES && counters.at(IDX_CYCLES).value > 0 && counters.at(i).value == 0) {
                    oss << "[ANOMALY] '" << counters.at(i).name << "' is zero while CPU Cycles > 0";
                }
            }

            // Decide whether to actually print summary vs being quiet (debug builds prefer being quiet).
            // int const required = PERF_TC_DEBUG_MIN_ANOMALIES_TO_PRINT;
            // if (anomaly_count < required) {
            //     // In debug we avoid printing trivial results; in release required==1 so this will not suppress.
            //     // oss << "No significant anomalies detected (anomaly_count=" << anomaly_count << ").";
            // }
        }

        void print_verbose(std::ostream& oss = std::cout) const noexcept {
            for (auto const& info : counters) {
                oss << std::setw(25) << std::left << info.name << " : " << info.value;
            }
        }
    };

#endif // __linux__




    namespace detail {

        // is_specialization_of helper
        template <template <typename...> class Template, typename T>
        struct is_specialization_of_impl : std::false_type {};

        template <template <typename...> class Template, typename... Args>
        struct is_specialization_of_impl<Template, Template<Args...>> : std::true_type {};

        template <template <typename...> class Template, typename T>
        constexpr bool is_specialization_of_v = is_specialization_of_impl<Template, std::remove_cvref_t<T>>::value;

        // Detect free to_string via ADL, but exclude arithmetic and string-like types so we don't override
        // numbers/strings.
        template <typename T>
        concept StringLike = std::is_convertible_v<T, std::string_view>;

        template <typename T>
        concept CharLike =
          std::same_as<std::remove_cv_t<T>, char> || std::same_as<std::remove_cv_t<T>, unsigned char> ||
          std::same_as<std::remove_cv_t<T>, signed char> || std::same_as<std::remove_cv_t<T>, char16_t> ||
          std::same_as<std::remove_cv_t<T>, char8_t> || std::same_as<std::remove_cv_t<T>, char32_t> ||
          std::same_as<std::remove_cv_t<T>, wchar_t>;

        // arrays of character types (e.g. char32_t[4])
        template <typename T>
        concept CharArrayLike = std::is_array_v<T> && CharLike<std::remove_all_extents_t<T>>;

        // Free to_string via ADL but exclude arithmetic and string-like
        template <typename T>
        concept HasFreeToString = (!std::is_arithmetic_v<T> && !StringLike<T>) && requires(T const& value) {
            { to_string(value) } -> std::convertible_to<std::string>;
        };

        // Streamable
        template <typename T>
        concept Streamable = requires(std::ostream& out, T const& value) {
            { out << value } -> std::same_as<std::ostream&>;
        };

        // Iterable (has begin/end)
        template <typename T>
        concept Iterable = requires(T obj) {
            std::begin(obj);
            std::end(obj);
        };

        // Safely detect tuple_size existence without instantiating tuple_size_v unguarded
        template <typename T>
        concept HasTupleSize = requires { typename std::tuple_size<T>::type; };

        // optional-like detection
        template <typename T>
        concept OptionalLike = is_specialization_of_v<std::optional, T>;

#ifdef __cpp_lib_expected
        template <typename T>
        concept ExpectedLike = is_specialization_of_v<std::expected, T>;
#else
        template <typename T>
        concept ExpectedLike = false;
#endif

        // helpers for escaping
        inline std::string hex_u32(uint32_t const uch, int const digits) {
            std::ostringstream oss;
            oss << std::hex << std::uppercase << std::setfill('0') << std::setw(digits)
                << (uch & ((digits == 8) ? 0xFFFF'FFFFU : (digits == 4 ? 0xFFFFU : 0xFFU))); // NOLINT(*-nested-*)
            return oss.str();
        }

        template <typename C>
        void escape_codepoint(std::ostringstream& oss, C const inp) {
            if (inp == 0) {
                oss << "\\0";
                return;
            }
            // produce either printable char (if ascii printable) or an escape (\x, \u, \U)
            if constexpr (sizeof(C) == 1) {
                auto const uch = static_cast<unsigned char>(inp);
                if (std::isprint(uch)) {
                    oss << static_cast<char>(uch);
                    return;
                }
                oss << "\\x" << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(uch);
            } else if constexpr (sizeof(C) == 2) {
                // 16-bit: \uXXXX
                auto const uch = static_cast<std::uint16_t>(inp);
                if (uch <= 0x7F && std::isprint(static_cast<int>(uch))) {
                    oss << static_cast<char>(uch);
                    return;
                }
                oss << "\\u" << hex_u32(uch, 4);

            } else { // sizeof >= 4
                auto const uch = static_cast<uint32_t>(inp);
                if (uch <= 0x7F && std::isprint(static_cast<int>(uch))) {
                    oss << static_cast<char>(uch);
                    return;
                }
                oss << "\\U" << hex_u32(uch, 8);
            }
        }

        template <typename C>
        char const* char_literal_prefix() {
            if constexpr (std::same_as<std::remove_cv_t<C>, char>) {
                return "";
            }
            if constexpr (std::same_as<std::remove_cv_t<C>, char16_t>) {
                return "u";
            }
            if constexpr (std::same_as<std::remove_cv_t<C>, char32_t>) {
                return "U";
            }
            if constexpr (std::same_as<std::remove_cv_t<C>, wchar_t>) {
                return "L";
            }
            return "";
        }

    } // namespace detail

    template <typename T>
    [[nodiscard]] std::string serialize(T const& value) {
        using U = std::remove_cvref_t<T>;

        // demangle the static type once and prefix with color
        std::string demangled_type = cleanup_typename(type_name<U>());
        std::string type_prefix    = std::string(color::GREY) + demangled_type + std::string(color::RESET) + " ";

        // 1) ADL to_string for non-arithmetic non-stringlike types
        if constexpr (detail::HasFreeToString<U>) {
            return type_prefix + to_string(value);
        } else if constexpr (std::same_as<U, bool>) {
            return type_prefix + (value ? "true" : "false");
        } else if constexpr (detail::CharArrayLike<U>) {
            // 2) char-array-like (e.g. char[N], char16_t[N], char32_t[N]) -> treat as string of codepoints
            using Elem                = std::remove_all_extents_t<U>;
            constexpr auto     Length = std::extent_v<U>;
            std::ostringstream oss;
            oss << detail::char_literal_prefix<Elem>() << '"';
            for (std::size_t i = 0; i < Length; ++i) {
                detail::escape_codepoint(oss, value[i]);
            }
            oss << '"';
            return type_prefix + oss.str();
        } else if constexpr (detail::StringLike<U>) {
            // 3) string-like (std::string, std::string_view, C-style char const*)
            std::ostringstream oss;
            oss << '"';
            for (auto const uch : std::string_view{value}) {
                if (std::isprint(uch)) {
                    oss << static_cast<char>(uch);
                } else {
                    oss << "\\x" << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(uch);
                    oss << std::dec << std::setfill(' ');
                }
            }
            oss << '"';
            return type_prefix + oss.str();
        } else if constexpr (detail::CharLike<U> && !std::is_array_v<U>) {
            // 4) single character types (char, char16_t, char32_t, wchar_t, signed/unsigned char)
            std::ostringstream oss;
            char const*        pfx = detail::char_literal_prefix<U>();
            oss << pfx;
            // printable only reliably works for ASCII; for non-ascii we'll use escapes
            if constexpr (sizeof(U) == 1) {
                auto const uch = static_cast<unsigned char>(value);
                if (std::isprint(uch)) {
                    oss << '\'' << static_cast<char>(value) << '\'';
                } else {
                    oss << "'\\x" << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(uch) << '\'';
                }
            } else if constexpr (sizeof(U) == 2) {
                auto const uch = static_cast<uint16_t>(value);
                if (uch <= 0x7F && std::isprint(static_cast<int>(uch))) {
                    oss << '\'' << static_cast<char>(uch) << '\'';
                } else {
                    oss << "'\\u" << detail::hex_u32(uch, 4) << '\'';
                }
            } else {
                auto const uch = static_cast<uint32_t>(value);
                if (uch <= 0x7F && std::isprint(static_cast<int>(uch))) {
                    oss << '\'' << static_cast<char>(uch) << '\'';
                } else {
                    oss << "'\\U" << detail::hex_u32(uch, 8) << '\'';
                }
            }
            return type_prefix + oss.str();
        } else if constexpr (std::is_pointer_v<U>) {
            // 5) null pointer / pointer
            if (value == nullptr) {
                return type_prefix + std::string("nullptr");
            }
            std::ostringstream oss;
            oss << "ptr(" << static_cast<void const*>(value) << ")";
            return type_prefix + oss.str();
        } else if constexpr (std::is_arithmetic_v<U>) {
            // 6) arithmetic
            std::ostringstream oss;
            if constexpr (std::is_floating_point_v<U>) {
                oss << std::setprecision(8);
            }
            oss << value;
            return type_prefix + oss.str();
        } else if constexpr (std::is_enum_v<U>) {
            // 7) enum
            using EU = std::underlying_type_t<U>;
            std::ostringstream oss;
            oss << static_cast<EU>(value);
            return type_prefix + oss.str();
        } else if constexpr (detail::OptionalLike<U>) {
            // 8) optional
            if (value.has_value()) {
                return type_prefix + std::string("optional(") + serialize(*value) + ")";
            }
            return type_prefix + std::string("nullopt");

        }
#ifdef __cpp_lib_expected
        else if constexpr (detail::ExpectedLike<U>)
        {
            // 9) expected (guarded by include availability)
            if (value.has_value()) {
                return type_prefix + std::string("expected(value=") + serialize(value.value()) + ")";
            }
            return type_prefix + std::string("expected(error=") + serialize(value.error()) + ")";

        }
#endif

        else if constexpr (detail::HasTupleSize<U> && requires { std::apply([](auto&&...) {}, std::declval<U>()); })
        {
            // 10) tuple-like (only when std::tuple_size<T> exists and apply works)
            std::ostringstream oss;
            oss << "(";
            bool first = true;
            std::apply(
              [&](auto const&... elems) {
                  ((oss << (first ? "" : ", ") << serialize(elems), first = false), ...);
              },
              value);
            oss << ")";
            return type_prefix + oss.str();
        } else if constexpr (detail::Iterable<U> && !detail::StringLike<U>) {
            // 11) iterable containers (but not string-like)
            std::ostringstream oss;
            oss << "{";
            bool first = true;
            for (auto const& elem : value) {
                if (!first) {
                    oss << ", ";
                }
                first = false;
                oss << serialize(elem);
            }
            oss << "}";
            return type_prefix + oss.str();
        } else if constexpr (detail::Streamable<U>) {
            // 12) streamable fallback
            std::ostringstream oss;
            oss << value;
            return type_prefix + oss.str();
        } else {
            // 13) ultimate fallback
            std::ostringstream oss;
            oss << "<" << demangled_type << "@" << &value << ">";
            return std::string(color::YELLOW) + oss.str() + std::string(color::RESET);
        }
    }

} // namespace testing

#endif // WEBPP_TESTS_TEST_UTILITIES_HPP
