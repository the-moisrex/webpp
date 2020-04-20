#include "utils_address_pch.h"
using namespace std;
using namespace boost::asio;

static void ip_asio_v4(benchmark::State& state) {
    for (auto _ : state) {
        auto addr = ip::make_address_v4("192.168.1.8");
        benchmark::DoNotOptimize(addr);
    }
}
BENCHMARK(ip_asio_v4);

static void ip_webpp_v4(benchmark::State& state) {
    for (auto _ : state) {
        auto addr = webpp::ipv4("192.168.1.8");
        benchmark::DoNotOptimize(addr);
    }
}
BENCHMARK(ip_webpp_v4);

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
    if (it == data.end())
        it = data.begin();
    return *it;
}

static void ip_asio_v4_random(benchmark::State& state) {
    ipv4_data();
    for (auto _ : state) {
        auto addr = ip::make_address_v4(ipv4_data());
        benchmark::DoNotOptimize(addr);
    }
}
BENCHMARK(ip_asio_v4_random);

static void ip_webpp_v4_random(benchmark::State& state) {
    ipv4_data();
    for (auto _ : state) {
        auto addr = webpp::ipv4(ipv4_data());
        benchmark::DoNotOptimize(addr);
    }
}
BENCHMARK(ip_webpp_v4_random);

///////////////////// IPv6 ///////////////////////////

static void ip_asio_v6(benchmark::State& state) {
    for (auto _ : state) {
        auto addr = ip::make_address_v6("::1");
        benchmark::DoNotOptimize(addr);
    }
}
BENCHMARK(ip_asio_v6);

static void ip_webpp_v6(benchmark::State& state) {
    for (auto _ : state) {
        auto addr = webpp::ipv6("::1");
        benchmark::DoNotOptimize(addr);
    }
}
BENCHMARK(ip_webpp_v6);

/////////////////// IPv6 Random ///////////////////////

auto ipv6_rands() {
    vector<string> ipv6_rand;
    for (int i = 0; i < 10; i++) {
        string str = "2a00:1450::200";
        str.append(to_string(i));
        ipv6_rand.push_back(std::move(str));
    }
    return ipv6_rand;
}
auto ipv6_data() {
    static auto data = ipv6_rands();
    static auto it   = data.begin();
    ++it;
    if (it == data.end())
        it = data.begin();
    return *it;
}

static void ip_asio_v6_random(benchmark::State& state) {
    ipv6_data();
    for (auto _ : state) {
        auto addr = ip::make_address_v6(ipv6_data());
        benchmark::DoNotOptimize(addr);
    }
}
BENCHMARK(ip_asio_v6_random);

static void ip_webpp_v6_random(benchmark::State& state) {
    ipv6_data();
    for (auto _ : state) {
        auto addr = webpp::ipv6(ipv6_data());
        benchmark::DoNotOptimize(addr);
    }
}
BENCHMARK(ip_webpp_v6_random);

// BENCHMARK_MAIN();
