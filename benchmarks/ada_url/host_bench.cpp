// Scheme, host, and IDNA comparison.
//
// Scheme inputs are colon-terminated because webpp's scheme() setter is the raw
// WHATWG scheme-state primitive, while ada's set_protocol() wraps it with the HTML
// "append ':'" rule; feeding both "https:"-style input keeps the comparison honest.

#include "adapters/ada_adapter.hpp"
#include "adapters/webpp_adapter.hpp"
#include "bench_common.hpp"
#include "inputs.hpp"

namespace {

    using namespace bench;

    [[maybe_unused]] bool const register_host = [] {
        // ---- scheme ----
        add_all("scheme", "webpp", "direct", inputs::scheme, [](State& state, auto const& item) {
            for (auto _ : state) {
                auto ctx = webpp_side::scheme_piece(item.value);
                benchmark::DoNotOptimize(ctx);
            }
        });

        add_all("scheme", "webpp", "setter", inputs::scheme, [](State& state, auto const& item) {
            auto url = webpp_side::make_url(inputs::base_scheme);
            for (auto _ : state) {
                auto const status = url.scheme(item.value);
                benchmark::DoNotOptimize(status);
            }
        });

        add_all("scheme", "ada", "setter", inputs::scheme, [](State& state, auto const& item) {
            auto url = ada_side::make_url<ada::url_aggregator>(inputs::base_scheme);
            for (auto _ : state) {
                auto const ok = url.set_protocol(item.value);
                benchmark::DoNotOptimize(ok);
            }
        });

        // ---- host ----
        add_all("host", "webpp", "direct", inputs::host, [](State& state, auto const& item) {
            for (auto _ : state) {
                auto ctx = webpp_side::host_piece(item.value);
                benchmark::DoNotOptimize(ctx);
            }
        });

        // webpp's hostname() continues past the host into path-start under state
        // override (spec: host state returns), appending one empty segment per call;
        // clear_path() keeps the state stable across iterations (see README).
        add_all("host", "webpp", "setter", inputs::host, [](State& state, auto const& item) {
            auto url = webpp_side::make_url(inputs::base_host);
            for (auto _ : state) {
                auto const status = url.hostname(item.value);
                url.clear_path();
                benchmark::DoNotOptimize(status);
            }
        });

        add_all("host", "ada", "setter", inputs::host, [](State& state, auto const& item) {
            auto url = ada_side::make_url<ada::url_aggregator>(inputs::base_host);
            for (auto _ : state) {
                auto const ok = url.set_hostname(item.value);
                benchmark::DoNotOptimize(ok);
            }
        });

        // ---- domain to ascii ----
        add_all("domain_to_ascii", "webpp", "direct", inputs::domain_to_ascii, [](State& state, auto const& item) {
            std::string out;
            for (auto _ : state) {
                webpp_side::domain_to_ascii(item.value, out);
                benchmark::DoNotOptimize(out);
            }
        });

        add_all("domain_to_ascii", "ada", "to_ascii", inputs::domain_to_ascii, [](State& state, auto const& item) {
            std::string out;
            for (auto _ : state) {
                ada_side::to_ascii(item.value, out);
                benchmark::DoNotOptimize(out);
            }
        });

        return true;
    }();

} // namespace
