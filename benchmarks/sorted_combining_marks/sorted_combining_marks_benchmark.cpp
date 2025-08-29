#include "../../webpp/unicode/ccc.hpp"
#include "../../webpp/unicode/unicode.hpp"
#include "../../webpp/unicode/utf32_iterator.hpp"
#include "../benchmark.hpp"
#include "../common_utils_pch.hpp"

#include <algorithm>
#include <random>
#include <string>
#include <string_view>
#include <vector>

using namespace std::string_view_literals;

namespace {

    void canonical_reorder_simple(std::u32string& input) {
        for (std::size_t idx = 1; idx < input.size(); idx++) {
            auto const ccc = webpp::unicode::ccc_of(input[idx]);
            if (ccc == 0) {
                continue;
            }
            auto const cur     = input[idx];
            auto       prev_id = idx;
            while (prev_id != 0 && webpp::unicode::ccc_of(input[prev_id - 1]) > ccc) {
                input[prev_id] = input[prev_id - 1];
                prev_id--;
            }
            input[prev_id] = cur;
        }
    }
} // namespace

// Generate ASCII-only string
std::string make_ascii_string(size_t length) {
    std::string result;
    result.reserve(length);
    std::mt19937                    gen(42);
    std::uniform_int_distribution<> dis(32, 126); // Printable ASCII
    for (size_t i = 0; i < length; ++i) {
        result.push_back(static_cast<char>(dis(gen)));
    }
    return result;
}

// Generate string with many combining marks
std::u32string make_combining_heavy_string(size_t length) {
    std::u32string result;
    result.reserve(length);
    std::mt19937 gen(42);

    // Common combining marks (accents)
    std::vector<char32_t> combining_marks = {
      0x0300,
      0x0301,
      0x0302,
      0x0303,
      0x0304,
      0x0305,
      0x0306,
      0x0307,
      0x0308,
      0x0309,
      0x030A,
      0x030B,
      0x030C,
      0x030D,
      0x030E,
      0x030F};

    std::uniform_int_distribution<> base_dis(0x4E00, 0x9FFF); // CJK Unified Ideographs
    std::uniform_int_distribution<> comb_dis(0, combining_marks.size() - 1);

    for (size_t i = 0; i < length; ++i) {
        // Add base character
        result.push_back(static_cast<char32_t>(base_dis(gen)));
        // Add 1-3 combining marks
        int num_marks = 1 + (i % 3);
        for (int j = 0; j < num_marks; ++j) {
            result.push_back(combining_marks[comb_dis(gen)]);
        }
    }
    return result;
}

// Convert between encodings
std::u16string utf8_to_utf16(std::string_view sv) {
    std::u16string result;
    auto           it = sv.begin();
    webpp::unicode::unchecked::append(result, it, sv.end());
    return result;
}

std::u32string utf8_to_utf32(std::string_view sv) {
    std::u32string result;
    auto           it = sv.begin();
    webpp::unicode::unchecked::append(result, it, sv.end());
    return result;
}

std::string utf32_to_utf8(std::u32string_view sv) {
    std::string result;
    auto        it = sv.begin();
    webpp::unicode::unchecked::append(result, it, sv.end());
    return result;
}

// Benchmark for ASCII UTF-8 input
static void BM_AsciiUTF8_Iterator(benchmark::State& state) {
    auto input = make_ascii_string(state.range(0));
    for (auto _ : state) {
        std::u32string temp;
        auto           it = input.begin();
        webpp::unicode::unchecked::append(temp, it, input.end());

        std::u32string                                  result;
        webpp::unicode::checked::utf32_forward_iter     fiter{temp.begin(), temp.end()};
        webpp::unicode::sorted_combining_marks_iterator iter{fiter};

        for (; iter != std::default_sentinel; ++iter) {
            result.push_back(*iter);
        }
        benchmark::DoNotOptimize(result);
    }
}

BENCHMARK(BM_AsciiUTF8_Iterator)->Range(8, 8 << 10);

static void BM_AsciiUTF8_CanonicalReorder(benchmark::State& state) {
    auto input = make_ascii_string(state.range(0));
    for (auto _ : state) {
        auto result = webpp::unicode::canonically_reordered(input);
        benchmark::DoNotOptimize(result);
    }
}

BENCHMARK(BM_AsciiUTF8_CanonicalReorder)->Range(8, 8 << 10);

// Benchmark for combining-heavy UTF-32 input
static void BM_CombiningUTF32_Iterator(benchmark::State& state) {
    auto input = make_combining_heavy_string(state.range(0));
    for (auto _ : state) {
        std::u32string                                  result;
        webpp::unicode::checked::utf32_forward_iter     fiter{input.begin(), input.end()};
        webpp::unicode::sorted_combining_marks_iterator iter{fiter};

        for (; iter != std::default_sentinel; ++iter) {
            result.push_back(*iter);
        }
        benchmark::DoNotOptimize(result);
    }
}

BENCHMARK(BM_CombiningUTF32_Iterator)->Range(8, 8 << 10);

static void BM_CombiningUTF32_Iterator_NoCopy(benchmark::State& state) {
    auto input = make_combining_heavy_string(state.range(0));
    for (auto _ : state) {
        webpp::unicode::checked::utf32_forward_iter     fiter{input.begin(), input.end()};
        webpp::unicode::sorted_combining_marks_iterator iter{fiter};
        for (; iter != std::default_sentinel; ++iter) {
            auto val = *iter;
            benchmark::DoNotOptimize(val);
        }
    }
}

BENCHMARK(BM_CombiningUTF32_Iterator_NoCopy)->Range(8, 8 << 10);

static void BM_CombiningUTF32_CanonicalReorder(benchmark::State& state) {
    auto input = make_combining_heavy_string(state.range(0));
    for (auto _ : state) {
        auto result = webpp::unicode::canonically_reordered(input);
        benchmark::DoNotOptimize(result);
    }
}

BENCHMARK(BM_CombiningUTF32_CanonicalReorder)->Range(8, 8 << 10);

static void BM_CombiningUTF32_Simple(benchmark::State& state) {
    auto input = make_combining_heavy_string(state.range(0));
    for (auto _ : state) {
        auto temp = input;
        canonical_reorder_simple(temp);
        benchmark::DoNotOptimize(temp);
    }
}

BENCHMARK(BM_CombiningUTF32_Simple)->Range(8, 8 << 10);

// Benchmark for UTF-8 to UTF-32 conversion
static void BM_UTF8toUTF32_Iterator(benchmark::State& state) {
    auto input      = make_combining_heavy_string(state.range(0));
    auto utf8_input = utf32_to_utf8(input);

    for (auto _ : state) {
        std::u32string temp;
        auto           it = utf8_input.begin();
        webpp::unicode::unchecked::append(temp, it, utf8_input.end());

        std::u32string                                  result;
        webpp::unicode::checked::utf32_forward_iter     fiter{temp.begin(), temp.end()};
        webpp::unicode::sorted_combining_marks_iterator iter{fiter};

        for (; iter != std::default_sentinel; ++iter) {
            result.push_back(*iter);
        }
        benchmark::DoNotOptimize(result);
    }
}

BENCHMARK(BM_UTF8toUTF32_Iterator)->Range(8, 8 << 10);

// Benchmark with pre-allocated output
static void BM_NoAlloc_Iterator(benchmark::State& state) {
    auto           input = make_combining_heavy_string(state.range(0));
    std::u32string result;
    result.resize(input.size());

    for (auto _ : state) {
        webpp::unicode::checked::utf32_forward_iter     fiter{input.begin(), input.end()};
        webpp::unicode::sorted_combining_marks_iterator iter{fiter};

        auto out = result.begin();
        for (; iter != std::default_sentinel; ++iter, ++out) {
            *out = *iter;
        }
        benchmark::DoNotOptimize(result);
    }
}

BENCHMARK(BM_NoAlloc_Iterator)->Range(8, 8 << 10);

static void BM_NoAlloc_CanonicalReorder_WithCopy(benchmark::State& state) {
    auto           input = make_combining_heavy_string(state.range(0));
    std::u32string result;
    result.resize(input.size());

    for (auto _ : state) {
        result.clear();
        std::copy(input.begin(), input.end(), std::back_inserter(result));
        webpp::unicode::canonically_reorder(result);
        benchmark::DoNotOptimize(result);
    }
}

BENCHMARK(BM_NoAlloc_CanonicalReorder_WithCopy)->Range(8, 8 << 10);

static void BM_NoAlloc_CanonicalReorder_NoCopy(benchmark::State& state) {
    auto           input = make_combining_heavy_string(state.range(0));
    std::u32string result;
    result.resize(input.size());
    std::copy(input.begin(), input.end(), std::back_inserter(result));

    for (auto _ : state) {
        webpp::unicode::canonically_reorder(result);
        benchmark::DoNotOptimize(result);
    }
}

BENCHMARK(BM_NoAlloc_CanonicalReorder_NoCopy)->Range(8, 8 << 10);
