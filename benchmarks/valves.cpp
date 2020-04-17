#include "valves_pch.h"
using namespace webpp;

namespace webpp {

    class fake_interface;

    template <>
    class request_t<fake_interface> : public basic_request_t {
      public:
        std::string_view request_uri() const noexcept { return __path; }

        void set_path(std::string_view ___path) noexcept { __path = ___path; }

      protected:
        std::string __path;
    };

    class fake_interface : public basic_interface<fake_interface> {
      public:
        webpp::router<fake_interface> router;
        std::string body_result;
        std::string header_result;
        request_t<fake_interface> req;

        fake_interface() noexcept = default;

        void operator()() noexcept {
            auto res = this->router(req);
            body_result = res.body.str();
            header_result = res.header.str();
        }
    };

} // namespace webpp



////////////////////////////// Init //////////////////////////////


static void valves_empty_valve_init(benchmark::State& state) {
    for (auto _ : state) {
        auto _valve = valves::empty_t();
        benchmark::DoNotOptimize(_valve);
    }
}
BENCHMARK(valves_empty_valve_init);

static void valves_empty_valve_no_init(benchmark::State& state) {
    for (auto _ : state) {
        auto _valve = valves::empty;
        benchmark::DoNotOptimize(_valve);
    }
}
BENCHMARK(valves_empty_valve_no_init);

static void valves_dynamic_valve_init(benchmark::State& state) {
    for (auto _ : state) {
        auto _valve = valves::dynamic_valve<fake_interface>();
        benchmark::DoNotOptimize(_valve);
    }
}
BENCHMARK(valves_dynamic_valve_init);



////////////////////////////// AND //////////////////////////////

static void valves_and_two_empties(benchmark::State& state) {
    for (auto _ : state) {
        auto _valve = valves::empty && valves::empty;
        benchmark::DoNotOptimize(_valve);
    }
}
BENCHMARK(valves_and_two_empties);

static void valves_dynamic_and_two_empties(benchmark::State& state) {
    for (auto _ : state) {
        auto _valve = valves::dynamic_valve<fake_interface>() &&
                      valves::empty && valves::empty;
        benchmark::DoNotOptimize(_valve);
    }
}
BENCHMARK(valves_dynamic_and_two_empties);

////////////////////////////// OR //////////////////////////////

static void valves_or_two_empties(benchmark::State& state) {
    for (auto _ : state) {
        auto _valve = valves::empty || valves::empty;
        benchmark::DoNotOptimize(_valve);
    }
}
BENCHMARK(valves_or_two_empties);

static void valves_dynamic_or_two_empties(benchmark::State& state) {
    for (auto _ : state) {
        auto _valve = valves::dynamic_valve<fake_interface>() ||
                      valves::empty || valves::empty;
        benchmark::DoNotOptimize(_valve);
    }
}
BENCHMARK(valves_dynamic_or_two_empties);

////////////////////////////// XOR //////////////////////////////

static void valves_xor_two_empties(benchmark::State& state) {
    for (auto _ : state) {
        auto _valve = valves::empty xor valves::empty;
        benchmark::DoNotOptimize(_valve);
    }
}
BENCHMARK(valves_xor_two_empties);

static void valves_dynamic_xor_two_empties(benchmark::State& state) {
    for (auto _ : state) {
        auto _valve = valves::dynamic_valve<fake_interface>() ^ valves::empty ^
                      valves::empty;
        benchmark::DoNotOptimize(_valve);
    }
}
BENCHMARK(valves_dynamic_xor_two_empties);

////////////////////////////// operator() //////////////////////////////

static void valves_run_empty(benchmark::State& state) {
    auto _valve = valves::empty;
    request_t<fake_interface> req;
    for (auto _ : state) {
        benchmark::DoNotOptimize(_valve(req));
    }
}
BENCHMARK(valves_run_empty);

static void valves_run_empty_dynamic(benchmark::State& state) {
    auto _valve = valves::dynamic_valve<fake_interface>() and valves::empty;
    request_t<fake_interface> req;
    for (auto _ : state) {
        benchmark::DoNotOptimize(_valve(req));
    }
}
BENCHMARK(valves_run_empty_dynamic);
