#ifndef WEBPP_TEST_IMPL_HPP
#define WEBPP_TEST_IMPL_HPP

#include <atomic>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <ostream>
#include <sstream>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <typeinfo>
#include <vector>

#if defined(__GNUG__)
#    include <cxxabi.h>
#endif

namespace testing {

    // -------------------- Utilities --------------------
    inline std::string demangle(char const* name) {
#if defined(__GNUG__)
        int   status = 0;
        char* dem    = abi::__cxa_demangle(name, nullptr, nullptr, &status);
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
    inline std::string TypeName() {
        return demangle(typeid(T).name());
    }

    // ANSI colors
    namespace color {
        constexpr char const* RESET  = "\033[0m";
        constexpr char const* RED    = "\033[31m";
        constexpr char const* GREEN  = "\033[32m";
        constexpr char const* YELLOW = "\033[33m";
        constexpr char const* BLUE   = "\033[34m";
        constexpr char const* CYAN   = "\033[36m";
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
            oss << std::string(reinterpret_cast<char const*>(value.data()), value.size());
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

    // -------------------- Test Registry --------------------
    struct TestInfo {
        std::string           suite;
        std::string           name;
        std::function<void()> func;
        std::int64_t          duration_ns = 0;
        bool                  failed      = false;

        [[nodiscard]] std::string full_name() const {
            return suite + "." + name;
        }
    };

    class Registry {
      public:
        static Registry& Instance() {
            static Registry inst;
            return inst;
        }

        void RegisterTest(std::string const& suite, std::string const& name, std::function<void()> fn) {
            // std::lock_guard<std::mutex> lk(mu_);
            tests_.emplace_back(suite, name, std::move(fn), 0, false);
        }

        std::vector<TestInfo>& tests() {
            return tests_;
        }

        void AddFailure(std::string const& suite, std::string const& inp_test) {
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
        std::vector<TestInfo> tests_;
    };

    inline void RegisterTest(char const* suite, char const* name, std::function<void()> fn) {
        Registry::Instance().RegisterTest(suite, name, std::move(fn));
    }

    // -------------------- Assertion infra --------------------
    struct AssertionContext {
        char const* file = "";
        int         line = 0;
        std::string expr1;
        std::string expr2;
        std::string macro_name;
        std::string extra;
        bool        success = true;

        [[nodiscard]] std::string message() const {
            std::ostringstream ss;
            if (!success) {
                ss << color::RED << macro_name << " failed: " << color::RESET;
                ss << expr1;
                if (!expr2.empty()) {
                    ss << " vs " << expr2;
                }
                if (!extra.empty()) {
                    ss << "\n  " << extra;
                }
                ss << "\n  (" << file << ":" << line << ")";
            } else {
                ss << color::GREEN << macro_name << " OK: " << color::RESET << expr1;
                if (!expr2.empty()) {
                    ss << " vs " << expr2;
                }
            }
            return ss.str();
        }
    };

    namespace internal {
        inline std::atomic<int> global_assertions{0};
        inline std::atomic<int> global_failures{0};
    } // namespace internal

    class AssertionResult {
      public:
        AssertionResult(
          bool        success,
          char const* file,
          int         line,
          std::string expr1,
          std::string expr2,
          std::string macro_name,
          char const* suite,
          char const* test)
          : ctx_(),
            suite_(suite),
            test_(test) {
            ctx_.success    = success;
            ctx_.file       = file;
            ctx_.line       = line;
            ctx_.expr1      = std::move(expr1);
            ctx_.expr2      = std::move(expr2);
            ctx_.macro_name = std::move(macro_name);
        }

        template <typename T>
        AssertionResult& operator<<(T&& value) {
            std::ostringstream tmp;
            tmp << std::forward<T>(value);
            if (!ctx_.extra.empty()) {
                ctx_.extra += " ";
            }
            ctx_.extra += tmp.str();
            return *this;
        }

        ~AssertionResult() {
            if (!ctx_.success) {
                std::cerr << ctx_.message() << "\n";
                Registry::Instance().AddFailure(suite_, test_);
                ++internal::global_failures;
            }
            ++internal::global_assertions;
        }

        [[nodiscard]] bool ok() const {
            return ctx_.success;
        }

      private:
        AssertionContext ctx_;
        char const*      suite_;
        char const*      test_;
    };

    // -------------------- TEST machinery --------------------
    class Test {
      public:
        virtual ~Test() = default;

        virtual void SetUp() {}

        virtual void TearDown() {}
    };

    struct CurrentTestInfo {
        char const* suite = "";
        char const* test  = "";
    };

    inline CurrentTestInfo& current_test() {
        static thread_local CurrentTestInfo info;
        return info;
    }

    inline int RunAllTests() {
        using clock = std::chrono::high_resolution_clock;

        auto&                          reg      = Registry::Instance();
        auto&                          tests    = reg.tests();
        std::int64_t                   total_ns = 0;
        float                          index    = 0;
        auto const                     length   = static_cast<float>(tests.size());
        std::chrono::time_point<clock> start{};
        std::chrono::time_point<clock> endp{};

        std::cout << color::CYAN << "[==========] Running " << tests.size() << " tests.\n" << color::RESET;
        for (auto& test : tests) {
            auto const percentage = static_cast<int>(index / length * 100.0F);
            auto const test_name  = test.full_name();
            std::cout << color::BLUE << "[ " << std::setw(7U) << percentage << "% ] " << color::RESET << test_name
                      << std::flush;
            current_test().suite = test.suite.c_str();
            current_test().test  = test.name.c_str();
            try {
                start = clock::now();
                test.func();
                endp = clock::now();
            } catch (std::exception const& ex) {
                std::cerr << "\n" << color::RED << "[  EXC     ] Exception: " << ex.what() << color::RESET << "\n";
                reg.AddFailure(test.suite, test.name);
            } catch (...) {
                std::cerr << "\n" << color::RED << "[  EXC     ] Unknown exception" << color::RESET << "\n";
                reg.AddFailure(test.suite, test.name);
            }
            auto const dur_ns  = std::chrono::duration_cast<std::chrono::nanoseconds>(endp - start).count();
            test.duration_ns   = dur_ns;
            total_ns          += dur_ns;
            if (test.failed) {
                std::cout << "\n"
                          << color::RED << "[  FAILED  ] " << color::RESET << test_name << color::CYAN << " (" << dur_ns
                          << " ns, " << (dur_ns / 1000) << " us)\n"
                          << color::RESET;
            } else {
                std::cout << "\r" << color::GREEN << "[       OK ] " << color::RESET << test_name << color::CYAN << " ("
                          << dur_ns << " ns, " << (dur_ns / 1000) << " us)\n"
                          << color::RESET;
            }

            ++index;
        }

        std::cout << color::CYAN << "[==========] " << tests.size() << " tests ran. Total: " << total_ns << " ns ("
                  << (total_ns / 1000) << " us)." << color::RESET << "\n";

        int failures   = static_cast<int>(internal::global_failures.load());
        int assertions = static_cast<int>(internal::global_assertions.load());
        std::cout << ((failures != 0) ? color::RED : color::GREEN) << "[  SUMMARY ] " << tests.size() << " tests, "
                  << assertions << " assertions, " << failures << " failures." << color::RESET << "\n";

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
    inline AssertionResult make_binary_assertion(
      A const&    lhs,
      B const&    rhs,
      char const* file,
      int         line,
      char const* exprA,
      char const* exprB,
      char const* macro_name,
      Predicate&& predicate) {
        bool is_ok = static_cast<bool>(std::forward<Predicate>(predicate)(lhs, rhs));
        return AssertionResult(
          is_ok,
          file,
          line,
          std::string(exprA) + " (" + stream_to_string(lhs) + ")",
          std::string(exprB) + " (" + stream_to_string(rhs) + ")",
          macro_name,
          ::testing::current_test().suite,
          ::testing::current_test().test);
    }

    inline AssertionResult make_unary_assertion(
      bool        value,
      bool        expect_true,
      char const* file,
      int         line,
      char const* expr,
      char const* macro_name) {
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

#define EXPECT_EQ(a, b)                      \
    (::testing::make_binary_assertion(       \
      (a),                                   \
      (b),                                   \
      __FILE__,                              \
      __LINE__,                              \
      #a,                                    \
      #b,                                    \
      "EXPECT_EQ",                           \
      [](auto const& lhs, auto const& rhs) { \
          return lhs == rhs;                 \
      }))
#define ASSERT_EQ(a, b)                      \
    (::testing::make_binary_assertion(       \
      (a),                                   \
      (b),                                   \
      __FILE__,                              \
      __LINE__,                              \
      #a,                                    \
      #b,                                    \
      "ASSERT_EQ",                           \
      [](auto const& lhs, auto const& rhs) { \
          return lhs == rhs;                 \
      }))
#define EXPECT_NE(a, b)                      \
    (::testing::make_binary_assertion(       \
      (a),                                   \
      (b),                                   \
      __FILE__,                              \
      __LINE__,                              \
      #a,                                    \
      #b,                                    \
      "EXPECT_NE",                           \
      [](auto const& lhs, auto const& rhs) { \
          return lhs != rhs;                 \
      }))
#define ASSERT_NE(a, b)                      \
    (::testing::make_binary_assertion(       \
      (a),                                   \
      (b),                                   \
      __FILE__,                              \
      __LINE__,                              \
      #a,                                    \
      #b,                                    \
      "ASSERT_NE",                           \
      [](auto const& lhs, auto const& rhs) { \
          return lhs != rhs;                 \
      }))
#define EXPECT_TRUE(x) \
    (::testing::make_unary_assertion(static_cast<bool>(x), true, __FILE__, __LINE__, #x, "EXPECT_TRUE"))
#define ASSERT_TRUE(x) \
    (::testing::make_unary_assertion(static_cast<bool>(x), true, __FILE__, __LINE__, #x, "ASSERT_TRUE"))

#define EXPECT_FALSE(x) \
    (::testing::make_unary_assertion(static_cast<bool>(x), false, __FILE__, __LINE__, #x, "EXPECT_FALSE"))
#define ASSERT_FALSE(x) \
    (::testing::make_unary_assertion(static_cast<bool>(x), false, __FILE__, __LINE__, #x, "ASSERT_FALSE"))

#define EXPECT_LT(a, b)                      \
    (::testing::make_binary_assertion(       \
      (a),                                   \
      (b),                                   \
      __FILE__,                              \
      __LINE__,                              \
      #a,                                    \
      #b,                                    \
      "EXPECT_LT",                           \
      [](auto const& lhs, auto const& rhs) { \
          return lhs < rhs;                  \
      }))
#define ASSERT_LT(a, b)                      \
    (::testing::make_binary_assertion(       \
      (a),                                   \
      (b),                                   \
      __FILE__,                              \
      __LINE__,                              \
      #a,                                    \
      #b,                                    \
      "ASSERT_LT",                           \
      [](auto const& lhs, auto const& rhs) { \
          return lhs < rhs;                  \
      }))
#define EXPECT_LE(a, b)                      \
    (::testing::make_binary_assertion(       \
      (a),                                   \
      (b),                                   \
      __FILE__,                              \
      __LINE__,                              \
      #a,                                    \
      #b,                                    \
      "EXPECT_LE",                           \
      [](auto const& lhs, auto const& rhs) { \
          return lhs <= rhs;                 \
      }))
#define ASSERT_LE(a, b)                      \
    (::testing::make_binary_assertion(       \
      (a),                                   \
      (b),                                   \
      __FILE__,                              \
      __LINE__,                              \
      #a,                                    \
      #b,                                    \
      "ASSERT_LE",                           \
      [](auto const& lhs, auto const& rhs) { \
          return lhs <= rhs;                 \
      }))
#define EXPECT_GT(a, b)                      \
    (::testing::make_binary_assertion(       \
      (a),                                   \
      (b),                                   \
      __FILE__,                              \
      __LINE__,                              \
      #a,                                    \
      #b,                                    \
      "EXPECT_GT",                           \
      [](auto const& lhs, auto const& rhs) { \
          return lhs > rhs;                  \
      }))
#define ASSERT_GT(a, b)                      \
    (::testing::make_binary_assertion(       \
      (a),                                   \
      (b),                                   \
      __FILE__,                              \
      __LINE__,                              \
      #a,                                    \
      #b,                                    \
      "ASSERT_GT",                           \
      [](auto const& lhs, auto const& rhs) { \
          return lhs > rhs;                  \
      }))
#define EXPECT_GE(a, b)                      \
    (::testing::make_binary_assertion(       \
      (a),                                   \
      (b),                                   \
      __FILE__,                              \
      __LINE__,                              \
      #a,                                    \
      #b,                                    \
      "EXPECT_GE",                           \
      [](auto const& lhs, auto const& rhs) { \
          return lhs >= rhs;                 \
      }))
#define ASSERT_GE(a, b)                      \
    (::testing::make_binary_assertion(       \
      (a),                                   \
      (b),                                   \
      __FILE__,                              \
      __LINE__,                              \
      #a,                                    \
      #b,                                    \
      "ASSERT_GE",                           \
      [](auto const& lhs, auto const& rhs) { \
          return lhs >= rhs;                 \
      }))


#define TYPED_TEST_SUITE(test_suite_name, ...) using WEBPP_CONCAT(test_suite_name, _Types) = __VA_ARGS__;

#define TYPED_TEST(test_suite_name, test_name)                                                             \
    template <typename TypeParam>                                                                          \
    void WEBPP_CONCAT(test_suite_name, _##test_name##_TypedTest)();                                        \
    namespace {                                                                                            \
        struct WEBPP_CONCAT(test_suite_name, _##test_name##_Registrar) {                                   \
            WEBPP_CONCAT(test_suite_name, _##test_name##_Registrar)() {                                    \
                using TL = WEBPP_CONCAT(test_suite_name, _Types);                                          \
                ::testing::ForEachType<TL>::apply([&]<typename T>() {                                      \
                    std::string composed = std::string(#test_name) + "<" + ::testing::TypeName<T>() + ">"; \
                    ::testing::RegisterTest(#test_suite_name, composed.c_str(), []() {                     \
                        WEBPP_CONCAT(test_suite_name, _##test_name##_TypedTest)<T>();                      \
                    });                                                                                    \
                });                                                                                        \
            }                                                                                              \
        };                                                                                                 \
        static WEBPP_CONCAT(test_suite_name, _##test_name##_Registrar) WEBPP_UNIQUE_NAME(_typed_reg_);     \
    }                                                                                                      \
    template <typename TypeParam>                                                                          \
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
