#include "valves_pch.hpp"
using namespace webpp;

namespace webpp {

    class fake_interface;

    template <>
    class request_t<fake_interface> : public basic_request_t {
      public:
        std::string_view request_uri() const noexcept {
            return __path;
        }

        void set_path(std::string_view ___path) noexcept {
            __path = ___path;
        }

        auto request_method() const noexcept {
            return "GET";
        }

      protected:
        std::string __path;
    };

    class fake_interface : public basic_interface<fake_interface> {
      public:
        webpp::router<fake_interface> router;
        std::string                   body_result;
        std::string                   header_result;
        request_t<fake_interface>     req;

        fake_interface() noexcept = default;

        void operator()() noexcept {
            auto res      = this->router(req);
            body_result   = res.body.str();
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
    auto                      _valve = valves::empty;
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

////////////////////////////// method init //////////////////////////////

static void valves_method_init(benchmark::State& state) {
    for (auto _ : state) {
        auto _valve = valves::method("GET");
        benchmark::DoNotOptimize(_valve);
    }
}
BENCHMARK(valves_method_init);

static void valves_dynamic_method_init(benchmark::State& state) {
    for (auto _ : state) {
        auto _valve =
          valves::dynamic_valve<fake_interface>() and valves::method("GET");
        benchmark::DoNotOptimize(_valve);
    }
}
BENCHMARK(valves_dynamic_method_init);

////////////////////////////// method run //////////////////////////////

static void valves_method_run(benchmark::State& state) {
    auto                      _valve = valves::method("GET");
    request_t<fake_interface> req;
    for (auto _ : state) {
        benchmark::DoNotOptimize(_valve(req));
    }
}
BENCHMARK(valves_method_run);

static void valves_dynamic_method_run(benchmark::State& state) {
    auto _valve =
      valves::dynamic_valve<fake_interface>() and valves::method("GET");
    request_t<fake_interface> req;
    for (auto _ : state) {
        benchmark::DoNotOptimize(_valve(req));
    }
}

BENCHMARK(valves_dynamic_method_run);

////////////////////////////// long init //////////////////////////////

static void valves_long_init(benchmark::State& state) {
    for (auto _ : state) {
        auto _valve = valves::empty or valves::method("POST") or
                      valves::method("GET") and valves::empty or
                      valves::tpath("/about") and valves::empty or
                      valves::path("/home");
        benchmark::DoNotOptimize(_valve);
    }
}
BENCHMARK(valves_long_init);

static void valves_dynamic_long_init(benchmark::State& state) {
    for (auto _ : state) {
        auto _valve =
          valves::dynamic_valve<fake_interface>() and valves::empty or
          valves::method("POST") or valves::method("GET") and valves::empty or
          valves::tpath("/about") and valves::empty or valves::path("/home");
        benchmark::DoNotOptimize(_valve);
    }
}
BENCHMARK(valves_dynamic_long_init);

////////////////////////////// long run //////////////////////////////

static void valves_long_run(benchmark::State& state) {
    auto _valve = valves::empty or valves::method("POST") or
                  valves::method("GET") and valves::empty or
                  valves::tpath("/about") and valves::empty or
                  valves::path("/home");
    request_t<fake_interface> req;
    for (auto _ : state) {
        benchmark::DoNotOptimize(_valve(req));
    }
}
BENCHMARK(valves_long_run);

static void valves_dynamic_long_run(benchmark::State& state) {
    auto _valve =
      valves::dynamic_valve<fake_interface>() and valves::empty or
      valves::method("POST") or valves::method("GET") and valves::empty or
      valves::tpath("/about") and valves::empty or valves::path("/home");
    request_t<fake_interface> req;
    for (auto _ : state) {
        benchmark::DoNotOptimize(_valve(req));
    }
}

BENCHMARK(valves_dynamic_long_run);
