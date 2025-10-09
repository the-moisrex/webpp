#ifndef WEBPP_TEST_IMPL_HPP
#define WEBPP_TEST_IMPL_HPP

#include "./test_utilities.hpp"

#include <atomic>
#include <chrono>
#include <iostream>
#include <ostream>
#include <sstream>
#include <string>
#include <string_view>
#include <tuple>
#include <utility>

namespace testing {

    // -------------------- Test Registry --------------------
    struct alignas(64) test_info {
        using func_type = void (*)();

        std::string_view         suite;
        std::string_view         name;
        func_type                func{};
        std::chrono::nanoseconds duration_ns{};
        bool                     failed = false;
    };

    // -------------------- Assertion infra --------------------
    struct alignas(128) assert_context {
        std::string_view file;
        int              line = 0;
        std::string      expr1;
        std::string      expr2;
        std::string_view macro_name;
        std::string      extra;
        bool             success = true;
    };

    inline test_info& current_test() {
        static test_info test;
        return test;
    }

    [[nodiscard]] inline std::string message(assert_context const& ctx) {
        std::ostringstream oss;
        if (!ctx.success) {
            oss << color::RED << ctx.macro_name << " failed: " << color::RESET;
            oss << ctx.expr1;
            if (!ctx.expr2.empty()) {
                oss << " vs " << ctx.expr2;
            }
            if (!ctx.extra.empty()) {
                oss << "\n  " << ctx.extra;
            }
            oss << "\n  (" << ctx.file << ":" << ctx.line << ")";
        } else {
            oss << color::GREEN << ctx.macro_name << " OK: " << color::RESET << ctx.expr1;
            if (!ctx.expr2.empty()) {
                oss << " vs " << ctx.expr2;
            }
        }
        return oss.str();
    }

    struct registry {
        using func_type = test_info::func_type;
#if WEBPP_MULTI_THREADING_ENABLED
        using atomic_int_type = std::atomic<int>;
#else
        using atomic_int_type = int;
#endif

        static registry& instance() {
            static registry reg{};
            return reg;
        }

        void register_test(std::string_view suite, std::string_view name, func_type func) {
            all_tests.emplace_back(suite, name, func);
        }

        std::vector<test_info>& tests() {
            return all_tests;
        }

        void asserted(bool status) {
            ++m_assertions;
            if (status) {
                ++m_successes;
            } else [[unlikely]] {
                ++m_failures;
                current_test().failed = true;
            }
        }

        [[nodiscard]] int assertions() const noexcept {
#if WEBPP_MULTI_THREADING_ENABLED
            return m_assertions.load();
#else
            return m_assertions;
#endif
        }

        [[nodiscard]] int successes() const noexcept {
#if WEBPP_MULTI_THREADING_ENABLED
            return m_successes.load();
#else
            return m_successes;
#endif
        }

        [[nodiscard]] int failures() const noexcept {
#if WEBPP_MULTI_THREADING_ENABLED
            return m_failures.load();
#else
            return m_failures;
#endif
        }

      private:
        std::vector<test_info> all_tests;
        atomic_int_type        m_assertions{}; // NOLINT
        atomic_int_type        m_failures{};   // NOLINT
        atomic_int_type        m_successes{};  // NOLINT
    };

    struct assert_result {
        assert_result(assert_result const&)            = default;
        assert_result(assert_result&&)                 = delete;
        assert_result& operator=(assert_result const&) = default;
        assert_result& operator=(assert_result&&)      = delete;

        assert_result(
          bool             success,
          std::string_view file,
          int              line,
          std::string      expr1,
          std::string      expr2,
          std::string_view macro_name)
          : ctx_() {
            ctx_.success    = success;
            ctx_.file       = file;
            ctx_.line       = line;
            ctx_.expr1      = std::move(expr1);
            ctx_.expr2      = std::move(expr2);
            ctx_.macro_name = macro_name;
        }

        template <typename T>
        assert_result& operator<<(T&& value) {
            // We don't need to print anything when the test is passed
            if (ctx_.success) [[likely]] {
                return *this;
            }

            std::ostringstream tmp;
            tmp << std::forward<T>(value);
            if (!ctx_.extra.empty()) {
                ctx_.extra += " ";
            }
            ctx_.extra += tmp.str();
            return *this;
        }

        ~assert_result() {
            if (!ctx_.success) {
                std::cerr << message(ctx_) << "\n";
            }
        }

        [[nodiscard]] bool ok() const {
            return ctx_.success;
        }

      private:
        assert_context ctx_;
    };

    // -------------------- TEST machinery --------------------
    inline int run_all_tests() {
        using std::cout;
        using clock = std::chrono::high_resolution_clock;
        using std::chrono::nanoseconds;

        auto&                          reg   = registry::instance();
        auto&                          tests = reg.tests();
        nanoseconds                    total_ns{};
        float                          index  = 0;
        auto const                     length = static_cast<float>(tests.size());
        std::chrono::time_point<clock> start{};
        std::chrono::time_point<clock> endp{};
        std::vector<test_info*>        failed_tests;

        cout << color::CYAN << "[==========] Running " << tests.size() << " tests.\n" << color::RESET;
        for (test_info& test : tests) {
            auto const percentage = static_cast<int>(index / length * 100.0F);

            current_test() = test;
            cout << color::YELLOW << "[ " << std::setw(7U) << percentage << "% ] " << color::RESET << test.suite << '.'
                 << test.name << std::flush;
            int failures   = reg.failures();
            int assertions = reg.assertions();
            int successes  = reg.successes();
#ifdef WEBPP_SUPPORTS_PERF_COUNTERS
            perf_counters counter;
#endif
            try {
#ifdef WEBPP_SUPPORTS_PERF_COUNTERS
                counter.start();
#endif
                start = clock::now();
                (*test.func)();
                endp = clock::now();
#ifdef WEBPP_SUPPORTS_PERF_COUNTERS
                counter.stop();
#endif
            } catch (std::exception const& ex) {
                std::cerr << "\n" << color::RED << "[  EXC     ] Exception: " << ex.what() << color::RESET << "\n";
                failed_tests.emplace_back(&test);
            } catch (...) {
                std::cerr << "\n" << color::RED << "[  EXC     ] Unknown exception" << color::RESET << "\n";
                failed_tests.emplace_back(&test);
            }
            failures          = reg.failures() - failures;
            assertions        = reg.assertions() - assertions;
            successes         = reg.successes() - successes;
            auto const dur    = std::chrono::duration_cast<nanoseconds>(endp - start);
            auto const color  = failures != 0 ? color::RED : color::GREEN;
            test.duration_ns  = dur;
            total_ns         += dur;
            if (test.failed) {
                cout << "\n" << color << "[  FAILED  ] ";
            } else {
                cout << "\r" << color << "[       OK ] ";
            }
            cout << color::RESET << test.suite << '.' << test.name << " " << format_duration(dur);
            if (failures != 0) {
                cout << " (" << color::RED << failures << color::RESET << "/" << color::GREEN << successes
                     << color::RESET << "/" << color::CYAN << assertions << color::RESET << " asserts)";
            } else {
                cout << color::GREY << " (" << successes << " asserts)" << color::RESET;
            }
#ifdef WEBPP_SUPPORTS_PERF_COUNTERS
            cout << color::PURPLE << " (";
            counter.print_short(cout);
            cout << ")" << color::RESET;
#endif
            cout << '\n' << std::flush;
            ++index;
        }

        int const  failures   = reg.failures();
        int const  assertions = reg.assertions();
        int const  successes  = reg.successes();
        auto const color      = failures != 0 ? color::RED : color::GREEN;

        cout << color << "[ ======== ] Run Time: " << color::RESET << format_duration(total_ns) << "\n";
        cout << color << "[  SUMMARY ] " << tests.size() << " tests, " << assertions << " assertions, " << failures
             << " failed assertions, " << color::GREEN << successes << color << " success assersions." << color::RESET
             << "\n";

        // std::cout << color::YELLOW << "Per-test timing (ns / us):\n" << color::RESET;
        // for (auto& t : tests) {
        //     std::cout << "  " << t.full_name() << ": " << t.duration_ns << " ns, " << (t.duration_ns / 1000) << " us"
        //               << (t.failed ? " [FAILED]" : "") << "\n";
        // }

        return (failures != 0) ? 1 : 0;
    }

// -------------------- Macros --------------------
#define TEST(test_suite_name, test_name)                                                                             \
    void test_suite_name##test_name();                                                                               \
    namespace details {                                                                                              \
        static int test_suite_name##test_name##Detail =                                                              \
          (::testing::registry::instance().register_test(#test_suite_name, #test_name, &test_suite_name##test_name), \
           0);                                                                                                       \
    }                                                                                                                \
    void test_suite_name##test_name()

#define TEST_F(test_fixture, test_name) TEST(test_fixture, test_name)

    // Generalized helpers to reduce duplication across make_* functions
    template <typename A, typename B, typename Predicate>
    inline assert_result make_binary_assertion(
      A const&         lhs,
      B const&         rhs,
      std::string_view file,
      int              line,
      std::string_view exprA,
      std::string_view exprB,
      std::string_view macro_name,
      Predicate const& predicate) {
        bool const is_ok = static_cast<bool>(predicate(lhs, rhs));
        registry::instance().asserted(is_ok);
        return assert_result{
          is_ok,
          file,
          line,
          !is_ok ? std::string(exprA) + " (" + stream_to_string(lhs) + ")" : "",
          !is_ok ? std::string(exprB) + " (" + stream_to_string(rhs) + ")" : "",
          macro_name};
    }

    inline assert_result make_unary_assertion(
      bool             value,
      bool             expect_true,
      std::string_view file,
      int              line,
      std::string_view expr,
      std::string_view macro_name) {
        bool const is_ok = expect_true ? value : !value;
        registry::instance().asserted(is_ok);
        return {is_ok,
                file,
                line,
                !is_ok ? std::string(expr) + " (" + (value ? "true" : "false") + ")" : "",
                "",
                macro_name};
    }

#define EXPECT_EQ(a, b) \
    (::testing::make_binary_assertion((a), (b), __FILE__, __LINE__, #a, #b, "EXPECT_EQ", ::testing::cmp_equal{}))
#define ASSERT_EQ(a, b) \
    (::testing::make_binary_assertion((a), (b), __FILE__, __LINE__, #a, #b, "ASSERT_EQ", ::testing::cmp_equal{}))
#define EXPECT_NE(a, b) \
    (::testing::make_binary_assertion((a), (b), __FILE__, __LINE__, #a, #b, "EXPECT_NE", ::testing::cmp_not_equal{}))
#define ASSERT_NE(a, b) \
    (::testing::make_binary_assertion((a), (b), __FILE__, __LINE__, #a, #b, "ASSERT_NE", ::testing::cmp_not_equal{}))
#define EXPECT_TRUE(x) \
    (::testing::make_unary_assertion(static_cast<bool>(x), true, __FILE__, __LINE__, #x, "EXPECT_TRUE"))
#define ASSERT_TRUE(x) \
    (::testing::make_unary_assertion(static_cast<bool>(x), true, __FILE__, __LINE__, #x, "ASSERT_TRUE"))

#define EXPECT_FALSE(x) \
    (::testing::make_unary_assertion(static_cast<bool>(x), false, __FILE__, __LINE__, #x, "EXPECT_FALSE"))
#define ASSERT_FALSE(x) \
    (::testing::make_unary_assertion(static_cast<bool>(x), false, __FILE__, __LINE__, #x, "ASSERT_FALSE"))

#define EXPECT_LT(a, b) \
    (::testing::make_binary_assertion((a), (b), __FILE__, __LINE__, #a, #b, "EXPECT_LT", ::testing::cmp_less{}))
#define ASSERT_LT(a, b) \
    (::testing::make_binary_assertion((a), (b), __FILE__, __LINE__, #a, #b, "ASSERT_LT", ::testing::cmp_less{}))
#define EXPECT_LE(a, b) \
    (::testing::make_binary_assertion((a), (b), __FILE__, __LINE__, #a, #b, "EXPECT_LE", ::testing::cmp_less_equal{}))
#define ASSERT_LE(a, b) \
    (::testing::make_binary_assertion((a), (b), __FILE__, __LINE__, #a, #b, "ASSERT_LE", ::testing::cmp_less_equal{}))
#define EXPECT_GT(a, b) \
    (::testing::make_binary_assertion((a), (b), __FILE__, __LINE__, #a, #b, "EXPECT_GT", ::testing::cmp_greater{}))
#define ASSERT_GT(a, b) \
    (::testing::make_binary_assertion((a), (b), __FILE__, __LINE__, #a, #b, "ASSERT_GT", ::testing::cmp_greater{}))
#define EXPECT_GE(a, b)                \
    (::testing::make_binary_assertion( \
      (a),                             \
      (b),                             \
      __FILE__,                        \
      __LINE__,                        \
      #a,                              \
      #b,                              \
      "EXPECT_GE",                     \
      ::testing::cmp_greater_equal{}))
#define ASSERT_GE(a, b)                \
    (::testing::make_binary_assertion( \
      (a),                             \
      (b),                             \
      __FILE__,                        \
      __LINE__,                        \
      #a,                              \
      #b,                              \
      "ASSERT_GE",                     \
      ::testing::cmp_greater_equal{}))

    // -------------------- Typed TEST support --------------------
    template <typename... Ts>
    struct Types {
        using type_list = std::tuple<Ts...>;
    };

    template <typename TypeList>
    struct ForEachType;

    template <typename... Ts>
    struct ForEachType<Types<Ts...>> {
        template <typename F>
        static void apply(F&& f) {
            (f.template operator()<Ts>(), ...);
        }
    };

#define TYPED_TEST_SUITE(test_suite_name, ...) using WEBPP_CONCAT(test_suite_name, _Types) = __VA_ARGS__;

#define TYPED_TEST(test_suite_name, test_name)                                                              \
    template <typename TypeParam>                                                                           \
    void WEBPP_CONCAT(test_suite_name, _##test_name##_TypedTest)();                                         \
    namespace {                                                                                             \
        struct WEBPP_CONCAT(test_suite_name, _##test_name##_Registrar) {                                    \
            WEBPP_CONCAT(test_suite_name, _##test_name##_Registrar)() {                                     \
                using TL = WEBPP_CONCAT(test_suite_name, _Types);                                           \
                ::testing::ForEachType<TL>::apply([&]<typename T>() {                                       \
                    std::string composed = std::string(#test_name) + "<" + ::testing::type_name<T>() + ">"; \
                    ::testing::register_test(#test_suite_name, composed.c_str(), []() {                     \
                        WEBPP_CONCAT(test_suite_name, _##test_name##_TypedTest)<T>();                       \
                    });                                                                                     \
                });                                                                                         \
            }                                                                                               \
        };                                                                                                  \
        static WEBPP_CONCAT(test_suite_name, _##test_name##_Registrar) WEBPP_UNIQUE_NAME(_typed_reg_);      \
    }                                                                                                       \
    template <typename TypeParam>                                                                           \
    void WEBPP_CONCAT(test_suite_name, _##test_name##_TypedTest)()

} // namespace testing

#ifndef WEBPP_NO_DEFAULT_MAIN
int main(int argc, char** argv) {
    (void) argc;
    (void) argv;
    return ::testing::run_all_tests();
}
#endif

#endif // WEBPP_TEST_IMPL_HPP
