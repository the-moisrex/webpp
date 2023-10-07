#include "../../webpp/ip/ipv4.hpp"
#include "../../webpp/ip/ipv6.hpp"
#include "../../webpp/std/std.hpp"
#include "../benchmark.hpp"
#include "./ada_ip_serializers.hpp"
#include "./ada_scheme.hpp"
#include "./webpp_ip_serializers.hpp"
#include "./webpp_scheme.hpp"

#include <vector>


static std::vector<std::string_view> schemes{{"one",        "two",        "three",
                                              "http",       "five",       "https",
                                              "https",      "ftp",        "ftps",
                                              "ftps",       "ws",         "ftps",
                                              "ftps",       "ftp",        "ftps",
                                              "ftps",       "https",      "https",
                                              "https",      "http",       "http",
                                              "http",       "ws",         "http",
                                              "ftps",       "ftps",       "ftps",
                                              "ftps",       "ftps",       "wss",
                                              "ws",         "ws",         "ws",
                                              "file",       "ssh",        "https",
                                              "https",      "http",       "http",
                                              "http",       "scheme",     "nonsense",
                                              "ws",         "ws",         "https",
                                              "https",      "",           "http",
                                              "http",       "     ",      "d;klasjfd;alsjf",
                                              "2408372-54", " @#$@fdsafd"}};

static constexpr std::string_view valid_ipv4s[]{
  "0.0.0.0",         "192.168.1.1",     "255.255.255.255", "192.0.2.1",       "198.51.100.2",
  "203.0.113.3",     "10.0.0.4",        "172.16.0.5",      "192.168.0.6",     "127.0.0.7",
  "169.254.0.8",     "224.0.0.9",       "239.255.255.10",  "128.0.0.11",      "191.255.255.12",
  "223.255.255.13",  "240.0.0.14",      "255.255.255.15",  "1.2.3.4",         "5.6.7.8",
  "9.10.11.12",      "13.14.15.16",     "17.18.19.20",     "21.22.23.24",     "25.26.27.28",
  "29.30.31.32",     "33.34.35.36",     "37.38.39.40",     "41.42.43.44",     "45.46.47.48",
  "49.50.51.52",     "53.54.55.56",     "57.58.59.60",     "61.62.63.64",     "65.66.67.68",
  "69.70.71.72",     "73.74.75.76",     "77.78.79.80",     "81.82.83.84",     "85.86.87.88",
  "89.90.91.92",     "93.94.95.96",     "97.98.99.100",    "101.102.103.104", "105.106.107.108",
  "109.110.111.112", "113.114.115.116", "117.118.119.120", "121.122.123.124", "125.126.127.128",
  "129.130.131.132", "133.134.135.136", "137.138.139.140", "141.142.143.144", "145.146.147.148",
  "149.150.151.152", "153.154.155.156", "157.158.159.160", "161.162.163.164", "165.166.167.168",
  "169.170.171.172", "173.174.175.176", "177.178.179.180", "181.182.183.184", "185.186.187.188",
  "189.190.191.192", "193.194.195.196", "197.198.199.200", "201.202.203.204", "205.206.207.208",
  "209.210.211.212", "213.214.215.216", "217.218.219.220", "221.222.223.224", "225.226.227.228",
  "229.230.231.232", "233.234.235.236", "237.238.239.240", "241.242.243.244", "245.246.247.248",
  "249.250.251.252", "253.254.255.0",   "255.254.253.0",   "1.254.253.0",     "255.1.253.0",
  "255.254.1.0",     "255.254.253.1",   "1.1.253.0",       "255.1.1.0",       "255.254.1.1",
  "1.254.1.0",       "1.1.1.0",         "255.255.1.0",     "255.255.254.0",   "255.255.255.1",
  "255.255.254.1",   "255.254.255.1",   "254.255.255.1",   "255.1.255.0",     "1.255.254.0",
  "1.254.255.0",     "254.1.255.0",     "254.255.1.0"};

static constexpr std::array<std::string_view, 115> valid_ipv6s{
  "2001:0db8:85a3:0000:0000:8a2e:0370:7334",
  "2001:db8:1234::5678",
  "2001:db8::1",
  "::1",
  "::2",
  "2001:0:0:0:0:0:0:1",
  "2001:db8:3333:4444:5555:6666:7777:8888",
  "2001:db8:3333:4444:cccc:dddd:eeee:ffff",
  "2001:db8::",          // implies that the last six segments are zero
  "::1234:5678",         // implies that the first six segments are zero
  "2001:db8::1234:5678", // implies that the middle four segments are zero
  "2001:0db8:0001::0ab9:c0a8:0102",
  "2001:db8:1::ab9:c0a8:102",
  "2001:0db8:85a3::8a2e:0370:7334",
  "2001:db8:1234::5678",
  "2001:db8::1",
  "2001::1",
  "::1234:5678:91.123.4.56", // implies that the first four ipv6 segments are zero
  "2001:db8:3333:4444:5555:6666:1.2.3.4",
  "::11.22.33.44",               // implies all six ipv6 segments are zero
  "2001:db8::123.123.123.123",   // implies that the last four ipv6 segments are zero
  "::1234:5678:1.2.3.4",         // implies that the first four ipv6 segments are zero
  "2001:db8::1234:5678:5.6.7.8", // implies that the middle two ipv6 segments are zero
  "::1",
  "::ffff:192.0.2.128", // IPv4-mapped IPv6 address
  "::FFFF:129.144.52.38",
  "::FAFF:129.144.52.38", // not a IPv4-Compatible IPv6 Address, but looks like one
  "1::129.144.52.38",     // not a IPv4-Compatible IPv6 Address, but looks like one
  "::",

  // AI Generated examples:

  "2001:db8:0:0:0:0:0:1",                   // loopback address
  "2001:db8:ffff:ffff:ffff:ffff:ffff:ffff", // largest address
  "::",                                     // unspecified address
  "::1",                                    // loopback address shorthand
  "2001:db8::",                             // compressed address
  "2001:db8:0:1:0:0:0:0",                   // sequential address
  "2001:db8:0:0:0:ff:0:0",                  // multicast address
  "fe80::1",                                // link-local address
  "ff01::1",                                // multicast address
  "ff02::1",                                // multicast address
  "ff02::2",                                // multicast address
  "ff02::3",                                // multicast address
  "ff02::4",                                // multicast address
  "ff02::5",                                // multicast address
  "ff02::6",                                // multicast address
  "ff02::7",                                // multicast address
  "ff02::8",                                // multicast address
  "ff02::9",                                // multicast address
  "ff02::a",                                // multicast address
  "ff02::b",                                // multicast address
  "ff02::c",                                // multicast address
  "ff02::d",                                // multicast address
  "ff02::e",                                // multicast address
  "ff02::f",                                // multicast address
  "fe80::217:f2ff:fe07:ed62",               // link-local address
  "fe80::217:f2ff:fe07:ed63",               // link-local address
  "fe80::217:f2ff:fe07:ed64",               // link-local address
  "fe80::217:f2ff:fe07:ed65",               // link-local address
  "fe80::217:f2ff:fe07:ed66",               // link-local address
  "fe80::217:f2ff:fe07:ed67",               // link-local address
  "fe80::217:f2ff:fe07:ed68",               // link-local address
  "fe80::217:f2ff:fe07:ed69",               // link-local address
  "fe80::217:f2ff:fe07:ed6a",               // link-local address
  "fe80::217:f2ff:fe07:ed6b",               // link-local address
  "fe80::217:f2ff:fe07:ed6c",               // link-local address
  "fe80::217:f2ff:fe07:ed6d",               // link-local address
  "fe80::217:f2ff:fe07:ed6e",               // link-local address
  "fe80::217:f2ff:fe07:ed6f",               // link-local address
  "fe80::217:f2ff:fe07:ed70",               // link-local address
  "fe80::217:f2ff:fe07:ed71",               // link-local address
  "fe80::217:f2ff:fe07:ed72",               // link-local address
  "fe80::217:f2ff:fe07:ed73",               // link-local address
  "fe80::217:f2ff:fe07:ed74",               // link-local address
  "fe80::217:f2ff:fe07:ed75",               // link-local address
  "fe80::217:f2ff:fe07:ed76",               // link-local address
  "fe80::217:f2ff:fe07:ed77",               // link-local address
  "fe80::217:f2ff:fe07:ed78",               // link-local address
  "fe80::217:f2ff:fe07:ed79",               // link-local address
  "fe80::217:f2ff:fe07:ed7a",               // link-local address
  "fe80::217:f2ff:fe07:ed7b",               // link-local address
  "fe80::217:f2ff:fe07:ed7c",               // link-local address
  "fe80::217:f2ff:fe07:ed7d",               // link-local address
  "fe80::217:f2ff:fe07:ed7e",               // link-local address
  "fe80::217:f2ff:fe07:ed7f",               // link-local address
  "fe80::217:f2ff:fe07:ed80",               // link-local address
  "fe80::217:f2ff:fe07:ed81",               // link-local address
  "fe80::217:f2ff:fe07:ed82",               // link-local address
  "fe80::217:f2ff:fe07:ed83",               // link-local address
  "fe80::217:f2ff:fe07:ed84",               // link-local address
  "fe80::217:f2ff:fe07:ed85",               // link-local address
  "fe80::217:f2ff:fe07:ed86",               // link-local address
  "fe80::217:f2ff:fe07:ed87",               // link-local address
  "fe80::217:f2ff:fe07:ed88",               // link-local address
  "fe80::217:f2ff:fe07:ed89",               // link-local address
  "fe80::217:f2ff:fe07:ed8a",               // link-local address
  "fe80::217:f2ff:fe07:ed8b",               // link-local address
  "fe80::217:f2ff:fe07:ed8c",               // link-local address
  "fe80::217:f2ff:fe07:ed8d",               // link-local address
  "fe80::217:f2ff:fe07:ed8e",               // link-local address
  "fe80::217:f2ff:fe07:ed8f",               // link-local address
  "fe80::217:f2ff:fe07:ed90",               // link-local address
  "fe80::217:f2ff:fe07:ed91",               // link-local address
  "fe80::217:f2ff:fe07:ed92",               // link-local address
  "fe80::217:f2ff:fe07:ed93",               // link-local address
  "fe80::217:f2ff:fe07:ed94",               // link-local address
  "fe80::217:f2ff:fe07:ed95",               // link-local address
  "fe80::217:f2ff:fe07:ed96",               // link-local address
  "fe80::217:f2ff:fe07:ed97",               // link-local address
  "fe80::217:f2ff:fe07:ed98",               // link-local address
  "fe80::217:f2ff:fe07:ed99",               // link-local address
  "fe80::217:f2ff:fe07:ed9a",               // link-local address
  "fe80::217:f2ff:fe07:ed9b",               // link-local address
  "fe80::217:f2ff:fe07:ed9c",               // link-local address
  "fe80::217:f2ff:fe07:ed9d",               // link-local address
  "fe80::217:f2ff:fe07:ed9e",               // link-local address
  "fe80::217:f2ff:fe07:ed9f"                // link-local address
};



static std::vector<std::uint32_t> ipv4s = ([] {
    std::vector<std::uint32_t> out;
    for (auto const ip : valid_ipv4s) {
        out.push_back(webpp::ipv4{ip}.integer());
    }
    return out;
})();

static std::vector<webpp::ipv4_octets> ipv4s_octets = ([] {
    std::vector<webpp::ipv4_octets> out;
    for (auto const ip : valid_ipv4s) {
        out.push_back(webpp::ipv4{ip}.octets());
    }
    return out;
})();

static std::vector<webpp::ipv4> ipv4s_ipv4 = ([] {
    std::vector<webpp::ipv4> out;
    for (auto const ip : valid_ipv4s) {
        out.emplace_back(ip);
    }
    return out;
})();

static std::vector<webpp::ipv6> ipv6s_ipv6 = ([] {
    std::vector<webpp::ipv6> out;
    for (auto const ip : valid_ipv6s) {
        out.emplace_back(ip);
    }
    return out;
})();

static std::vector<webpp::ipv6::octets16_t> ipv6s_16 = ([] {
    std::vector<webpp::ipv6::octets16_t> out;
    for (auto const ip : valid_ipv6s) {
        out.emplace_back(webpp::ipv6{ip}.octets16());
    }
    return out;
})();

static void WebppSchemePort(benchmark::State& state) {
    std::size_t i = 0;
    for (auto _ : state) {
        auto scheme = schemes[i++ % schemes.size()];
        benchmark::DoNotOptimize(scheme);
        auto w_scheme = webpp::v1::basic_scheme<std::string_view>{scheme};
        auto port     = w_scheme.known_port();
        benchmark::DoNotOptimize(port);
    }
}
BENCHMARK(WebppSchemePort);


static void AdaSchemePort(benchmark::State& state) {
    std::size_t i = 0;
    for (auto _ : state) {
        auto scheme = schemes[i++ % schemes.size()];
        benchmark::DoNotOptimize(scheme);
        auto port = ada::scheme::get_special_port(scheme);
        benchmark::DoNotOptimize(port);
    }
}
BENCHMARK(AdaSchemePort);



////////////////////////////// IPv4 Serialization //////////////////////////////

static void WebppV1SerializeIPv4(benchmark::State& state) {
    std::size_t i = 0;
    for (auto _ : state) {
        auto        ip = ipv4s_octets[i++ % ipv4s_octets.size()];
        std::string out;
        webpp::v1::resize_and_append(out, webpp::v1::max_ipv4_str_len, [&ip](auto* buf) constexpr noexcept {
            return webpp::v1::inet_ntop4(ip.data(), buf);
        });
        benchmark::DoNotOptimize(out);
    }
}
BENCHMARK(WebppV1SerializeIPv4);

static void WebppV1OptimizedSerializeIPv4(benchmark::State& state) {
    std::size_t i = 0;
    for (auto _ : state) {
        auto        ip = ipv4s_octets[i++ % ipv4s_octets.size()];
        std::string out{15, '\0'};
        auto        res = webpp::v1::inet_ntop4(ip.data(), out.data());
        out.resize(static_cast<std::size_t>(res - out.data()));
        benchmark::DoNotOptimize(out);
    }
}
BENCHMARK(WebppV1OptimizedSerializeIPv4);

static void WebppV2OptimizedSerializeIPv4(benchmark::State& state) {
    std::size_t i = 0;
    for (auto _ : state) {
        auto        ip = ipv4s_octets[i++ % ipv4s_octets.size()];
        std::string out{15, '\0'};
        auto        res = webpp::v2::inet_ntop4(ip.data(), out.data());
        out.resize(static_cast<std::size_t>(res - out.data()));
        benchmark::DoNotOptimize(out);
    }
}
BENCHMARK(WebppV2OptimizedSerializeIPv4);

static void WebppV3OptimizedSerializeIPv4(benchmark::State& state) {
    std::size_t i = 0;
    for (auto _ : state) {
        auto        ip = ipv4s_octets[i++ % ipv4s_octets.size()];
        std::string out{15, '\0'};
        auto        res = webpp::v3::inet_ntop4(ip.data(), out.data());
        out.resize(static_cast<std::size_t>(res - out.data()));
        benchmark::DoNotOptimize(out);
    }
}
BENCHMARK(WebppV3OptimizedSerializeIPv4);

static void WebppSerializeIPv4(benchmark::State& state) {
    std::size_t i = 0;
    for (auto _ : state) {
        auto        ip = ipv4s_ipv4[i++ % ipv4s_ipv4.size()];
        std::string out;
        ip.to_string(out);
        benchmark::DoNotOptimize(out);
    }
}
BENCHMARK(WebppSerializeIPv4);

static void AdaSerializeIPv4(benchmark::State& state) {
    std::size_t i = 0;
    for (auto _ : state) {
        auto        ip  = ipv4s[i++ % ipv4s.size()];
        std::string out = ada::serializers::ipv4(ip);
        benchmark::DoNotOptimize(out);
    }
}
BENCHMARK(AdaSerializeIPv4);


////////////////////////////// IPv6 Serialization //////////////////////////////


static void AdaSerializeIPv6(benchmark::State& state) {
    std::size_t i = 0;
    for (auto _ : state) {
        auto        ip  = ipv6s_16[i++ % ipv6s_16.size()];
        std::string out = ada::serializers::ipv6(ip);
        benchmark::DoNotOptimize(out);
    }
}
BENCHMARK(AdaSerializeIPv6);

static void WebppSerializeIPv6(benchmark::State& state) {
    std::size_t i = 0;
    for (auto _ : state) {
        auto        ip = ipv6s_ipv6[i++ % ipv6s_ipv6.size()];
        std::string out;
        ip.ip_to_string(out);
        benchmark::DoNotOptimize(out);
    }
}
BENCHMARK(WebppSerializeIPv6);


static void WebppV1SerializeIPv6Optimized(benchmark::State& state) {
    std::size_t i = 0;
    for (auto _ : state) {
        auto        ip = ipv6s_ipv6[i++ % ipv6s_ipv6.size()];
        std::string out{webpp::v1::max_ipv6_str_len, '\0'};
        auto        res = webpp::v1::inet_ntop6(ip.octets().data(), out.data());
        out.resize(static_cast<std::size_t>(res - out.data()));
        benchmark::DoNotOptimize(out);
    }
}
BENCHMARK(WebppV1SerializeIPv6Optimized);
