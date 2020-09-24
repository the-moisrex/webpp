#include "../benchmark.hpp"
#include "../boost_pch.hpp"

// clang-format off
#include webpp_include(utils/ipv4)
#include webpp_include(utils/ipv6)
// clang-format on

using namespace std;
using namespace boost::asio;

static void IP_asio_v4(benchmark::State& state) {
    for (auto _ : state) {
        auto addr = ip::make_address_v4("192.168.1.8");
        benchmark::DoNotOptimize(addr);
    }
}
BENCHMARK(IP_asio_v4);

static void IP_webpp_v4(benchmark::State& state) {
    for (auto _ : state) {
        auto addr = webpp::ipv4("192.168.1.8");
        benchmark::DoNotOptimize(addr);
    }
}
BENCHMARK(IP_webpp_v4);

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

static void IP_asio_v4_random(benchmark::State& state) {
    ipv4_data();
    for (auto _ : state) {
        auto addr = ip::make_address_v4(ipv4_data());
        benchmark::DoNotOptimize(addr);
    }
}
BENCHMARK(IP_asio_v4_random);

static void IP_webpp_v4_random(benchmark::State& state) {
    ipv4_data();
    for (auto _ : state) {
        auto addr = webpp::ipv4(ipv4_data());
        benchmark::DoNotOptimize(addr);
    }
}
BENCHMARK(IP_webpp_v4_random);


