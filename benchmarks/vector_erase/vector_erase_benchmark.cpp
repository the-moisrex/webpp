#include "../benchmark.hpp"

#include <vector>
#include <algorithm>
using namespace std;

auto const strs = []() {
  vector<string> _strs;
  for (int i = 0; i < 10000; i++) {
    _strs.push_back(str_generator());
  }
  return _strs;
 }();
auto const ints = []() {
  vector<int> _ints;
  for (int i = 0; i < 10000; i++) {
    _ints.push_back(i);
  }
  return _ints;
 }();

static void StdVecErase_IntRotate(benchmark::State& state) {
  for (auto _ : state) {
    auto the_ints = ints;
    auto the_end = the_ints.end();
    for (int i = 0; i < the_ints.size()/2; i++) {
      the_end = rotate(the_ints.begin(), next(the_ints.begin()), the_ints.end());
    }
    the_ints.erase(the_end, the_ints.end());
    benchmark::DoNotOptimize(the_end);
    benchmark::DoNotOptimize(the_ints);
  }
}
BENCHMARK(StdVecErase_IntRotate);

static void StdVecErase_IntErase(benchmark::State& state) {
  for (auto _ : state) {
    auto the_ints = ints;
    auto the_end = the_ints.end();
    for (int i = 0; i < the_ints.size()/2; i++) {
      the_ints.erase(the_ints.begin(), next(the_ints.begin()));
    }
    benchmark::DoNotOptimize(the_end);
    benchmark::DoNotOptimize(the_ints);
  }
}
BENCHMARK(StdVecErase_IntErase);



static void StdVecErase_StrRotate(benchmark::State& state) {
  for (auto _ : state) {
    auto the_strs = strs;
    auto the_end = the_strs.end();
    for (int i = 0; i < the_strs.size()/2; i++) {
      the_end = rotate(the_strs.begin(), next(the_strs.begin()), the_strs.end());
    }
    the_strs.erase(the_end, the_strs.end());
    benchmark::DoNotOptimize(the_end);
    benchmark::DoNotOptimize(the_strs);
  }
}
BENCHMARK(StdVecErase_StrRotate);

static void StdVecErase_StrShift(benchmark::State& state) {
  for (auto _ : state) {
    auto the_strs = strs;
    auto the_end = the_strs.end();
    for (int i = 0; i < the_strs.size()/2; i++) {
      the_end = shift_left(the_strs.begin(), the_strs.end(), 2);
    }
    the_strs.erase(the_end, the_strs.end());
    benchmark::DoNotOptimize(the_end);
    benchmark::DoNotOptimize(the_strs);
  }
}
BENCHMARK(StdVecErase_StrShift);

static void StdVecErase_StrErase(benchmark::State& state) {
  for (auto _ : state) {
    auto the_strs = strs;
    auto the_end = the_strs.end();
    for (int i = 0; i < the_strs.size()/2; i++) {
      the_strs.erase(the_strs.begin(), next(the_strs.begin()));
    }
    benchmark::DoNotOptimize(the_end);
    benchmark::DoNotOptimize(the_strs);
  }
}
BENCHMARK(StdVecErase_StrErase);

