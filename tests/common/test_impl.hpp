#ifndef WEBPP_TEST_IMPL_HPP
#define WEBPP_TEST_IMPL_HPP

#include "./test_utilities.hpp"

#include <atomic>
#include <chrono>
#include <cmath>
#include <functional>
#include <iostream>
#include <mutex>
#include <ostream>
#include <sstream>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

namespace testing {

    // -------------------- Test Registry --------------------
    struct alignas(128) test_info {
        std::string_view         suite;
        std::string_view         name;
        std::function<void()>    func;
        std::chrono::nanoseconds duration_ns{};
        bool                     failed = false;
    };

    [[nodiscard]] std::string full_name(test_info const& info) {
        return std::string{info.suite.data(), info.suite.size()} + "." +
               std::string{info.name.data(), info.name.size()};
    }

    struct alignas(32) registry {
        static registry& instance() {
            static registry inst;
            return inst;
        }

        void register_test(std::string_view suite, std::string_view name, std::function<void()> fn) {
            // std::lock_guard<std::mutex> lk(mu_);
            tests_.emplace_back(suite, name, std::move(fn), std::chrono::nanoseconds{}, false);
        }

        [[nodiscard]] std::vector<test_info>& tests() noexcept {
            return tests_;
        }

        void add_failure(std::string_view suite, std::string_view inp_test) {
            // std::lock_guard<std::mutex> lk(mu_);
            for (auto& test : tests_) {
                if (test.suite == suite && test.name == inp_test) {
                    test.failed = true;
                    break;
                }
            }
        }

      private:
        // std::mutex            mu_;
        std::vector<test_info> tests_;
    };

    inline void RegisterTest(std::string const& suite, std::string const& name, std::function<void()> func) {
        registry::instance().register_test(suite, name, std::move(func));
    }

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

    [[nodiscard]] std::string message(assert_context const& ctx) {
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

    namespace internal {
        inline std::atomic<int> global_assertions{0};
        inline std::atomic<int> global_failures{0};
    } // namespace internal

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
          std::string_view macro_name,
          std::string_view suite,
          std::string_view test)
          : ctx_(),
            suite_(suite),
            test_(test) {
            ctx_.success    = success;
            ctx_.file       = file;
            ctx_.line       = line;
            ctx_.expr1      = std::move(expr1);
            ctx_.expr2      = std::move(expr2);
            ctx_.macro_name = macro_name;
        }

        template <typename T>
        assert_result& operator<<(T&& value) {
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
                registry::instance().add_failure(suite_, test_);
                ++internal::global_failures;
            }
            ++internal::global_assertions;
        }

        [[nodiscard]] bool ok() const {
            return ctx_.success;
        }

      private:
        assert_context   ctx_;
        std::string_view suite_;
        std::string_view test_;
    };

    // -------------------- TEST machinery --------------------
    struct Test {
        virtual ~Test() = default;

        virtual void SetUp() {}

        virtual void TearDown() {}
    };

    struct alignas(32) CurrentTestInfo {
        std::string_view suite;
        std::string_view test;
    };

    inline CurrentTestInfo& current_test() {
        static thread_local CurrentTestInfo info;
        return info;
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

    inline int RunAllTests() {
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

        cout << color::CYAN << "[==========] Running " << tests.size() << " tests.\n" << color::RESET;
        for (auto& test : tests) {
            auto const percentage = static_cast<int>(index / length * 100.0F);
            auto const test_name  = full_name(test);
            cout << color::YELLOW << "[ " << std::setw(7U) << percentage << "% ] " << color::RESET << test_name
                 << std::flush;
            current_test().suite = test.suite;
            current_test().test  = test.name;
            try {
                start = clock::now();
                test.func();
                endp = clock::now();
            } catch (std::exception const& ex) {
                std::cerr << "\n" << color::RED << "[  EXC     ] Exception: " << ex.what() << color::RESET << "\n";
                reg.add_failure(test.suite, test.name);
            } catch (...) {
                std::cerr << "\n" << color::RED << "[  EXC     ] Unknown exception" << color::RESET << "\n";
                reg.add_failure(test.suite, test.name);
            }
            auto const dur    = std::chrono::duration_cast<nanoseconds>(endp - start);
            test.duration_ns  = dur;
            total_ns         += dur;
            if (test.failed) {
                cout << "\n"
                     << color::RED << "[  FAILED  ] " << color::RESET << test_name << " " << format_duration(dur)
                     << "\n";
            } else {
                cout << "\r" << color::GREEN << "[       OK ] " << color::RESET << test_name << " "
                     << format_duration(dur) << "\n";
            }
            cout << std::flush;
            ++index;
        }

        int        failures   = static_cast<int>(internal::global_failures.load());
        int        assertions = static_cast<int>(internal::global_assertions.load());
        auto const color      = failures != 0 ? color::RED : color::GREEN;

        cout << color << "[ ======== ] Run Time: " << color::RESET << format_duration(total_ns) << "\n";
        cout << color << "[  SUMMARY ] " << tests.size() << " tests, " << assertions << " assertions, " << failures
             << " failures." << color::RESET << "\n";

        // std::cout << color::YELLOW << "Per-test timing (ns / us):\n" << color::RESET;
        // for (auto& t : tests) {
        //     std::cout << "  " << t.full_name() << ": " << t.duration_ns << " ns, " << (t.duration_ns / 1000) << " us"
        //               << (t.failed ? " [FAILED]" : "") << "\n";
        // }

        return (failures != 0) ? 1 : 0;
    }

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

// -------------------- Macros --------------------
#define WEBPP_CONCAT_INTERNAL_(a, b) a##b
#define WEBPP_CONCAT(a, b)           WEBPP_CONCAT_INTERNAL_(a, b)
#define WEBPP_UNIQUE_NAME(base)      WEBPP_CONCAT(base, __COUNTER__)

#define TEST(test_suite_name, test_name)                                                    \
    struct WEBPP_CONCAT(test_suite_name, _##test_name##_Test) : public ::testing::Test {    \
        void        TestBody();                                                             \
        static void RunIt() {                                                               \
            WEBPP_CONCAT(test_suite_name, _##test_name##_Test) t;                           \
            t.SetUp();                                                                      \
            t.TestBody();                                                                   \
            t.TearDown();                                                                   \
        }                                                                                   \
    };                                                                                      \
    static int WEBPP_UNIQUE_NAME(_reg_) =                                                   \
      (::testing::RegisterTest(#test_suite_name,                                            \
                               #test_name,                                                  \
                               &WEBPP_CONCAT(test_suite_name, _##test_name##_Test)::RunIt), \
       0);                                                                                  \
    void WEBPP_CONCAT(test_suite_name, _##test_name##_Test)::TestBody()

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
        return assert_result{
          is_ok,
          file,
          line,
          std::string(exprA) + " (" + stream_to_string(lhs) + ")",
          std::string(exprB) + " (" + stream_to_string(rhs) + ")",
          macro_name,
          ::testing::current_test().suite,
          ::testing::current_test().test};
    }

    inline assert_result make_unary_assertion(
      bool             value,
      bool             expect_true,
      std::string_view file,
      int              line,
      std::string_view expr,
      std::string_view macro_name) {
        bool const is_ok = expect_true ? value : !value;
        return {is_ok,
                file,
                line,
                std::string(expr) + " (" + (value ? "true" : "false") + ")",
                "",
                macro_name,
                ::testing::current_test().suite,
                ::testing::current_test().test};
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
                    ::testing::RegisterTest(#test_suite_name, composed.c_str(), []() {                      \
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
    return ::testing::RunAllTests();
}
#endif

#endif // WEBPP_TEST_IMPL_HPP
