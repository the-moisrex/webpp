// IPv4/IPv6 parsing and formatting comparison.
//
// Row notes:
// - webpp/host_ipv4 is webpp's WHATWG host-IPv4 parser (hex/octal octets allowed);
//   webpp/pton is inet_pton4/6, the strict RFC parser.
// - ada/try_parse_ipv4_fast is ada's decimal fast path (defined in the installed
//   checkers-inl.h, so it links from outside libada); ada's sibling syntactic
//   pre-check checkers::is_ipv4 is always_inline with its body in ada's .cpp and
//   cannot be called from an external TU (see README), so there is no is_ipv4 row.
// - The fast path only exists in ada >= v4.0.0 (WEBPP_BENCH_ADA_HAS_IPV4_FAST is
//   set per ref by CMake from the ref's version string).

#include "adapters/ada_adapter.hpp"
#include "adapters/webpp_adapter.hpp"
#include "bench_common.hpp"
#include "inputs.hpp"

namespace {

    using namespace bench;

    [[maybe_unused]] bool const register_ip = [] {
        // ---- ipv4 ----
        add_all("ipv4", "webpp", "pton", inputs::ipv4, [](State& state, auto const& item) {
            for (auto _ : state) {
                auto const ok = webpp_side::pton4(item.value);
                benchmark::DoNotOptimize(ok);
            }
        });

        add_all("ipv4", "webpp", "host_ipv4", inputs::ipv4, [](State& state, auto const& item) {
            for (auto _ : state) {
                auto const ok = webpp_side::host_ipv4_piece(item.value);
                benchmark::DoNotOptimize(ok);
            }
        });

        add_all("ipv4", "webpp", "setter", inputs::ipv4_setter, [](State& state, auto const& item) {
            auto url = webpp_side::make_url(inputs::base_host);
            for (auto _ : state) {
                auto const status = url.hostname(item.value);
                url.clear_path();
                benchmark::DoNotOptimize(status);
            }
        });

        add_all("ipv4", "ada", "setter", inputs::ipv4_setter, [](State& state, auto const& item) {
            auto url = ada_side::make_url<ada::url_aggregator>(inputs::base_host);
            for (auto _ : state) {
                auto const ok = url.set_hostname(item.value);
                benchmark::DoNotOptimize(ok);
            }
        });

#if WEBPP_BENCH_ADA_HAS_IPV4_FAST
        // try_parse_ipv4_fast is defined in the installed checkers-inl.h (ada >= v4),
        // so it is callable from outside libada.
        add_all("ipv4", "ada", "try_parse_fast", inputs::ipv4, [](State& state, auto const& item) {
            for (auto _ : state) {
                auto const packed = ada::checkers::try_parse_ipv4_fast(item.value);
                benchmark::DoNotOptimize(packed);
            }
        });
#endif

        // ---- ipv6 ----
        add_all("ipv6", "webpp", "pton", inputs::ipv6, [](State& state, auto const& item) {
            for (auto _ : state) {
                auto const ok = webpp_side::pton6(item.value);
                benchmark::DoNotOptimize(ok);
            }
        });

        add_all("ipv6", "webpp", "setter", inputs::ipv6_setter, [](State& state, auto const& item) {
            auto url = webpp_side::make_url(inputs::base_host);
            for (auto _ : state) {
                auto const status = url.hostname(item.value);
                url.clear_path();
                benchmark::DoNotOptimize(status);
            }
        });

        add_all("ipv6", "ada", "setter", inputs::ipv6_setter, [](State& state, auto const& item) {
            auto url = ada_side::make_url<ada::url_aggregator>(inputs::base_host);
            for (auto _ : state) {
                auto const ok = url.set_hostname(item.value);
                benchmark::DoNotOptimize(ok);
            }
        });

        // ---- ipv4 serialization ----
        add_all("serialize_ipv4", "webpp", "ntop", inputs::ip4_addresses, [](State& state, auto const& entry) {
            char buffer[32]{};
            for (auto _ : state) {
                auto const size = webpp_side::ntop4(entry.bytes, buffer);
                benchmark::DoNotOptimize(size);
                benchmark::DoNotOptimize(buffer);
            }
        });

        add_all("serialize_ipv4", "ada", "serializers", inputs::ip4_addresses, [](State& state, auto const& entry) {
            auto const packed = inputs::packed_ip4(entry);
            for (auto _ : state) {
                auto text = ada_side::ipv4_to_string(packed);
                benchmark::DoNotOptimize(text);
            }
        });

        // ---- ipv6 serialization ----
        add_all("serialize_ipv6", "webpp", "ntop", inputs::ip6_addresses, [](State& state, auto const& entry) {
            char buffer[64]{};
            for (auto _ : state) {
                auto const size = webpp_side::ntop6(entry.bytes, buffer);
                benchmark::DoNotOptimize(size);
                benchmark::DoNotOptimize(buffer);
            }
        });

        add_all("serialize_ipv6", "ada", "serializers", inputs::ip6_addresses, [](State& state, auto const& entry) {
            auto const pieces = inputs::ip6_pieces(entry);
            for (auto _ : state) {
                auto text = ada_side::ipv6_to_string(pieces);
                benchmark::DoNotOptimize(text);
            }
        });

        return true;
    }();

} // namespace
