// Percent encoding/decoding comparison (query percent-encode set on both sides).
//
// Asymmetries documented in the README:
// - webpp's encoder writes into a caller-owned string (cleared per iteration);
//   ada's percent_encode<false> overwrites its caller-owned output.
// - webpp's decoder is the decode_percent_encoded primitive driven by the same
//   loop pattern its host parser uses; ada's percent_decode scans from the first
//   '%' itself (we pass input.find('%') per its API contract).
// - Corpora are ASCII: webpp's encode_uri_component only touches bytes that are in
//   the given set, while ada also UTF-8-encodes non-ASCII code points.

#include "adapters/ada_adapter.hpp"
#include "adapters/webpp_adapter.hpp"
#include "bench_common.hpp"
#include "inputs.hpp"

#include <string>

namespace {

    using namespace bench;

    [[maybe_unused]] bool const register_percent = [] {
        add_all("percent_encode", "webpp", "direct", inputs::percent_encode, [](State& state, auto const& item) {
            std::string out;
            for (auto _ : state) {
                webpp_side::percent_encode(item.value, out);
                benchmark::DoNotOptimize(out);
            }
        });

        add_all("percent_encode", "ada", "direct", inputs::percent_encode, [](State& state, auto const& item) {
            std::string out;
            for (auto _ : state) {
                ada_side::percent_encode(item.value, out);
                benchmark::DoNotOptimize(out);
            }
        });

        add_all("percent_decode", "webpp", "direct", inputs::percent_decode, [](State& state, auto const& item) {
            std::string out;
            for (auto _ : state) {
                auto ok = webpp_side::percent_decode(item.value, out);
                benchmark::DoNotOptimize(ok);
                benchmark::DoNotOptimize(out);
            }
        });

        add_all("percent_decode", "ada", "direct", inputs::percent_decode, [](State& state, auto const& item) {
            for (auto _ : state) {
                auto text = ada_side::percent_decode(item.value);
                benchmark::DoNotOptimize(text);
            }
        });

        return true;
    }();

} // namespace
