#include "../benchmark.hpp"
#include "../boost_pch.hpp"

// clang-format off
#include webpp_include(utils/ipv4)
#include webpp_include(utils/ipv6)
#include webpp_include(traits/std_traits)
// clang-format on

using namespace std;
using namespace boost::asio;

///////////////////// IPv6 ///////////////////////////

static void IP_asio_v6(benchmark::State& state) {
    for (auto _ : state) {
        auto addr = ip::make_address_v6("::1");
        benchmark::DoNotOptimize(addr);
    }
}
BENCHMARK(IP_asio_v6);

static void IP_webpp_v6(benchmark::State& state) {
    for (auto _ : state) {
        auto addr = webpp::ipv6<webpp::std_traits>("::1");
        benchmark::DoNotOptimize(addr);
    }
}
BENCHMARK(IP_webpp_v6);

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

static void IP_asio_v6_random(benchmark::State& state) {
    ipv6_data();
    for (auto _ : state) {
        auto addr = ip::make_address_v6(ipv6_data());
        benchmark::DoNotOptimize(addr);
    }
}
BENCHMARK(IP_asio_v6_random);

static void IP_webpp_v6_random(benchmark::State& state) {
    ipv6_data();
    for (auto _ : state) {
        auto addr = webpp::ipv6<webpp::std_traits>(ipv6_data());
        benchmark::DoNotOptimize(addr);
    }
}
BENCHMARK(IP_webpp_v6_random);


