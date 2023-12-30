#include "../../webpp/ip/ipv4.hpp"
#include "../../webpp/ip/ipv6.hpp"
#include "../../webpp/traits/std_traits.hpp"
#include "../benchmark.hpp"
#include "../boost_pch.hpp"
#include "./inet_pton4_parser.hpp"
#include "./ipv4_host_parser.hpp"
#include "./ipv4_v1.hpp"
#include "./ipv4_v2.hpp"


using namespace std;
#ifdef webpp_has_boost
using namespace boost::asio;
#endif

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

static void IP_webpp_host_v4(benchmark::State& state) {
    std::array<uint8_t, 4> out{};
    std::string_view const ip_str = "192.168.1.8";
    for (auto _ : state) {
        bool res = webpp::v1::parse_host_ipv4(ip_str.begin(), ip_str.end(), out.data());
        benchmark::DoNotOptimize(res);
        benchmark::DoNotOptimize(out);
    }
}

BENCHMARK(IP_webpp_host_v4);

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
