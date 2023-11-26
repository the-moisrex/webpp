#include "../benchmark.hpp"
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

auto prepare2() {
    S s;
    for (int i = 0; i < 10; i++) {
        s.push_back("This is a long string" + std::to_string(i));
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

//////////////////////////////////////////////////

static void JoelHas2(benchmark::State& state) {
    auto vals = prepare2();
    for (auto _ : state) {
        auto has_them = vals.has("This is a long string265", "This is a long string123");
        benchmark::DoNotOptimize(has_them);
    }
}

BENCHMARK(JoelHas2);

static void MoHas2(benchmark::State& state) {
    auto vals = prepare2();
    for (auto _ : state) {
        auto has_them = vals.has2("This is a long string265", "This is a long string123");
        benchmark::DoNotOptimize(has_them);
    }
}

BENCHMARK(MoHas2);

static void JoelHasMo2(benchmark::State& state) {
    auto vals = prepare2();
    for (auto _ : state) {
        auto has_them = vals.has3("This is a long string265", "This is a long string123");
        benchmark::DoNotOptimize(has_them);
    }
}

BENCHMARK(JoelHasMo2);
