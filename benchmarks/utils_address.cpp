#include "utils_address_pch.h"
using namespace std;
using namespace boost::asio;

static void AsioV4(benchmark::State& state) {
  for (auto _ : state) {
    auto addr = ip::make_address_v4("192.168.1.8");
    benchmark::DoNotOptimize(addr);
  }
}
BENCHMARK(AsioV4);

static void WebppV4(benchmark::State& state) {
  for (auto _ : state) {
    auto addr = webpp::ipv4("192.168.1.8");
    benchmark::DoNotOptimize(addr);
  }
}
BENCHMARK(WebppV4);


/////////////////// Random ///////////////////////////

auto ipv4_rands() {
  vector<string> ipv4_rand;
  for (int i = 0 ; i < 255; i++) {
    string str = "192.168.1.";
    str.append(to_string(i));
    ipv4_rand.push_back(std::move(str));
  }
  return ipv4_rand;
}
auto ipv4_data() {
  static auto data = ipv4_rands();
  static auto it = data.begin();
  ++it;
  if (it == data.end())
    it = data.begin();
  return *it;
}

static void AsioV4Random(benchmark::State& state) {
  ipv4_data();
  for (auto _ : state) {
    auto addr = ip::make_address_v4(ipv4_data());
    benchmark::DoNotOptimize(addr);
  }
}
BENCHMARK(AsioV4Random);

static void WebppV4Random(benchmark::State& state) {
  ipv4_data();
  for (auto _ : state) {
    auto addr = webpp::ipv4(ipv4_data());
    benchmark::DoNotOptimize(addr);
  }
}
BENCHMARK(WebppV4Random);



///////////////////// IPv6 ///////////////////////////



static void AsioV6(benchmark::State& state) {
  for (auto _ : state) {
    auto addr = ip::make_address_v6("::1");
    benchmark::DoNotOptimize(addr);
  }
}
BENCHMARK(AsioV6);

static void WebppV6(benchmark::State& state) {
  for (auto _ : state) {
    auto addr = webpp::ipv6("::1");
    benchmark::DoNotOptimize(addr);
  }
}
BENCHMARK(WebppV6);


/////////////////// IPv6 Random ///////////////////////

auto ipv6_rands() {
  vector<string> ipv6_rand;
  for (int i = 0 ; i < 10; i++) {
    string str = "2a00:1450::200";
    str.append(to_string(i));
    ipv6_rand.push_back(std::move(str));
  }
  return ipv6_rand;
}
auto ipv6_data() {
  static auto data = ipv6_rands();
  static auto it = data.begin();
  ++it;
  if (it == data.end())
    it = data.begin();
  return *it;
}

static void AsioV6Random(benchmark::State& state) {
  ipv6_data();
  for (auto _ : state) {
    auto addr = ip::make_address_v6(ipv6_data());
    benchmark::DoNotOptimize(addr);
  }
}
BENCHMARK(AsioV6Random);

static void WebppV6Random(benchmark::State& state) {
  ipv6_data();
  for (auto _ : state) {
    auto addr = webpp::ipv6(ipv6_data());
    benchmark::DoNotOptimize(addr);
  }
}
BENCHMARK(WebppV6Random);




// BENCHMARK_MAIN();
