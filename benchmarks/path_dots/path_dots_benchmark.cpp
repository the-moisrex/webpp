#include "../../webpp/uri/parser/parse_path.hpp"
#include "../benchmark.hpp"

#include <cstdint>
#include <string_view>

namespace stl {
    using namespace std;
}

namespace v1 {
    template <bool IgnoreWhitespace = true, typename IterT>
    [[nodiscard]] static constexpr stl::uint8_t dots_count(IterT pos, IterT end) noexcept {
        using char_type = typename std::iterator_traits<IterT>::value_type;

        stl::uint8_t dots = 0;
        char_type    prev = 0;

        for (;; ++pos) {
            if (pos == end) {
                break;
            }

            switch (*pos) {
                case '%':
                    if (prev != 0) {
                        return 0;
                    }
                    prev = '%';
                    continue;
                case '2':
                    if (prev != '%') {
                        return 0;
                    }
                    prev = '2';
                    continue;
                case 'e':
                case 'E':
                    if (prev != '2') {
                        return 0;
                    }
                    prev = 0;
                    ++dots;
                    continue;
                case '.':
                    ++dots;
                    continue;
                [[unlikely]] case '\n':
                [[unlikely]] case '\r':
                [[unlikely]] case '\t':
                    if constexpr (IgnoreWhitespace) {
                        continue;
                    }
                    [[fallthrough]];

                // a normal path:
                [[likely]] default:
                    return 0;
            }
            break;
        }

        return dots;
    }
} // namespace v1

namespace v2 {

    // Character Category Lookup Table
    template <bool IgnoreWhitespaces = true>
    static constexpr auto dots_category = ([]() consteval {
        stl::array<stl::uint8_t, 256U> category = {};
        category.fill(5);       // Default category is 5 (other characters)
        category['%'] = 1;      // Start of percent-encoding
        category['2'] = 2;      // Part of %2e
        category['e'] = 3;      // End of %2e (lowercase)
        category['E'] = 3;      // End of %2E (uppercase)
        category['.'] = 4;      // Literal dot
        if constexpr (IgnoreWhitespaces) {
            category['\n'] = 0; // Whitespace (ignored)
            category['\r'] = 0; // Whitespace (ignored)
            category['\t'] = 0; // Whitespace (ignored)
        }
        return category;
    })();

    // State Transition Table
    static constexpr stl::array<stl::array<stl::uint8_t, 6>, 11> state_transitions{
      {{0, 0, 0, 0, 0, 0}, // nothing
                           // State 1: dot_count=1, matching=0 (valid state)
       {1, 6, 0, 0, 2, 0},
       // State 2: dot_count=2, matching=0 (valid state)
       {2, 8, 0, 0, 0, 0},

       // State 3: dot_count=0, matching=0 (initial state)
       {3, 4, 0, 0, 1, 0},
       // State 4: dot_count=0, matching=1 (after '%')
       {4, 0, 5, 0, 0, 0},
       // State 5: dot_count=0, matching=2 (after '%2')
       {5, 0, 0, 1, 0, 0},

       // State 6: dot_count=1, matching=1 (after '%')
       {6, 0, 7, 0, 0, 0},
       // State 7: dot_count=1, matching=2 (after '%2')
       {7, 0, 0, 2, 0, 0},

       // State 8: dot_count=2, matching=1 (after '%')
       {8, 0, 9, 0, 0, 0},
       // State 9: dot_count=2, matching=2 (after '%2')
       {9, 0, 0, 0, 0, 0},
       // State 10: invalid state
       {10, 0, 0, 0, 0, 0}}
    };

    template <bool IgnoreWhitespace = true, typename IterT>
    [[nodiscard]] static constexpr stl::uint8_t dots_count(IterT pos, IterT end) noexcept {
        using char_type = typename std::iterator_traits<IterT>::value_type;

        char_type prev = 0;

        stl::uint8_t state = 3; // Start at initial state

        // **FSM Processing Loop**
        for (; pos != end; ++pos) {
            auto const category = dots_category<IgnoreWhitespace>[static_cast<unsigned char>(*pos)];
            state               = state_transitions[state][category];
            if (state == 0) {
                break; // Exit early if sequence becomes invalid
            }
        }

        if (state >= 3) {
            return 0;
        }
        return state;
    }

} // namespace v2

namespace v3 {
    template <bool IgnoreWhitespace = true, typename IterT>
    [[nodiscard]] static constexpr stl::uint8_t dots_count(IterT pos, IterT end) noexcept {
        return webpp::uri::details::dots_count<IgnoreWhitespace>(pos, end);
    }

} // namespace v3

namespace {

    void test_if_eq(auto&& lhs, auto&& rhs) {
        benchmark::DoNotOptimize(lhs);
        benchmark::DoNotOptimize(rhs);
    }

} // namespace

#define benchmark_version(Ver)                                        \
    static void IPV6Size_##Ver(benchmark::State& state) {             \
        auto dots = [](std::string_view const input) {                \
            return Ver::dots_count<true>(input.begin(), input.end()); \
        };                                                            \
        for (auto _ : state) {                                        \
            test_if_eq(dots("."), 1);                                 \
            test_if_eq(dots(".."), 2);                                \
            test_if_eq(dots("%2e"), 1);                               \
            test_if_eq(dots("%2E"), 1);                               \
            test_if_eq(dots("%2e%2e"), 2);                            \
            test_if_eq(dots(".%2e"), 2);                              \
            test_if_eq(dots("%2e."), 2);                              \
            test_if_eq(dots("\n.\n"), 1);                             \
            test_if_eq(dots("\t..\t"), 2);                            \
            test_if_eq(dots("\r%2e\r"), 1);                           \
            test_if_eq(dots(".%2E"), 2);                              \
            test_if_eq(dots("%2E."), 2);                              \
            test_if_eq(dots("..."), 0);                               \
            test_if_eq(dots("%2e%2e%2e"), 0);                         \
            test_if_eq(dots(".%2e."), 0);                             \
            test_if_eq(dots("%"), 0);                                 \
            test_if_eq(dots("%2"), 0);                                \
            test_if_eq(dots("%2f"), 0);                               \
            test_if_eq(dots("2e"), 0);                                \
            test_if_eq(dots("e2"), 0);                                \
            test_if_eq(dots(".a"), 0);                                \
            test_if_eq(dots("..b"), 0);                               \
            test_if_eq(dots("%2ec"), 0);                              \
            test_if_eq(dots("a."), 0);                                \
            test_if_eq(dots("b.."), 0);                               \
            test_if_eq(dots("c%2e"), 0);                              \
            test_if_eq(dots(".%"), 0);                                \
            test_if_eq(dots("..%"), 0);                               \
            test_if_eq(dots("%2e%"), 0);                              \
            test_if_eq(dots("%2e.."), 0);                             \
            test_if_eq(dots(".%2e.."), 0);                            \
            test_if_eq(dots("..%2e."), 0);                            \
            test_if_eq(dots("%2e\n.\t%2e"), 0);                       \
            test_if_eq(dots(" ."), 0);                                \
            test_if_eq(dots("  .."), 0);                              \
            test_if_eq(dots("   %2e"), 0);                            \
            test_if_eq(dots(" \t\n\r"), 0);                           \
            test_if_eq(dots(""), 0);                                  \
            test_if_eq(dots("...."), 0);                              \
            test_if_eq(dots("%2e%2e."), 0);                           \
            test_if_eq(dots(".%2e%2e"), 0);                           \
            test_if_eq(dots("abc."), 0);                              \
            test_if_eq(dots("abc.."), 0);                             \
            test_if_eq(dots("abc%2e"), 0);                            \
            test_if_eq(dots("...%2e"), 0);                            \
            test_if_eq(dots("%2..."), 0);                             \
            test_if_eq(dots("%2e.a"), 0);                             \
            test_if_eq(dots("..a"), 0);                               \
            test_if_eq(dots(".a"), 0);                                \
        }                                                             \
    }                                                                 \
    BENCHMARK(IPV6Size_##Ver)


benchmark_version(v1);
benchmark_version(v2);
benchmark_version(v3);
