#include "../../core/include/webpp/std/functional.hpp"
#include "../../core/include/webpp/std/memory_resource.hpp"
#include "../benchmark.hpp"

using namespace webpp;

static void DirectCall(benchmark::State& state) {
    auto func = [i = 0]() mutable {
        return ++i;
    };
    for (auto _ : state) {
        benchmark::DoNotOptimize(func());
    }
}
BENCHMARK(DirectCall);

static void STLFunctionCall(benchmark::State& state) {
    std::function<int()> func = [i = 0]() mutable {
        return ++i;
    };
    for (auto _ : state) {
        benchmark::DoNotOptimize(func());
    }
}
BENCHMARK(STLFunctionCall);

static void ISTLFunctionCall(benchmark::State& state) {
    istl::function<int()> func = [i = 0]() mutable {
        return ++i;
    };
    for (auto _ : state) {
        benchmark::DoNotOptimize(func());
    }
}
BENCHMARK(ISTLFunctionCall);

///////////////////////////////////////////////////

static void STLFunctionInitCall(benchmark::State& state) {
    int c = 0;
    for (auto _ : state) {
        std::function<int()> func = [i = ++c]() mutable {
            return ++i;
        };
        benchmark::DoNotOptimize(func());
    }
}
BENCHMARK(STLFunctionInitCall);

static void ISTLFunctionInitCall(benchmark::State& state) {
    int c = 0;
    for (auto _ : state) {
        istl::function<int()> func = [i = ++c]() mutable {
            return ++i;
        };
        benchmark::DoNotOptimize(func());
    }
}
BENCHMARK(ISTLFunctionInitCall);


static void ISTLFunctionInitCallPMR(benchmark::State& state) {
    int                                 c = 0;
    stl::array<stl::byte, 100>          buff{};
    stl::pmr::monotonic_buffer_resource res{buff.begin(), buff.size()};
    stl::pmr::polymorphic_allocator     alloc{&res};

    for (auto _ : state) {
        istl::pmr::function<int()> func{[i = ++c]() mutable {
                                            return ++i;
                                        },
                                        alloc};
        benchmark::DoNotOptimize(func());
    }
}
BENCHMARK(ISTLFunctionInitCallPMR);

///////////////////////////////////////////////////


static void STLFunctionAssignCall(benchmark::State& state) {
    int                  c = 0;
    std::function<int()> func;
    for (auto _ : state) {
        func = [i = ++c]() mutable {
            return ++i;
        };
        benchmark::DoNotOptimize(func());
    }
}
BENCHMARK(STLFunctionAssignCall);

static void ISTLFunctionAssignCallPMR(benchmark::State& state) {
    int                                 c = 0;
    stl::array<stl::byte, 100>          buff{};
    stl::pmr::monotonic_buffer_resource res{buff.begin(), buff.size()};
    stl::pmr::polymorphic_allocator     alloc{&res};

    istl::pmr::function<int()> func{alloc};
    for (auto _ : state) {
        func = [i = ++c]() mutable {
            return ++i;
        };
        benchmark::DoNotOptimize(func());
    }
}
BENCHMARK(ISTLFunctionAssignCallPMR);

///////////////////////////////////////////////////////////

static void STLFunctionAssignBigCall(benchmark::State& state) {
    stl::size_t                  c = 0;
    std::function<stl::size_t()> func;
    for (auto _ : state) {
        func = [i = ++c, big = stl::array<stl::size_t, 30>{}]() mutable {
            big[i % 30] = i;
            return big[i % 30];
        };
        benchmark::DoNotOptimize(func());
    }
}
BENCHMARK(STLFunctionAssignBigCall);

static void ISTLFunctionAssignBigCallPMR(benchmark::State& state) {
    stl::size_t                         c = 0;
    stl::array<stl::byte, 100>          buff{};
    stl::pmr::monotonic_buffer_resource res{buff.begin(), buff.size()};
    stl::pmr::polymorphic_allocator     alloc{&res};

    istl::pmr::function<stl::size_t()> func{alloc};
    for (auto _ : state) {
        func = [i = ++c, big = stl::array<stl::size_t, 30>{}]() mutable {
            big[i % 30] = i;
            return big[i % 30];
        };
        benchmark::DoNotOptimize(func());
    }
}
BENCHMARK(ISTLFunctionAssignBigCallPMR);
