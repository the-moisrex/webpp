#include "../../webpp/ip/inet_pton.hpp"
#include "../benchmark.hpp"

#include <array>
#include <cstring>
using namespace std;
using namespace webpp;


static constexpr const char* inet_ntop4_manual(const uint8_t* src, char* out) noexcept {
#define WEBPP_PUT_CHAR()                                   \
    if (*src < 10) {                                       \
        *out++ = static_cast<char>('0' + *src);            \
    } else if (*src < 100) {                               \
        *out++ = static_cast<char>('0' + *src / 10);       \
        *out++ = static_cast<char>('0' + *src % 10);       \
    } else {                                               \
        *out++ = static_cast<char>('0' + *src / 100);      \
        *out++ = static_cast<char>('0' + *src % 100 / 10); \
        *out++ = static_cast<char>('0' + *src % 10);       \
    }
    WEBPP_PUT_CHAR()
    ++src;
    *out++ = '.';
    WEBPP_PUT_CHAR()
    ++src;
    *out++ = '.';
    WEBPP_PUT_CHAR()
    ++src;
    *out++ = '.';
    WEBPP_PUT_CHAR()
    return out;
}


#ifdef SPRINTF_CHAR
#    define SPRINTF(x) strlen(sprintf /**/ x)
#else
#    define SPRINTF(x) ((size_t) sprintf x)
#endif

static const char* inet_ntop4_glibc(const uint8_t* src, char* out, size_t size) noexcept {
    static const char fmt[] = "%u.%u.%u.%u";
    char              tmp[sizeof "255.255.255.255"];
    if (SPRINTF((tmp, fmt, src[0], src[1], src[2], src[3])) >= size) {
        return nullptr;
    }
    return strcpy(out, tmp);
}


static constexpr string_view valid_ipv4s[]{
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




/* const char *
 * inet_ntop6(src, out, size)
 *	convert IPv6 binary address into presentation (printable) format
 * author:
 *	Paul Vixie, 1996.
 */
static const char* glibc_inet_ntop6(const uint8_t* src, char* out, size_t size) noexcept {
    IF_CXX23(static)
    constexpr auto uint16_byte_count = sizeof(stl::uint16_t); // Number of bytes of data in an uint16_t
    IF_CXX23(static) constexpr auto ipv4_byte_count = 4;
    IF_CXX23(static) constexpr auto ipv6_byte_count = 16;
    /*
     * Note that int32_t and int16_t need only be "at least" large enough
     * to contain a value of the specified size.  On some systems, like
     * Crays, there is no such thing as an integer variable with 16 bits.
     * Keep this in mind if you think this function should have been coded
     * to use pointer overlays.  All the world's not a VAX.
     */
    char tmp[sizeof "ffff:ffff:ffff:ffff:ffff:ffff:255.255.255.255"], *tp;
    struct {
        int base, len;
    } best, cur;
    u_int words[ipv6_byte_count / uint16_byte_count];
    int   i;
    /*
     * Preprocess:
     *	Copy the input (bytewise) array into a wordwise array.
     *	Find the longest run of 0x00's in src[] for :: shorthanding.
     */
    memset(words, '\0', sizeof words);
    for (i = 0; i < ipv6_byte_count; i += 2)
        words[i / 2] = (src[i] << 8) | src[i + 1];
    best.base = -1;
    cur.base  = -1;
    best.len  = 0;
    cur.len   = 0;
    for (i = 0; i < (ipv6_byte_count / uint16_byte_count); i++) {
        if (words[i] == 0) {
            if (cur.base == -1)
                cur.base = i, cur.len = 1;
            else
                cur.len++;
        } else {
            if (cur.base != -1) {
                if (best.base == -1 || cur.len > best.len)
                    best = cur;
                cur.base = -1;
            }
        }
    }
    if (cur.base != -1) {
        if (best.base == -1 || cur.len > best.len)
            best = cur;
    }
    if (best.base != -1 && best.len < 2)
        best.base = -1;
    /*
     * Format the result.
     */
    tp = tmp;
    for (i = 0; i < (ipv6_byte_count / uint16_byte_count); i++) {
        /* Are we inside the best run of 0x00's? */
        if (best.base != -1 && i >= best.base && i < (best.base + best.len)) {
            if (i == best.base)
                *tp++ = ':';
            continue;
        }
        /* Are we following an initial run of 0x00s or any real hex? */
        if (i != 0)
            *tp++ = ':';
        /* Is this address an encapsulated IPv4? */
        if (i == 6 && best.base == 0 && (best.len == 6 || (best.len == 5 && words[5] == 0xffff))) {
            if (!inet_ntop4_glibc(src + 12, tp, sizeof tmp - (tp - tmp)))
                return nullptr;
            tp += strlen(tp);
            break;
        }
        tp += SPRINTF((tp, "%x", words[i]));
    }
    /* Was it a trailing run of 0x00's? */
    if (best.base != -1 && (best.base + best.len) == (ipv6_byte_count / uint16_byte_count))
        *tp++ = ':';
    *tp++ = '\0';
    /*
     * Check for overflow, copy, and we're done.
     */
    if ((size_t) (tp - tmp) > size) {
        return nullptr;
    }
    return strcpy(out, tmp);
}


static constexpr auto IPV6_ADDR_SIZE = 16u; // Bytes
using octets8_t                      = array<uint8_t, 16u>;
using octets16_t                     = array<uint16_t, 8u>;
using octets_t                       = octets8_t;

/**
 * @brief return all the octets in 16bit format
 */
[[nodiscard]] constexpr octets16_t octets16(octets_t const& _octets) noexcept {
    // IP: XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX
    // 08: 00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15
    // 16: --0-- --1-- --2-- --3-- --4-- --5-- --6-- --7--
    // 32: -----0----- -----1----- -----2----- -----3-----
    // 64: -----------0----------- -----------1-----------

    octets16_t       ndata = {};
    constexpr size_t len   = ndata.size();
    using t                = uint16_t;
    for (size_t i = 0; i < len; i++) {
        ndata[i] = static_cast<uint16_t>((static_cast<t>(_octets[i * 2u + 0u]) << (16u - 8u * 1u)) |
                                         (static_cast<t>(_octets[i * 2u + 1u]) << (16u - 8u * 2u)));
    }
    return ndata;
}


/**
 * @brief return the short string representation of ip version 6
 * TODO: all zero ip prints in a wrong format
 */
constexpr void short_str_to(octets_t const& octets, auto& buffer) noexcept {
    using char_type = char;

    auto _octets = octets16(octets);

    // finding all the ranges that are zero filled
    typename decltype(_octets)::const_iterator range_start = _octets.cend(), range_end = _octets.cend(),
                                               start, finish                           = _octets.cbegin();
    do {
        start = find(finish, _octets.cend(), 0u);
        if (start == _octets.cend()) {
            break;
        }
        finish = find_if(start, _octets.cend(), [](auto const& a) {
            return a != 0;
        });
        if (range_start == _octets.cend() || distance(start, finish) > distance(range_start, range_end)) {
            range_start = start;
            range_end   = finish;
        }
    } while (finish != _octets.cend());


    // generating short string representation of the ip version 6

    // I'm not going to use string here because most small object
    // optimizations are smaller than 48bit (16 or 20 mostly)
    // I should be using a string with local-allocator in it, but I've already written this algo
    // char_type   buffer[IPV6_ADDR_SIZE * 2 + (IPV6_ADDR_SIZE - 1) + 1];
    size_t index = 0;


    const auto append_to_buffer = [](auto& buffer, auto& index, uint16_t octet) {
        constexpr auto hex_table = "000102030405060708090a0b0c0d0e0f1011"
                                   "12131415161718191a1b1c1d1e1f20212223"
                                   "2425262728292a2b2c2d2e2f303132333435"
                                   "363738393a3b3c3d3e3f4041424344454647"
                                   "48494a4b4c4d4e4f50515253545556575859"
                                   "5a5b5c5d5e5f606162636465666768696a6b"
                                   "6c6d6e6f707172737475767778797a7b7c7d"
                                   "7e7f808182838485868788898a8b8c8d8e8f"
                                   "909192939495969798999a9b9c9d9e9fa0a1"
                                   "a2a3a4a5a6a7a8a9aaabacadaeafb0b1b2b3"
                                   "b4b5b6b7b8b9babbbcbdbebfc0c1c2c3c4c5"
                                   "c6c7c8c9cacbcccdcecfd0d1d2d3d4d5d6d7"
                                   "d8d9dadbdcdddedfe0e1e2e3e4e5e6e7e8e9"
                                   "eaebecedeeeff0f1f2f3f4f5f6f7f8f9fafb"
                                   "fcfdfeff";
        auto           hex_val   = hex_table + octet;
        if (octet > 0xfu)
            buffer[index++] = *hex_val;
        ++hex_val;
        buffer[index++] = *hex_val;
    };

    auto it = _octets.cbegin();

    // [0, range_start)
    while (it != range_start) {
        append_to_buffer(buffer, index, *it);
        if (++it != range_start)
            buffer[index++] = ':';
    }

    // [range_start, range_end)
    if (it != range_end) {
        buffer[index++] = ':';
        buffer[index++] = ':';
        it              = range_end;
    }

    // [range_end, end)
    while (it != _octets.cend()) {
        append_to_buffer(buffer, index, *it);
        if (++it != _octets.cend())
            buffer[index++] = ':';
    }

    buffer[index++] = '\0';
}



////////////////////////////// IPv4 //////////////////////////////

static void IPv4ToStrGlibc(benchmark::State& state) {
    array<array<uint8_t, 4>, sizeof(valid_ipv4s) / sizeof(string_view)> ips;

    auto ip = ips.begin();
    for (auto const& _ip : valid_ipv4s) {
        inet_pton4(_ip.data(), _ip.data() + _ip.size(), (ip++)->data());
    }
    array<char, sizeof "255.255.255.255"> new_ip{};

    for (auto _ : state) {
        for (auto _ip : ips) {
            inet_ntop4_glibc(_ip.data(), new_ip.data(), new_ip.size());
            benchmark::DoNotOptimize(_ip);
            benchmark::DoNotOptimize(new_ip);
        }
    }
}
BENCHMARK(IPv4ToStrGlibc);


static void IPv4ToStrManual(benchmark::State& state) {
    array<array<uint8_t, 4>, sizeof(valid_ipv4s) / sizeof(string_view)> ips;

    auto ip = ips.begin();
    for (auto const& _ip : valid_ipv4s) {
        inet_pton4(_ip.data(), _ip.data() + _ip.size(), (ip++)->data());
    }
    array<char, sizeof "255.255.255.255"> new_ip{};

    for (auto _ : state) {
        for (auto _ip : ips) {
            inet_ntop4_manual(_ip.data(), new_ip.data());
            benchmark::DoNotOptimize(_ip);
            benchmark::DoNotOptimize(new_ip);
        }
    }
}
BENCHMARK(IPv4ToStrManual);


////////////////////////////// IPv6 //////////////////////////////

static constexpr array<string_view, 6> valid_ipv6s{"2001:0db8:85a3:0000:0000:8a2e:0370:7334",
                                                   "2001:db8:1234::5678",
                                                   "2001:db8::1",
                                                   "::1",
                                                   "::2",
                                                   "2001:0:0:0:0:0:0:1"};

static constexpr auto ipv6_bytes = sizeof "ffff:ffff:ffff:ffff:ffff:ffff:255.255.255.255";


static void IPv6ToStrGlibc(benchmark::State& state) {
    array<octets_t, 6> ips{};

    auto ip = ips.begin();
    for (auto const& _ip : valid_ipv6s) {
        inet_pton6(_ip.data(), _ip.data() + _ip.size(), (ip++)->data());
    }
    array<char, ipv6_bytes> new_ip{};

    for (auto _ : state) {
        for (auto _ip : ips) {
            glibc_inet_ntop6(_ip.data(), new_ip.data(), new_ip.size());
            benchmark::DoNotOptimize(_ip);
            benchmark::DoNotOptimize(new_ip);
        }
    }
}
BENCHMARK(IPv6ToStrGlibc);


static void IPv6ToStrManual(benchmark::State& state) {
    array<octets_t, 6> ips{};

    auto ip = ips.begin();
    for (auto const& _ip : valid_ipv6s) {
        inet_pton6(_ip.data(), _ip.data() + _ip.size(), (ip++)->data());
    }
    array<char, ipv6_bytes> new_ip{};

    for (auto _ : state) {
        for (auto _ip : ips) {
            short_str_to(_ip, new_ip);
            benchmark::DoNotOptimize(_ip);
            benchmark::DoNotOptimize(new_ip);
        }
    }
}
BENCHMARK(IPv6ToStrManual);
