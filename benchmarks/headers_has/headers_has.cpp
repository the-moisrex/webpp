#include <benchmark/benchmark.h>
#include "headers.hpp"

using namespace std;

static void EmptyRandom(benchmark::State& state) {
  for (auto _ : state) {
    // benchmark::DoNotOptimize(c);
  }
}
BENCHMARK(EmptyRandom);


auto prepare() {
    S s;
    for (int i = 0; i < 1000; i++) {
        s.push_back(std::to_string(i));
    }
    return s;
}

static void JoelHas(benchmark::State& state) {
    auto vals = prepare();
  for (auto _ : state) {
    auto has_them = vals.has("265", "123");
    benchmark::DoNotOptimize(has_them);
  }
}
BENCHMARK(JoelHas);


static void MoHas(benchmark::State& state) {
    auto vals = prepare();
  for (auto _ : state) {
    auto has_them = vals.has2("265", "123");
    benchmark::DoNotOptimize(has_them);
  }
}
BENCHMARK(MoHas);



static void JoelHasMo(benchmark::State& state) {
    auto vals = prepare();
  for (auto _ : state) {
    auto has_them = vals.has3("265", "123");
    benchmark::DoNotOptimize(has_them);
  }
}
BENCHMARK(JoelHasMo);
