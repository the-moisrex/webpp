// Path, query, fragment, port, and credentials comparison.
//
// Direct parser rows only exist for webpp (ada exposes no per-piece public parser
// for these states); setter rows exist on both sides.
//
// Path inputs differ by config on purpose: the direct parser sees the path the way
// the state machine does after path-start consumed the leading slash (no slash),
// while setters receive user-shaped input with the slash, exactly like
// url.pathname = "/..." / ada::url::set_pathname("/...").

#include "adapters/ada_adapter.hpp"
#include "adapters/webpp_adapter.hpp"
#include "bench_common.hpp"
#include "inputs.hpp"

namespace {

    using namespace bench;

    [[maybe_unused]] bool const register_path = [] {
        // ---- path ----
        add_all("path", "webpp", "direct", inputs::path_direct, [](State& state, auto const& item) {
            for (auto _ : state) {
                auto ctx = webpp_side::path_piece(item.value);
                benchmark::DoNotOptimize(ctx);
            }
        });

        // webpp's path() setter appends to the existing path; the WHATWG pathname
        // setter empties it first, so clear_path() mirrors that contract (see README).
        add_all("path", "webpp", "setter", inputs::path_setter, [](State& state, auto const& item) {
            auto url = webpp_side::make_url(inputs::base_host);
            for (auto _ : state) {
                url.clear_path();
                auto status = url.path(item.value);
                benchmark::DoNotOptimize(status);
            }
        });

        add_all("path", "ada", "setter", inputs::path_setter, [](State& state, auto const& item) {
            auto url = ada_side::make_url<ada::url_aggregator>(inputs::base_host);
            for (auto _ : state) {
                auto ok = url.set_pathname(item.value);
                benchmark::DoNotOptimize(ok);
            }
        });

        // ---- queries ----
        add_all("query", "webpp", "direct", inputs::query, [](State& state, auto const& item) {
            for (auto _ : state) {
                auto ctx = webpp_side::queries_piece(item.value);
                benchmark::DoNotOptimize(ctx);
            }
        });

        add_all("query", "webpp", "setter", inputs::query, [](State& state, auto const& item) {
            auto url = webpp_side::make_url(inputs::base_query);
            for (auto _ : state) {
                auto status = url.queries(item.value);
                benchmark::DoNotOptimize(status);
            }
        });

        add_all("query", "ada", "setter", inputs::query, [](State& state, auto const& item) {
            auto url = ada_side::make_url<ada::url_aggregator>(inputs::base_query);
            for (auto _ : state) {
                url.set_search(item.value);
                benchmark::DoNotOptimize(url);
            }
        });

        // ---- fragment ----
        add_all("fragment", "webpp", "direct", inputs::fragment, [](State& state, auto const& item) {
            for (auto _ : state) {
                auto ctx = webpp_side::fragment_piece(item.value);
                benchmark::DoNotOptimize(ctx);
            }
        });

        add_all("fragment", "webpp", "setter", inputs::fragment, [](State& state, auto const& item) {
            auto url = webpp_side::make_url(inputs::base_query);
            for (auto _ : state) {
                auto status = url.fragment(item.value);
                benchmark::DoNotOptimize(status);
            }
        });

        add_all("fragment", "ada", "setter", inputs::fragment, [](State& state, auto const& item) {
            auto url = ada_side::make_url<ada::url_aggregator>(inputs::base_query);
            for (auto _ : state) {
                url.set_hash(item.value);
                benchmark::DoNotOptimize(url);
            }
        });

        // ---- port ----
        add_all("port", "webpp", "direct", inputs::port_direct, [](State& state, auto const& item) {
            for (auto _ : state) {
                auto ctx = webpp_side::port_piece(item.value);
                benchmark::DoNotOptimize(ctx);
            }
        });

        add_all("port", "webpp", "setter", inputs::port_setter, [](State& state, auto const& item) {
            auto url = webpp_side::make_url(inputs::base_port);
            for (auto _ : state) {
                auto status = url.port(item.value);
                benchmark::DoNotOptimize(status);
            }
        });

        add_all("port", "ada", "setter", inputs::port_setter, [](State& state, auto const& item) {
            auto url = ada_side::make_url<ada::url_aggregator>(inputs::base_port);
            for (auto _ : state) {
                auto ok = url.set_port(item.value);
                benchmark::DoNotOptimize(ok);
            }
        });

        // ---- credentials ----
        add_all("credentials", "webpp", "setter", inputs::credentials, [](State& state, auto const& item) {
            auto url = webpp_side::make_url(inputs::base_creds);
            for (auto _ : state) {
                auto user_status = url.username(item.username);
                auto pass_status = url.password(item.password);
                benchmark::DoNotOptimize(user_status);
                benchmark::DoNotOptimize(pass_status);
            }
        });

        add_all("credentials", "ada", "setter", inputs::credentials, [](State& state, auto const& item) {
            auto url = ada_side::make_url<ada::url_aggregator>(inputs::base_creds);
            for (auto _ : state) {
                auto user_ok = url.set_username(item.username);
                auto pass_ok = url.set_password(item.password);
                benchmark::DoNotOptimize(user_ok);
                benchmark::DoNotOptimize(pass_ok);
            }
        });

        return true;
    }();

} // namespace
