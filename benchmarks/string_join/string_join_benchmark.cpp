#include "../../core/include/webpp/strings/join.hpp"
#include "../benchmark_pch.hpp"
#include "../common_utils_pch.hpp"

#include <memory_resource>
#include <string>
#include <string_view>

using namespace webpp;
using namespace std;

static void StrJoin_Webpp(benchmark::State& state) {
  for (auto _ : state) {
    string str1 = str_generator();
    string_view str2 = "two ";
    pmr::string str3 = str_generator<pmr::string>();

    auto res = join(str1, str2, str3, "; done");
    benchmark::DoNotOptimize(res);
  }
}
BENCHMARK(StrJoin_Webpp);



static void StrJoin_STD(benchmark::State& state) {
  for (auto _ : state) {
    string str1 = str_generator();
    string_view str2 = "two ";
    pmr::string str3 = str_generator<pmr::string>();

    pmr::string res;
    res.reserve(str1.size() + str2.size() + str3.size() + 6);
    res += str1;
    res += str2;
    res += str3;
    res += "; done";
    benchmark::DoNotOptimize(res);
  }
}
BENCHMARK(StrJoin_STD);
