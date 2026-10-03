// Registration helpers for the ada_url comparison suite.
//
// Benchmark names follow "piece/impl/config/input" (exactly four slash-separated
// parts). The ada ref is not part of the name: it is encoded in the results file
// name (results/<ref-id>.json), so tools/merge-benchmark-results.mjs can build
// one column per ada version.

#ifndef WEBPP_BENCH_ADA_URL_COMMON_HPP
#define WEBPP_BENCH_ADA_URL_COMMON_HPP

#include "../benchmark.hpp" // IWYU pragma: export  (pulls <benchmark/benchmark.h>)

#include <string>
#include <string_view>
#include <utility>

namespace bench {

    using benchmark::State;

    /// Register one benchmark whose name is the four-part contract joined with '/'.
    ///
    /// The return type is deduced: google/benchmark v1.9 moved `Benchmark` into
    /// `benchmark::internal`, while older releases expose `benchmark::Benchmark`.
    template <class Fn>
    auto
    add(std::string_view piece, std::string_view impl, std::string_view config, std::string_view input, Fn&& func) {
        std::string name;
        name.reserve(piece.size() + impl.size() + config.size() + input.size() + 4);
        name.append(piece).append("/").append(impl).append("/").append(config).append("/").append(input);
        return benchmark::RegisterBenchmark(std::move(name), std::forward<Fn>(func))->Unit(benchmark::kNanosecond);
    }

    /// Register one benchmark per corpus entry: fn receives (state, entry).
    ///
    /// The corpus must outlive the benchmark run; the corpora in inputs.hpp are
    /// inline-constexpr, so their addresses are stable.
    template <class Corpus, class Fn>
    void
    add_all(std::string_view piece, std::string_view impl, std::string_view config, Corpus const& corpus, Fn&& func) {
        for (auto const& entry : corpus) {
            add(piece, impl, config, entry.slug, [func = std::forward<Fn>(func), &entry](State& state) {
                func(state, entry);
            });
        }
    }

} // namespace bench

#endif // WEBPP_BENCH_ADA_URL_COMMON_HPP
