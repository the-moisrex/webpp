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
    };

    // -------------------- Assertion infra --------------------
    struct alignas(128) assert_context {
        std::string_view file;
        int              line = 0;
        std::string_view lhs_expr;
        std::string_view rhs_expr;
        std::string      lhs_value;
        std::string      rhs_value;
        std::string_view macro_name;
        std::string      extra;
        bool             success = true;
    };

    inline test_info& current_test() {
        static test_info test;
        return test;
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

        void asserted(bool const status) {
            ++m_assertions;
            if (status) {
                ++m_successes;
            } else [[unlikely]] {
                ++m_failures;
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

    inline void handle_failure(std::exception const& err) {
        std::cout << "\n"
                  << color::RED << "[  EXC     ] Exception: " << color::RESET << err.what() << "\n"
                  << std::flush;
    }

    inline void handle_failure(assert_context const& ctx) {
        using std::cout;

        cout << '\n' << color::RED << ctx.macro_name << " Failed: " << color::RESET;
        cout << ctx.file << ":" << ctx.line << '\n';
        cout << "  LHS:      " << color::YELLOW << ctx.lhs_expr << color::RESET << '\n';
        if (!ctx.rhs_expr.empty()) {
            cout << "  RHS:      " << color::YELLOW << ctx.rhs_expr << color::RESET << '\n';
        }
        cout << "  Value:    " << ctx.lhs_value << '\n';
        if (!ctx.rhs_value.empty()) {
            cout << "  Expected: " << ctx.rhs_value << '\n';
        }
        cout << color::GREY << ctx.extra << color::RESET << '\n' << std::flush;
    }

    inline void handle_failure() {
        std::cout << "\n" << color::RED << "[  EXC     ] Unknown exception" << color::RESET << "\n" << std::flush;
    }

    struct assert_result {
        assert_result(assert_result const&)            = default;
        assert_result(assert_result&&)                 = delete;
        assert_result& operator=(assert_result const&) = default;
        assert_result& operator=(assert_result&&)      = delete;

        assert_result(
          bool const             success,
          std::string_view const file,
          int const              line,
          std::string_view const lhs_expr,
          std::string_view const rhs_expr,
          std::string            lhs_val,
          std::string            rhs_val,
          std::string_view const macro_name) {
            ctx.success    = success;
            ctx.file       = file;
            ctx.line       = line;
            ctx.lhs_expr   = lhs_expr;
            ctx.rhs_expr   = rhs_expr;
            ctx.lhs_value  = std::move(lhs_val);
            ctx.rhs_value  = std::move(rhs_val);
            ctx.macro_name = macro_name;
        }

        template <typename T>
        assert_result& operator<<(T&& value) {
            // We don't need to print anything when the test is passed
            if (ctx.success) [[likely]] {
                return *this;
            }

            std::ostringstream tmp;
            tmp << std::forward<T>(value);
            if (!ctx.extra.empty()) {
                ctx.extra += " ";
            }
            ctx.extra += tmp.str();
            return *this;
        }

        ~assert_result() {
            if (!ctx.success) {
                handle_failure(ctx);
            }
        }

        [[nodiscard]] bool ok() const {
            return ctx.success;
        }

      private:
        assert_context ctx;
    };

    // -------------------- TEST machinery --------------------
    inline int run_all_tests(bool const run_perfs = false) {
        using std::cout;
        using clock = std::chrono::high_resolution_clock;
        using std::chrono::nanoseconds;
        using std::chrono::time_point;

        auto&             reg   = registry::instance();
        auto&             tests = reg.tests();
        nanoseconds       total_ns{};
        float             index  = 0;
        auto const        length = static_cast<float>(tests.size());
        time_point<clock> start{};
        time_point<clock> endp{};
        // std::vector<test_info*>        failed_tests;

        cout << color::CYAN << "[ ======== ] Running " << tests.size() << " tests.\n" << color::RESET << std::flush;
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
                if (run_perfs) {
                    counter.start();
                }
#endif
                start = clock::now();
                (*test.func)();
                endp = clock::now();
#ifdef WEBPP_SUPPORTS_PERF_COUNTERS
                if (run_perfs) {
                    counter.stop();
                }
#endif
            } catch (std::exception const& err) {
                reg.asserted(false);
                handle_failure(err);
                // failed_tests.emplace_back(&test);
            } catch (...) {
                reg.asserted(false);
                handle_failure();
                // failed_tests.emplace_back(&test);
            }
            failures              = reg.failures() - failures;
            assertions            = reg.assertions() - assertions;
            successes             = reg.successes() - successes;
            bool const is_failed  = failures != 0;
            auto const dur        = std::chrono::duration_cast<nanoseconds>(endp - start);
            auto const color      = is_failed ? color::RED : color::GREEN;
            test.duration_ns      = dur;
            total_ns             += dur;
            if (is_failed) {
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
            if (run_perfs) {
                cout << color::PURPLE << " ";
                counter.print_anomalies(cout);
                cout << color::RESET;
            }
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
             << " failed assertions, " << color::GREEN << successes << color << " success assertions." << color::RESET
             << "\n";

        // std::cout << color::YELLOW << "Per-test timing (ns / us):\n" << color::RESET;
        // for (auto& t : tests) {
        //     std::cout << "  " << t.full_name() << ": " << t.duration_ns << " ns, " << (t.duration_ns / 1000) << " us"
        //               << (t.failed ? " [FAILED]" : "") << "\n";
        // }

        return (failures != 0) ? 1 : 0;
    }

// -------------------- Macros --------------------
// NOLINTBEGIN(*)
#define TEST(test_suite_name, test_name)                                                                             \
    static void      test_suite_name##test_name();                                                                   \
    static const int test_suite_name##test_name##Detail =                                                            \
      (::testing::registry::instance().register_test(#test_suite_name, #test_name, &test_suite_name##test_name), 0); \
    static void test_suite_name##test_name()
    // NOLINTEND(*)

// NOLINTBEGIN(*)
#define TEST_F(test_suite_name, test_name)                                                                             \
    struct test_suite_name##test_name : test_suite_name {                                                              \
        void body();                                                                                                   \
    };                                                                                                                 \
    static constinit test_suite_name##test_name test_suite_name##test_name##Instance;                                  \
    static void                                 test_suite_name##test_name##Func() {                                   \
        test_suite_name##test_name##Instance.body();                                   \
    }                                                                                                                  \
    static const int test_suite_name##test_name##Detail =                                                              \
      (::testing::registry::instance().register_test(#test_suite_name, #test_name, &test_suite_name##test_name##Func), \
       0);                                                                                                             \
    void test_suite_name##test_name::body()

    // NOLINTEND(*)

    // Generalized helpers to reduce duplication across make_* functions
    template <typename A, typename B, typename Predicate>
    assert_result make_binary_assertion(
      A const&               lhs,
      B const&               rhs,
      std::string_view const file,
      int const              line,
      std::string_view const exprA,
      std::string_view const exprB,
      std::string_view const macro_name,
      Predicate const&       predicate) {
        bool const is_ok = static_cast<bool>(predicate(lhs, rhs));
        registry::instance().asserted(is_ok);
        return assert_result{
          is_ok,
          file,
          line,
          exprA,
          exprB,
          !is_ok ? serialize(lhs) : "",
          !is_ok ? serialize(rhs) : "",
          macro_name};
    }

    inline assert_result make_unary_assertion(
      bool const             value,
      bool const             expect_true,
      std::string_view const file,
      int const              line,
      std::string_view const expr,
      std::string_view const macro_name) {
        bool const is_ok = expect_true ? value : !value;
        registry::instance().asserted(is_ok);
        return {is_ok, file, line, expr, "", !is_ok ? serialize(value) : "", "", macro_name};
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
        static void apply(F&& func) {
            (func.template operator()<Ts>(), ...);
        }
    };

    struct Test {
        virtual void SetUp() {}

        virtual void TearDown() {}
    };

#define TYPED_TEST_SUITE(test_suite_name, ...) using test_suite_name##Types = __VA_ARGS__;

// NOLINTBEGIN(*)
#define TYPED_TEST(test_suite_name, test_name)                                                     \
    template <typename TypeParam>                                                                  \
    struct test_suite_name##test_name : test_suite_name<TypeParam> {                               \
        void body();                                                                               \
    };                                                                                             \
    template <typename TypeParam>                                                                  \
    static void test_suite_name##test_name##Func() {                                               \
        test_suite_name##test_name<TypeParam> instance;                                            \
        instance.SetUp();                                                                          \
        instance.body();                                                                           \
        instance.TearDown();                                                                       \
    }                                                                                              \
    static const int test_suite_name##test_name##Detail =                                          \
      (::testing::ForEachType<test_suite_name##Types>::apply([]<typename T>() {                    \
           std::string composed = std::string(#test_name) + "<" + ::testing::type_name<T>() + ">"; \
           ::testing::registry::instance().register_test(                                          \
             #test_suite_name,                                                                     \
             #test_name,                                                                           \
             &test_suite_name##test_name##Func<T>);                                                \
       }),                                                                                         \
       0);                                                                                         \
    template <typename TypeParam>                                                                  \
    void test_suite_name##test_name<TypeParam>::body()
    // NOLINTEND(*)

} // namespace testing

#ifndef WEBPP_NO_DEFAULT_MAIN
int main(int const argc, char** argv) {
    bool enable_perf = false;
    for (int i = 1; i < argc; i++) {
        std::string_view const arg{argv[i]};
        if (arg == "--perf" || arg == "-perf") {
            enable_perf = true;
        }
    }
    return ::testing::run_all_tests(enable_perf);
}
#endif

#endif // WEBPP_TEST_IMPL_HPP
