#include "../../webpp/ip/ipv4.hpp"
#include "../../webpp/ip/ipv6.hpp"
#include "../../webpp/traits/std_traits.hpp"
#include "../benchmark.hpp"
#include "./inet_pton4_parser.hpp"
#include "./ipv4_host_parser.hpp"
#include "./ipv4_v1.hpp"
#include "./ipv4_v2.hpp"

#include <arpa/inet.h>

using namespace std;
#ifdef webpp_has_boost
using namespace boost::asio;
#endif


// ============================================================================
// Blog post parsers from https://www.yagiz.co/simd-is-the-wrong-way-to-parse-ipv4
// ============================================================================

static constexpr uint64_t parse_ipv4_fail = static_cast<uint64_t>(1) << 32;

/// Naive loop parser from the blog post (inner while per octet).
static uint64_t parse_ipv4_naive(char const* p, char const* end) noexcept {
    uint32_t addr = 0;
    for (int i = 0; i < 4; ++i) {
        if (p == end || *p < '0' || *p > '9') {
            return parse_ipv4_fail;
        }
        uint32_t val = static_cast<uint32_t>(*p++ - '0');
        while (p < end && *p >= '0' && *p <= '9') {
            val = val * 10 + static_cast<uint32_t>(*p++ - '0');
            if (val > 255) {
                return parse_ipv4_fail;
            }
        }
        addr = (addr << 8) | val;
        if (i < 3) {
            if (p == end || *p != '.') {
                return parse_ipv4_fail;
            }
            ++p;
        }
    }
    return (p == end) ? addr : parse_ipv4_fail;
}

/// Unrolled scalar parser from the blog post (no inner while, explicit digit handling).
static uint64_t parse_ipv4_unrolled(char const* p, char const* end) noexcept {
    uint32_t addr = 0;
    for (int i = 0; i < 4; ++i) {
        if (p == end || *p < '0' || *p > '9') {
            return parse_ipv4_fail;
        }
        uint32_t val = static_cast<uint32_t>(*p++ - '0');
        if (p < end && *p >= '0' && *p <= '9') {
            if (val == 0) {
                return parse_ipv4_fail; // leading zero: not decimal
            }
            val = val * 10 + static_cast<uint32_t>(*p++ - '0');
            if (p < end && *p >= '0' && *p <= '9') {
                val = val * 10 + static_cast<uint32_t>(*p++ - '0');
                if (val > 255) {
                    return parse_ipv4_fail;
                }
            }
        }
        addr = (addr << 8) | val;
        if (i < 3) {
            if (p == end || *p != '.') {
                return parse_ipv4_fail;
            }
            ++p;
        }
    }
    if (p == end) {
        return addr;
    }
    if (p + 1 == end && *p == '.') {
        return addr; // trailing dot
    }
    return parse_ipv4_fail;
}

#ifdef webpp_has_boost
static void IP_asio_v4(benchmark::State& state) {
    for (auto _ : state) {
        auto addr = ip::make_address_v4("192.168.1.8");
        benchmark::DoNotOptimize(addr);
    }
}

BENCHMARK(IP_asio_v4);
#endif

static void IP_webpp_v4(benchmark::State& state) {
    for (auto _ : state) {
        auto addr = webpp::ipv4("192.168.1.8");
        benchmark::DoNotOptimize(addr);
    }
}

BENCHMARK(IP_webpp_v4);

static void IP_webpp_host_ipv4(benchmark::State& state) {
    std::array<uint8_t, 4> out{};
    std::string_view const ip_str = "192.168.1.8";
    for (auto _ : state) {
        bool res = webpp::v1::parse_host_ipv4(ip_str.begin(), ip_str.end(), out.data());
        benchmark::DoNotOptimize(res);
        benchmark::DoNotOptimize(out);
    }
}

BENCHMARK(IP_webpp_host_ipv4);

static void IP_webpp_host_ipv4_v2(benchmark::State& state) {
    std::array<uint8_t, 4> out{};
    std::string_view const ip_str = "192.168.1.8";
    for (auto _ : state) {
        bool res = webpp::v2::parse_host_ipv4(ip_str.begin(), ip_str.end(), out.data());
        benchmark::DoNotOptimize(res);
        benchmark::DoNotOptimize(out);
    }
}

BENCHMARK(IP_webpp_host_ipv4_v2);

static void IP_webpp_host_ipv4_v3(benchmark::State& state) {
    std::array<uint8_t, 4> out{};
    std::string_view const ip_str = "192.168.1.8";
    for (auto _ : state) {
        bool res = webpp::v3::parse_host_ipv4(ip_str.begin(), ip_str.end(), out.data());
        benchmark::DoNotOptimize(res);
        benchmark::DoNotOptimize(out);
    }
}

BENCHMARK(IP_webpp_host_ipv4_v3);

static void IP_webpp_host_ipv4_v4(benchmark::State& state) {
    std::array<uint8_t, 4> out{};
    std::string_view const ip_str = "192.168.1.8";
    for (auto _ : state) {
        bool res = webpp::v4::parse_host_ipv4(ip_str.begin(), ip_str.end(), out.data());
        benchmark::DoNotOptimize(res);
        benchmark::DoNotOptimize(out);
    }
}

BENCHMARK(IP_webpp_host_ipv4_v4);

// --------------- Blog post parsers (fixed string) ---------------

static void IP_naive_loop(benchmark::State& state) {
    std::string_view const ip_str = "192.168.1.8";
    for (auto _ : state) {
        auto addr = parse_ipv4_naive(ip_str.data(), std::next(ip_str.data(), ip_str.size()));
        benchmark::DoNotOptimize(addr);
    }
}

BENCHMARK(IP_naive_loop);

static void IP_unrolled_scalar(benchmark::State& state) {
    std::string_view const ip_str = "192.168.1.8";
    for (auto _ : state) {
        auto addr = parse_ipv4_unrolled(ip_str.data(), std::next(ip_str.data(), ip_str.size()));
        benchmark::DoNotOptimize(addr);
    }
}

BENCHMARK(IP_unrolled_scalar);

static void IP_glibc_inet_pton(benchmark::State& state) {
    in_addr result{};
    for (auto _ : state) {
        auto res = ::inet_pton(AF_INET, "192.168.1.8", &result);
        benchmark::DoNotOptimize(res);
        benchmark::DoNotOptimize(result);
    }
}

BENCHMARK(IP_glibc_inet_pton);

static void IP_webpp_inet_pton4(benchmark::State& state) {
    std::string_view const ip_str = "192.168.1.8";
    for (auto _ : state) {
        auto                   src = ip_str.begin();
        std::array<uint8_t, 4> out{};
        auto                   res = webpp::inet_pton4(src, ip_str.end(), out.data());
        benchmark::DoNotOptimize(res);
        benchmark::DoNotOptimize(out);
    }
}

BENCHMARK(IP_webpp_inet_pton4);

/////////////////// Random ///////////////////////////

auto ipv4_rands() {
    vector<string> ipv4_rand;
    for (int i = 0; i < 255; i++) {
        string str = "192.168.1.";
        str.append(to_string(i));
        ipv4_rand.push_back(std::move(str));
    }
    return ipv4_rand;
}

auto ipv4_data() {
    static auto data = ipv4_rands();
    static auto it   = data.begin();
    ++it;
    if (it == data.end()) {
        it = data.begin();
    }
    return *it;
}

#ifdef webpp_has_boost
static void IP_asio_v4_random(benchmark::State& state) {
    ipv4_data();
    for (auto _ : state) {
        auto addr = ip::make_address_v4(ipv4_data());
        benchmark::DoNotOptimize(addr);
    }
}

BENCHMARK(IP_asio_v4_random);
#endif

static void IP_webpp_v4_random(benchmark::State& state) {
    ipv4_data();
    for (auto _ : state) {
        auto addr = webpp::ipv4(ipv4_data());
        benchmark::DoNotOptimize(addr);
    }
}

BENCHMARK(IP_webpp_v4_random);

static void IP_webpp_host_v4_random(benchmark::State& state) {
    ipv4_data();
    std::array<uint8_t, 4> out{};
    for (auto _ : state) {
        auto const ip_str = ipv4_data();
        bool       res    = webpp::v1::parse_host_ipv4(ip_str.begin(), ip_str.end(), out.data());
        benchmark::DoNotOptimize(res);
        benchmark::DoNotOptimize(out);
    }
}

BENCHMARK(IP_webpp_host_v4_random);

static void IP_webpp_host_v4_random_v2(benchmark::State& state) {
    ipv4_data();
    std::array<uint8_t, 4> out{};
    for (auto _ : state) {
        auto const ip_str = ipv4_data();
        bool       res    = webpp::v2::parse_host_ipv4(ip_str.begin(), ip_str.end(), out.data());
        benchmark::DoNotOptimize(res);
        benchmark::DoNotOptimize(out);
    }
}

BENCHMARK(IP_webpp_host_v4_random_v2);

static void IP_webpp_host_v4_random_v3(benchmark::State& state) {
    ipv4_data();
    std::array<uint8_t, 4> out{};
    for (auto _ : state) {
        auto const ip_str = ipv4_data();
        bool       res    = webpp::v3::parse_host_ipv4(ip_str.begin(), ip_str.end(), out.data());
        benchmark::DoNotOptimize(res);
        benchmark::DoNotOptimize(out);
    }
}

BENCHMARK(IP_webpp_host_v4_random_v3);

static void IP_webpp_host_v4_random_v4(benchmark::State& state) {
    ipv4_data();
    std::array<uint8_t, 4> out{};
    for (auto _ : state) {
        auto const ip_str = ipv4_data();
        bool       res    = webpp::v4::parse_host_ipv4(ip_str.begin(), ip_str.end(), out.data());
        benchmark::DoNotOptimize(res);
        benchmark::DoNotOptimize(out);
    }
}

BENCHMARK(IP_webpp_host_v4_random_v4);

static void IP_webpp_v4_random_inet_pton4_v1(benchmark::State& state) {
    ipv4_data();
    for (auto _ : state) {
        auto const        ip = ipv4_data();
        array<uint8_t, 4> out_ip;
        auto              addr = v1::inet_pton4(ip.data(), ip.data() + ip.size(), out_ip.data());
        benchmark::DoNotOptimize(addr);
        benchmark::DoNotOptimize(out_ip);
    }
}

BENCHMARK(IP_webpp_v4_random_inet_pton4_v1);

static void IP_webpp_v4_random_inet_pton4_v2(benchmark::State& state) {
    ipv4_data();
    for (auto _ : state) {
        auto const        ip = ipv4_data();
        array<uint8_t, 4> out_ip;
        auto              addr = v2::inet_pton4(ip.data(), ip.data() + ip.size(), out_ip.data());
        benchmark::DoNotOptimize(addr);
        benchmark::DoNotOptimize(out_ip);
    }
}

BENCHMARK(IP_webpp_v4_random_inet_pton4_v2);

static void IP_webpp_v4_random_ipv4_v1(benchmark::State& state) {
    ipv4_data();
    for (auto _ : state) {
        auto addr = webpp::v1::ipv4(ipv4_data());
        benchmark::DoNotOptimize(addr);
    }
}

BENCHMARK(IP_webpp_v4_random_ipv4_v1);

static void IP_webpp_v4_random_ipv4_v2(benchmark::State& state) {
    ipv4_data();
    for (auto _ : state) {
        auto addr = webpp::v2::ipv4(ipv4_data());
        benchmark::DoNotOptimize(addr);
    }
}

BENCHMARK(IP_webpp_v4_random_ipv4_v2);

// --------------- Blog post parsers (random) ---------------

static void IP_naive_loop_random(benchmark::State& state) {
    ipv4_data();
    for (auto _ : state) {
        auto const ip   = ipv4_data();
        auto       addr = parse_ipv4_naive(ip.data(), ip.data() + ip.size());
        benchmark::DoNotOptimize(addr);
    }
}

BENCHMARK(IP_naive_loop_random);

static void IP_unrolled_scalar_random(benchmark::State& state) {
    ipv4_data();
    for (auto _ : state) {
        auto const ip   = ipv4_data();
        auto       addr = parse_ipv4_unrolled(ip.data(), ip.data() + ip.size());
        benchmark::DoNotOptimize(addr);
    }
}

BENCHMARK(IP_unrolled_scalar_random);

static void IP_glibc_inet_pton_random(benchmark::State& state) {
    ipv4_data();
    in_addr result{};
    for (auto _ : state) {
        auto const ip  = ipv4_data();
        auto       res = ::inet_pton(AF_INET, ip.c_str(), &result);
        benchmark::DoNotOptimize(res);
        benchmark::DoNotOptimize(result);
    }
}

BENCHMARK(IP_glibc_inet_pton_random);

static void IP_webpp_inet_pton4_random(benchmark::State& state) {
    ipv4_data();
    for (auto _ : state) {
        auto const             ip  = ipv4_data();
        auto                   src = ip.begin();
        std::array<uint8_t, 4> out;
        auto                   res = webpp::inet_pton4(src, ip.end(), out.data());
        benchmark::DoNotOptimize(res);
        benchmark::DoNotOptimize(out);
    }
}

BENCHMARK(IP_webpp_inet_pton4_random);

////////////////////////////// Prefix //////////////////////////////


static void ipv4_prefix_parsing_v1(benchmark::State& state) {
    ipv4_data();
    for (auto _ : state) {
        auto res = webpp::v1::to_prefix(ipv4_data());
        benchmark::DoNotOptimize(res);
    }
}

BENCHMARK(ipv4_prefix_parsing_v1);

static void ipv4_prefix_parsing_v2(benchmark::State& state) {
    ipv4_data();
    for (auto _ : state) {
        auto res = webpp::v2::to_prefix(ipv4_data());
        benchmark::DoNotOptimize(res);
    }
}

BENCHMARK(ipv4_prefix_parsing_v2);
