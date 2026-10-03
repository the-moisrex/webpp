// Full-parse comparison: webpp's parser over its component types/configs versus
// ada::parse<ada::url> and ada::parse<ada::url_aggregator>.

#include "adapters/ada_adapter.hpp"
#include "adapters/webpp_adapter.hpp"
#include "bench_common.hpp"
#include "inputs.hpp"

namespace {

    using namespace bench;

    [[maybe_unused]] bool const register_full_parse = [] {
        add_all("full_parse", "webpp", "owning_standard", inputs::full_parse, [](State& state, auto const& item) {
            for (auto _ : state) {
                auto ctx = webpp_side::full_parse_owning<webpp_side::standard_opts>(item.value);
                benchmark::DoNotOptimize(ctx);
            }
        });

        add_all("full_parse", "webpp", "structured_standard", inputs::full_parse, [](State& state, auto const& item) {
            for (auto _ : state) {
                auto ctx = webpp_side::full_parse_structured<webpp_side::standard_opts>(item.value);
                benchmark::DoNotOptimize(ctx);
            }
        });

        add_all("full_parse", "webpp", "owning_strict", inputs::full_parse, [](State& state, auto const& item) {
            for (auto _ : state) {
                auto ctx = webpp_side::full_parse_owning<webpp_side::strict_opts>(item.value);
                benchmark::DoNotOptimize(ctx);
            }
        });

        add_all("full_parse", "webpp", "owning_loose", inputs::full_parse, [](State& state, auto const& item) {
            for (auto _ : state) {
                auto ctx = webpp_side::full_parse_owning<webpp_side::loose_opts>(item.value);
                benchmark::DoNotOptimize(ctx);
            }
        });

        add_all("full_parse", "ada", "url", inputs::full_parse, [](State& state, auto const& item) {
            for (auto _ : state) {
                auto result = ada_side::full_parse<ada::url>(item.value);
                benchmark::DoNotOptimize(result);
            }
        });

        add_all("full_parse", "ada", "url_aggregator", inputs::full_parse, [](State& state, auto const& item) {
            for (auto _ : state) {
                auto result = ada_side::full_parse<ada::url_aggregator>(item.value);
                benchmark::DoNotOptimize(result);
            }
        });

        return true;
    }();

} // namespace
