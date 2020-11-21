#include "../../core/include/webpp/strings/join.hpp"
#include "../benchmark_pch.hpp"
#include "../common_utils_pch.hpp"

#include <memory_resource>
#include <string>
#include <string_view>

#if __has_include(<fmt/format.h>)
#include <fmt/format.h>
#include <fmt/compile.h>
#define has_fmt
#endif

using namespace webpp;

static void StrJoin_Webpp(benchmark::State& state) {
  for (auto _ : state) {
    std::string str1 = str_generator();
    std::string_view str2 = "two ";
    std::pmr::string str3 = str_generator<std::pmr::string>();

    auto res = string::join(str1, str2, str3, "; done");
    benchmark::DoNotOptimize(res);
  }
}
BENCHMARK(StrJoin_Webpp);



static void StrJoin_STD(benchmark::State& state) {
  for (auto _ : state) {
    std::string str1 = str_generator();
    std::string_view str2 = "two ";
    std::pmr::string str3 = str_generator<std::pmr::string>();

    std::pmr::string res;
    res.reserve(str1.size() + str2.size() + str3.size() + 6);
    res += str1;
    res += str2;
    res += str3;
    res += "; done";
    benchmark::DoNotOptimize(res);
  }
}
BENCHMARK(StrJoin_STD);

#ifdef has_fmt
static void StrJoin_FMT(benchmark::State& state) {
    for (auto _ : state) {
        std::string str1 = str_generator();
        std::string_view str2 = "two ";
        std::pmr::string str3 = str_generator<std::pmr::string>();

        auto res = fmt::format(FMT_COMPILE("{}{}{}{}"), str1, str2, str3, "; done");
        benchmark::DoNotOptimize(res);
    }
}
BENCHMARK(StrJoin_FMT);
#endif