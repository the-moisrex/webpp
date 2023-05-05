#include "../../webpp/ip/ipv4.hpp"
#include "../../webpp/ip/ipv6.hpp"
#include "../../webpp/traits/std_traits.hpp"
#include "../benchmark.hpp"
#include "../boost_pch.hpp"


enum struct inet_pton4_status {
    valid = 0,
    too_little_octets,    // not enough octets
    too_many_octets,      // found too many octets
    invalid_octet_range,  // at least one octet is not in range
    invalid_leading_zero, // the octet is starting with an invalid leading zero
    invalid_character     // found a non-standard character
};

namespace v1 {

    static constexpr inet_pton4_status inet_pton4(const char* src, const char* end, uint8_t* out) noexcept {
        bool saw_digit = false;
        int  octets    = 0;
        *out           = 0;
        while (src != end) {
            char const ch = *src++;
            if (ch >= '0' && ch <= '9') {
                unsigned int const new_i = *out * 10u + static_cast<unsigned int>(ch - '0');
                if (saw_digit && *out == 0) {
                    return inet_pton4_status::invalid_leading_zero;
                }
                if (new_i > 255) {
                    return inet_pton4_status::invalid_octet_range;
                }
                *out = static_cast<uint8_t>(new_i);
                if (!saw_digit) {
                    if (++octets > 4) {
                        return inet_pton4_status::too_many_octets;
                    }
                    saw_digit = true;
                }
            } else if (ch == '.' && saw_digit) {
                if (octets == 4) {
                    return inet_pton4_status::too_many_octets;
                }
                *++out    = 0;
                saw_digit = false;
            } else {
                return inet_pton4_status::invalid_character;
            }
        }
        if (octets < 4) {
            return inet_pton4_status::too_little_octets;
        }
        return inet_pton4_status::valid;
    }
} // namespace v1

namespace v2 {

    static constexpr inet_pton4_status inet_pton4(const char* src, const char* end, uint8_t* out) noexcept {
        bool saw_digit = false;
        int  octets    = 0;
        *out           = 0;
        while (src != end) {
            char const ch = *src++;
            switch (ch) {
                case '0':
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9': {
                    unsigned int const new_i = *out * 10u + static_cast<unsigned int>(ch - '0');
                    if (saw_digit && *out == 0) {
                        return inet_pton4_status::invalid_leading_zero;
                    }
                    if (new_i > 255) {
                        return inet_pton4_status::invalid_octet_range;
                    }
                    *out = static_cast<uint8_t>(new_i);
                    if (!saw_digit) {
                        if (++octets > 4) {
                            return inet_pton4_status::too_many_octets;
                        }
                        saw_digit = true;
                    }
                    break;
                }
                case '.':
                    if (saw_digit) {
                        if (octets == 4) {
                            return inet_pton4_status::too_many_octets;
                        }
                        *++out    = 0;
                        saw_digit = false;
                        break;
                    }
                    [[fallthrough]];
                default: return inet_pton4_status::invalid_character;
            }
        }
        if (octets < 4) {
            return inet_pton4_status::too_little_octets;
        }
        return inet_pton4_status::valid;
    }
} // namespace v2


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


static void IP_webpp_v4_random_inet_pton4_v1(benchmark::State& state) {
    ipv4_data();
    for (auto _ : state) {
        const auto        ip = ipv4_data();
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
        const auto        ip = ipv4_data();
        array<uint8_t, 4> out_ip;
        auto              addr = v2::inet_pton4(ip.data(), ip.data() + ip.size(), out_ip.data());
        benchmark::DoNotOptimize(addr);
        benchmark::DoNotOptimize(out_ip);
    }
}
BENCHMARK(IP_webpp_v4_random_inet_pton4_v2);
