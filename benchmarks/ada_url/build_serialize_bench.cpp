// Setter-chain ("build") and href serialization comparison.
//
// build: a base URL is parsed once outside the loop; each iteration applies the
// full public setter chain. webpp clears the path before setting it because its
// path() setter appends (see README); the rest of the chain is plain replace.
//
// serialize_href: webpp's as_string() returns a std::string by value, matching
// ada::url::get_href(); ada::url_aggregator::get_href() returns a string_view and
// is reported as its own config.

#include "adapters/ada_adapter.hpp"
#include "adapters/webpp_adapter.hpp"
#include "bench_common.hpp"
#include "inputs.hpp"

#include <string_view>

namespace {

    using namespace bench;

    inline constexpr std::string_view build_username = "alice";
    inline constexpr std::string_view build_password = "p@ss word";
    inline constexpr std::string_view build_hostname = "user.example.com";
    inline constexpr std::string_view build_path     = "/deep/path/to/thing.html";
    inline constexpr std::string_view build_queries  = "a=1&b=2";
    inline constexpr std::string_view build_fragment = "build-frag";
    inline constexpr std::string_view build_port     = "8080";

    inline constexpr std::array<inputs::corpus_entry, 1> build_corpus{
      {
       {.slug = "default", .value = ""},
       }
    };

    [[maybe_unused]] bool const register_build = [] {
        // ---- build ----
        add_all("build", "webpp", "setters", build_corpus, [](State& state, auto const&) {
            auto url = webpp_side::make_url(inputs::base_build);
            for (auto _ : state) {
                auto user_status = url.username(build_username);
                auto pass_status = url.password(build_password);
                auto host_status = url.hostname(build_hostname);
                url.clear_path();
                auto path_status  = url.path(build_path);
                auto query_status = url.queries(build_queries);
                auto frag_status  = url.fragment(build_fragment);
                auto port_status  = url.port(build_port);
                benchmark::DoNotOptimize(user_status);
                benchmark::DoNotOptimize(pass_status);
                benchmark::DoNotOptimize(host_status);
                benchmark::DoNotOptimize(path_status);
                benchmark::DoNotOptimize(query_status);
                benchmark::DoNotOptimize(frag_status);
                benchmark::DoNotOptimize(port_status);
                benchmark::DoNotOptimize(url);
            }
        });

        add_all("build", "ada", "setters", build_corpus, [](State& state, auto const&) {
            auto url = ada_side::make_url<ada::url_aggregator>(inputs::base_build);
            for (auto _ : state) {
                auto user_ok = url.set_username(build_username);
                auto pass_ok = url.set_password(build_password);
                auto host_ok = url.set_hostname(build_hostname);
                auto path_ok = url.set_pathname(build_path);
                url.set_search(build_queries);
                url.set_hash(build_fragment);
                auto port_ok = url.set_port(build_port);
                benchmark::DoNotOptimize(user_ok);
                benchmark::DoNotOptimize(pass_ok);
                benchmark::DoNotOptimize(host_ok);
                benchmark::DoNotOptimize(path_ok);
                benchmark::DoNotOptimize(port_ok);
                benchmark::DoNotOptimize(url);
            }
        });

        // ---- serialize href ----
        add_all("serialize_href", "webpp", "as_string", inputs::serialize_href, [](State& state, auto const& item) {
            auto url = webpp_side::make_url(item.value);
            for (auto _ : state) {
                auto text = url.as_string();
                benchmark::DoNotOptimize(text);
            }
        });

        add_all("serialize_href", "ada", "url", inputs::serialize_href, [](State& state, auto const& item) {
            auto url = ada_side::make_url<ada::url>(item.value);
            for (auto _ : state) {
                auto text = url.get_href();
                benchmark::DoNotOptimize(text);
            }
        });

        add_all("serialize_href", "ada", "url_aggregator", inputs::serialize_href, [](State& state, auto const& item) {
            auto url = ada_side::make_url<ada::url_aggregator>(item.value);
            for (auto _ : state) {
                auto text = url.get_href();
                benchmark::DoNotOptimize(text);
            }
        });

        return true;
    }();

} // namespace
