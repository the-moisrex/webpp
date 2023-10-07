#include "../../webpp/std/string_view.hpp"
#include "../benchmark.hpp"
#include "./ada_scheme.hpp"
#include "./webpp_scheme.hpp"

using namespace webpp;


static std::vector<std::string_view> schemes{{"one",
                                              "two",
                                              "three",
                                              "http",
                                              "five",
                                              "https",
                                              "https",
                                              "ftp",
                                              "ftps",
                                              "wss",
                                              "file",
                                              "ssh",
                                              "scheme",
                                              "nonsense",
                                              "",
                                              "     ",
                                              "d;klasjfd;alsjf",
                                              "2408372-54",
                                              " @#$@fdsafd"}};

static void WebppSchemePort(benchmark::State& state) {
    std::size_t i = 0;
    for (auto _ : state) {
        auto scheme = schemes[i++ % schemes.size()];
        benchmark::DoNotOptimize(scheme);
        auto w_scheme = webpp::v1::basic_scheme<std::string_view>{scheme};
        auto port     = w_scheme.known_port();
        benchmark::DoNotOptimize(port);
    }
}
BENCHMARK(WebppSchemePort);


static void AdaSchemePort(benchmark::State& state) {
    std::size_t i = 0;
    for (auto _ : state) {
        auto scheme = schemes[i++ % schemes.size()];
        benchmark::DoNotOptimize(scheme);
        auto port = ada::scheme::get_special_port(scheme);
        benchmark::DoNotOptimize(port);
    }
}
BENCHMARK(AdaSchemePort);
