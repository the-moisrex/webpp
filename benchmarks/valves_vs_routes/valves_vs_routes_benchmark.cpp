#include "../../webpp/http/routes/valves.hpp"
#include "../../webpp/traits/default_traits.hpp"
#include "../../webpp/traits/enable_traits.hpp"
#include "../benchmark.hpp"
#include "path.hpp"
#include "route.hpp"

using namespace std;
using namespace webpp;
using namespace webpp::http;

static void ValveBench(benchmark::State& state) {
    enable_owner_traits<default_dynamic_traits> et;
    request                                     req{et};
    context                                     ctx{req};
    for (auto _ : state) {
        auto t = valve{} / "about" / "page" >> [] {
            return "hello world";
        };
        t(ctx);
        benchmark::DoNotOptimize(t);
        benchmark::DoNotOptimize(ctx);
    }
}

BENCHMARK(ValveBench);

static void RouteBench(benchmark::State& state) {
    enable_owner_traits<default_dynamic_traits> et;
    request                                     req{et};
    simple_context<request>                     ctx{req};
    for (auto _ : state) {
        auto const t = root / "about" / "page" >>= [] {
            return "hello world";
        };
        benchmark::DoNotOptimize(t(ctx, req));
        benchmark::DoNotOptimize(ctx);
    }
}

BENCHMARK(RouteBench);

static void ValveCallBench(benchmark::State& state) {
    enable_owner_traits<default_dynamic_traits> et;
    request                                     req{et};
    context                                     ctx{req};
    auto                                        t = valve{} / "about" / "page" >> [] {
        return "hello world";
    };
    for (auto _ : state) {
        t(ctx);
        benchmark::DoNotOptimize(t);
        benchmark::DoNotOptimize(ctx);
    }
}

BENCHMARK(ValveCallBench);

static void RouteCallBench(benchmark::State& state) {
    enable_owner_traits<default_dynamic_traits> et;
    request                                     req{et};
    simple_context<request>                     ctx{req};
    auto const                                  t = root / "about" / "page" >>= [] {
        return "hello world";
    };
    for (auto _ : state) {
        benchmark::DoNotOptimize(t(ctx, req));
        benchmark::DoNotOptimize(ctx);
    }
}

BENCHMARK(RouteCallBench);

static void ValveStringBench(benchmark::State& state) {
    enable_owner_traits<default_dynamic_traits> et;
    request                                     req{et};
    context                                     ctx{req};
    for (auto _ : state) {
        auto t = valve{} / stl::pmr::string{"about"} / "page" >> [] {
            return "hello world";
        };
        t(ctx);
        benchmark::DoNotOptimize(t);
        benchmark::DoNotOptimize(ctx);
    }
}

BENCHMARK(ValveStringBench);

static void RouteStringBench(benchmark::State& state) {
    enable_owner_traits<default_dynamic_traits> et;
    request                                     req{et};
    simple_context<request>                     ctx{req};
    for (auto _ : state) {
        auto const t = root / stl::pmr::string{"about"} / "page" >>= [] {
            return "hello world";
        };
        benchmark::DoNotOptimize(t(ctx, req));
        benchmark::DoNotOptimize(ctx);
    }
}

BENCHMARK(RouteStringBench);
