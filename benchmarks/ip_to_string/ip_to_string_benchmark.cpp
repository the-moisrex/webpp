#include "../../webpp/common/meta.hpp"
#include "../../webpp/ip/inet_pton.hpp"
#include "../benchmark.hpp"

#include <arpa/inet.h> // ntohl
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
    webpp_static_constexpr auto uint16_byte_count =
      sizeof(stl::uint16_t); // Number of bytes of data in an uint16_t
    webpp_static_constexpr auto ipv4_byte_count = 4;
    webpp_static_constexpr auto ipv6_byte_count = 16;
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

#define MAX_IPv4_STR_LEN 16
#define MAX_IPv6_STR_LEN 64


/*
 * Internet address (a structure for historical reasons)
 */
struct apple_in_addr {
    typedef __uint32_t in_addr_t; /* base type for internet address */
    in_addr_t          s_addr;
};

/*
 * IPv6 address
 */
struct apple_in6_addr {
    union {
        uint8_t  __u6_addr8[16];
        uint16_t __u6_addr16[8];
        uint32_t __u6_addr32[4];
    } __u6_addr; /* 128-bit IP6 address */
};
static const char* hexchars = "0123456789abcdef";

#define s6_addr   __u6_addr.__u6_addr8
#define s6_addr8  __u6_addr.__u6_addr8
#define s6_addr16 __u6_addr.__u6_addr16
#define s6_addr32 __u6_addr.__u6_addr32

/*
 * IPv4 compatible
 */
#define APPLE_IN6_IS_ADDR_V4COMPAT(a)                               \
    ((*(const uint32_t*) (const void*) (&(a)->s6_addr[0]) == 0) &&  \
     (*(const uint32_t*) (const void*) (&(a)->s6_addr[4]) == 0) &&  \
     (*(const uint32_t*) (const void*) (&(a)->s6_addr[8]) == 0) &&  \
     (*(const uint32_t*) (const void*) (&(a)->s6_addr[12]) != 0) && \
     (*(const uint32_t*) (const void*) (&(a)->s6_addr[12]) != ntohl(1)))

/*
 * Mapped
 */
#define APPLE_IN6_IS_ADDR_V4MAPPED(a)                              \
    ((*(const uint32_t*) (const void*) (&(a)->s6_addr[0]) == 0) && \
     (*(const uint32_t*) (const void*) (&(a)->s6_addr[4]) == 0) && \
     (*(const uint32_t*) (const void*) (&(a)->s6_addr[8]) == ntohl(0x0000ffff)))

static const char* apple_inet_ntop4(const struct apple_in_addr* addr, char* buf, size_t len) {
    const u_int8_t* ap = (const u_int8_t*) &addr->s_addr;
    char            tmp[MAX_IPv4_STR_LEN]; /* max length of ipv4 addr string */
    int             fulllen;

    /*
     * snprintf returns number of bytes printed (not including NULL) or
     * number of bytes that would have been printed if more than would
     * fit
     */
    fulllen = snprintf(tmp, sizeof(tmp), "%d.%d.%d.%d", ap[0], ap[1], ap[2], ap[3]);
    if (fulllen >= (int) len) {
        return NULL;
    }

    bcopy(tmp, buf, fulllen + 1);

    return buf;
}

static const char* apple_inet_ntop6(const struct apple_in6_addr* addr, char* dst, size_t size) {
    char                 hexa[8][5], tmp[MAX_IPv6_STR_LEN];
    int                  zr[8];
    size_t               len;
    int32_t              i, j, k, skip;
    uint8_t              x8, hx8;
    uint16_t             x16;
    struct apple_in_addr a4;

    if (addr == NULL)
        return NULL;

    bzero(tmp, sizeof(tmp));

    /*  check for mapped or compat addresses */
    i = APPLE_IN6_IS_ADDR_V4MAPPED(addr);
    j = APPLE_IN6_IS_ADDR_V4COMPAT(addr);
    if ((i != 0) || (j != 0)) {
        char tmp2[16]; /* max length of ipv4 addr string */
        a4.s_addr = addr->__u6_addr.__u6_addr32[3];
        len       = snprintf(tmp,
                       sizeof(tmp),
                       "::%s%s",
                       (i != 0) ? "ffff:" : "",
                       apple_inet_ntop4(&a4, tmp2, sizeof(tmp2)));
        if (len >= size)
            return NULL;
        bcopy(tmp, dst, len + 1);
        return dst;
    }

    k = 0;
    for (i = 0; i < 16; i += 2) {
        j    = 0;
        skip = 1;

        bzero(hexa[k], 5);

        x8 = addr->__u6_addr.__u6_addr8[i];

        hx8 = x8 >> 4;
        if (hx8 != 0) {
            skip         = 0;
            hexa[k][j++] = hexchars[hx8];
        }

        hx8 = x8 & 0x0f;
        if ((skip == 0) || ((skip == 1) && (hx8 != 0))) {
            skip         = 0;
            hexa[k][j++] = hexchars[hx8];
        }

        x8 = addr->__u6_addr.__u6_addr8[i + 1];

        hx8 = x8 >> 4;
        if ((skip == 0) || ((skip == 1) && (hx8 != 0))) {
            hexa[k][j++] = hexchars[hx8];
        }

        hx8          = x8 & 0x0f;
        hexa[k][j++] = hexchars[hx8];

        k++;
    }

    /* find runs of zeros for :: convention */
    j = 0;
    for (i = 7; i >= 0; i--) {
        zr[i] = j;
        x16   = addr->__u6_addr.__u6_addr16[i];
        if (x16 == 0)
            j++;
        else
            j = 0;
        zr[i] = j;
    }

    /* find longest run of zeros */
    k = -1;
    j = 0;
    for (i = 0; i < 8; i++) {
        if (zr[i] > j) {
            k = i;
            j = zr[i];
        }
    }

    for (i = 0; i < 8; i++) {
        if (i != k)
            zr[i] = 0;
    }

    len = 0;
    for (i = 0; i < 8; i++) {
        if (zr[i] != 0) {
            /* check for leading zero */
            if (i == 0)
                tmp[len++] = ':';
            tmp[len++] = ':';
            i += (zr[i] - 1);
            continue;
        }
        for (j = 0; hexa[i][j] != '\0'; j++)
            tmp[len++] = hexa[i][j];
        if (i != 7)
            tmp[len++] = ':';
    }

    /* trailing NULL */
    len++;

    if (len > size)
        return NULL;
    bcopy(tmp, dst, len);
    return dst;
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


namespace v2 {

    /**
     * Determine whether the address is a mapped IPv4 address
     * @return bool
     */
    [[nodiscard]] constexpr bool is_v4_mapped(const stl::uint8_t* octets) noexcept {
        return (octets[0] == 0) && (octets[1] == 0) && (octets[2] == 0) && (octets[3] == 0) &&
               (octets[4] == 0) && (octets[5] == 0) && (octets[6] == 0) && (octets[7] == 0) &&
               (octets[8] == 0) && (octets[9] == 0) && (octets[10] == 0xff) && (octets[11] == 0xff);
    }

    /**
     * Determine whether the address is compatible with ipv4
     * @return bool
     */
    [[nodiscard]] constexpr bool is_ipv6_address_v4_compat(const stl::uint8_t* octets) noexcept {
        return (octets[0] == 0x00) && (octets[1] == 0x00) && (octets[2] == 0x00) && (octets[3] == 0x00) &&
               (octets[4] == 0x00) && (octets[5] == 0x00) && (octets[6] == 0x00) && (octets[7] == 0x00) &&
               (octets[8] == 0x00) && (octets[9] == 0x00) && (octets[10] == 0xff) && (octets[11] == 0xff);
    }

    /**
     * Convert an IPv4 to string
     * It's fast, but it's not pretty, I know :)
     */
    static constexpr char* inet_ntop4(const stl::uint8_t* src, char* out) noexcept {
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
#undef WEBPP_PUT_CHAR
    }

    static constexpr char* inet_ntop6(const stl::uint8_t* src, char* out) noexcept {
        webpp_static_constexpr const char* hex_chars = "0123456789abcdef";

        if (!src) {
            return nullptr;
        }

        *out = '\0';

        // check for mapped or compat addresses
        if (is_v4_mapped(src) || is_ipv6_address_v4_compat(src)) {
            *out++ = ':';
            *out++ = ':';
            *out++ = 'f';
            *out++ = 'f';
            *out++ = 'f';
            *out++ = 'f';
            *out++ = ':';
            return inet_ntop4(src + 12, out);
        }

        char                hexa[8 * 5];
        char*               hex_ptr   = hexa;
        const stl::uint8_t* src_ptr   = src;
        char*               octet_ptr = hex_ptr;
        for (int i = 0; i != 8; ++i) {
            bool skip = true;

            octet_ptr    = hex_ptr;
            *octet_ptr++ = '\0';
            *octet_ptr++ = '\0';
            *octet_ptr++ = '\0';
            *octet_ptr++ = '\0';
            *octet_ptr++ = '\0';
            octet_ptr    = hex_ptr;

            stl::uint8_t x8  = *src_ptr++;
            stl::uint8_t hx8 = x8 >> 4u;

            if (hx8 != 0u) {
                skip         = false;
                *octet_ptr++ = hex_chars[hx8];
            }

            hx8 = x8 & 0x0fu;
            if (!skip || (hx8 != 0u)) {
                skip         = false;
                *octet_ptr++ = hex_chars[hx8];
            }

            x8 = *src_ptr++;

            hx8 = x8 >> 4u;
            if (!skip || (hx8 != 0u)) {
                *octet_ptr++ = hex_chars[hx8];
            }

            hx8          = x8 & 0x0fu;
            *octet_ptr++ = hex_chars[hx8];
            hex_ptr += 5;
        }

        // find runs of zeros for :: convention
        int j             = 0;
        int longest_count = 0;
        int longest_index = -1;
        for (stl::int32_t i = 7; i >= 0; i--) {
            if (src[i + i] == 0 && src[i + i + 1] == 0) {
                j++;
                if (j > longest_count) {
                    longest_index = i;
                    longest_count = j;
                }
            } else {
                j = 0;
            }
        }

        for (int i = 0; i != 8; ++i) {
            if (i == longest_index) {
                // check for leading zero
                if (i == 0) {
                    *out++ = ':';
                }
                *out++ = ':';
                i += longest_count - 1;
            } else {
                for (hex_ptr = hexa + i * 5; *hex_ptr != '\0'; hex_ptr++) {
                    *out++ = *hex_ptr;
                }
                if (i != 7) {
                    *out++ = ':';
                }
            }
        }

        *out = '\0';
        return out;
    }
} // namespace v2


namespace v3 {


    namespace details {
        static constexpr const char* hex_chars = "0123456789abcdef";
    }

    /**
     * Convert IPv6 binary address into presentation (printable) format
     */
    static constexpr char* inet_ntop6(const stl::uint8_t* src, char* out) noexcept {

        if (!src) {
            return nullptr;
        }

        *out = '\0';

        char                hexa[8 * 5];
        char*               hex_ptr   = hexa;
        const stl::uint8_t* src_ptr   = src;
        char*               octet_ptr = hex_ptr;
        for (int i = 0; i != 8; ++i) {
            bool skip = true;

            octet_ptr    = hex_ptr;
            *octet_ptr++ = '\0';
            *octet_ptr++ = '\0';
            *octet_ptr++ = '\0';
            *octet_ptr++ = '\0';
            *octet_ptr++ = '\0';
            octet_ptr    = hex_ptr;

            stl::uint8_t x8  = *src_ptr++;
            stl::uint8_t hx8 = x8 >> 4u;

            if (hx8 != 0u) {
                skip         = false;
                *octet_ptr++ = details::hex_chars[hx8];
            }

            hx8 = x8 & 0x0fu;
            if (!skip || (hx8 != 0u)) {
                skip         = false;
                *octet_ptr++ = details::hex_chars[hx8];
            }

            x8 = *src_ptr++;

            hx8 = x8 >> 4u;
            if (!skip || (hx8 != 0u)) {
                *octet_ptr++ = details::hex_chars[hx8];
            }

            hx8          = x8 & 0x0fu;
            *octet_ptr++ = details::hex_chars[hx8];
            hex_ptr += 5;
        }

        // find runs of zeros for :: convention
        int j             = 0;
        int longest_count = 0;
        int longest_index = -1;
        for (stl::int32_t i = 7; i >= 0; i--) {
            if (src[i + i] == 0 && src[i + i + 1] == 0) {
                j++;
                if (j > longest_count) {
                    longest_index = i;
                    longest_count = j;
                }
            } else {
                j = 0;
            }
        }

        for (int i = 0; i != 8; ++i) {
            if (i == longest_index) {
                // check for leading zero
                if (i == 0) {
                    *out++ = ':';

                    // check for ipv4-mapped or ipv4-compatible addresses (which is deprecated now)
                    if (longest_count == 6) {
                        *out++ = ':';
                        return v2::inet_ntop4(src + 12, out);
                    } else if (longest_count == 5 && src[10] == 0xffu && src[11] == 0xffu) {
                        *out++ = ':';
                        *out++ = 'f';
                        *out++ = 'f';
                        *out++ = 'f';
                        *out++ = 'f';
                        *out++ = ':';
                        return v2::inet_ntop4(src + 12, out);
                    }
                }
                *out++ = ':';
                i += longest_count - 1;
            } else {
                for (hex_ptr = hexa + i * 5; *hex_ptr != '\0'; hex_ptr++) {
                    *out++ = *hex_ptr;
                }
                if (i != 7) {
                    *out++ = ':';
                }
            }
        }

        *out = '\0';
        return out;
    }


} // namespace v3


namespace v4 {
    /**
     * Convert IPv6 binary address into presentation (printable) format
     */
    static constexpr char* inet_ntop6(const stl::uint8_t* src, char* out) noexcept {

        if (!src) {
            return nullptr;
        }

        *out = '\0';

        char                hexa[8 * 5];
        char*               hex_ptr   = hexa;
        const stl::uint8_t* src_ptr   = src;
        char*               octet_ptr = hex_ptr;
        for (int i = 0; i != 8; ++i) {
            bool skip = true;

            octet_ptr    = hex_ptr;
            *octet_ptr++ = '\0';
            *octet_ptr++ = '\0';
            *octet_ptr++ = '\0';
            *octet_ptr++ = '\0';
            *octet_ptr++ = '\0';
            octet_ptr    = hex_ptr;

            stl::uint8_t x8  = *src_ptr++;
            stl::uint8_t hx8 = x8 >> 4u;

            if (hx8 != 0u) {
                skip         = false;
                *octet_ptr++ = v3::details::hex_chars[hx8];
            }

            hx8 = x8 & 0x0fu;
            if (!skip || (hx8 != 0u)) {
                skip         = false;
                *octet_ptr++ = v3::details::hex_chars[hx8];
            }

            x8 = *src_ptr++;

            hx8 = x8 >> 4u;
            if (!skip || (hx8 != 0u)) {
                *octet_ptr++ = v3::details::hex_chars[hx8];
            }

            hx8          = x8 & 0x0fu;
            *octet_ptr++ = v3::details::hex_chars[hx8];
            hex_ptr += 5;
        }

        // find runs of zeros for :: convention
        int j             = 0;
        int longest_count = 0;
        int longest_index = -1;
        for (stl::int32_t i = 7; i >= 0; i--) {
            if (src[i + i] == 0 && src[i + i + 1] == 0) {
                j++;
                if (j > longest_count) {
                    longest_index = i;
                    longest_count = j;
                }
            } else {
                j = 0;
            }
        }

        if (longest_index == 0) {
            *out++ = ':';

            // check for ipv4-mapped or ipv4-compatible addresses (which is deprecated now)
            if (longest_count == 6) {
                *out++ = ':';
                return v2::inet_ntop4(src + 12, out);
            } else if (longest_count == 5 && src[10] == 0xffu && src[11] == 0xffu) {
                *out++ = ':';
                *out++ = 'f';
                *out++ = 'f';
                *out++ = 'f';
                *out++ = 'f';
                *out++ = ':';
                return v2::inet_ntop4(src + 12, out);
            }
        }
        for (int i = 0; i != 8; ++i) {
            if (i == longest_index) {
                // check for leading zero
                *out++ = ':';
                i += longest_count - 1;
            } else {
                for (hex_ptr = hexa + i * 5; *hex_ptr != '\0'; hex_ptr++) {
                    *out++ = *hex_ptr;
                }
                if (i != 7) {
                    *out++ = ':';
                }
            }
        }

        *out = '\0';
        return out;
    }


} // namespace v4


namespace v5 {

    static constexpr char* inet_ntop6(const stl::uint8_t* src, char* out) noexcept {

        if (!src) {
            return nullptr;
        }

        *out = '\0';

        char                hexa[8 * 5];
        char*               hex_ptr   = hexa;
        const stl::uint8_t* src_ptr   = src;
        char*               octet_ptr = hex_ptr;


        int j             = 0;
        int longest_count = 0;
        int longest_index = -1;

        for (int i = 0; i != 8; ++i) {
            bool skip = true;

            octet_ptr    = hex_ptr;
            *octet_ptr++ = '\0';
            *octet_ptr++ = '\0';
            *octet_ptr++ = '\0';
            *octet_ptr++ = '\0';
            *octet_ptr++ = '\0';
            octet_ptr    = hex_ptr;

            stl::uint8_t x8  = *src_ptr++;
            stl::uint8_t hx8 = x8 >> 4u;

            if (hx8 != 0u) {
                skip         = false;
                *octet_ptr++ = v3::details::hex_chars[hx8];
            }

            hx8 = x8 & 0x0fu;
            if (!skip || (hx8 != 0u)) {
                skip         = false;
                *octet_ptr++ = v3::details::hex_chars[hx8];
            }

            x8 = *src_ptr++;

            hx8 = x8 >> 4u;
            if (!skip || (hx8 != 0u)) {
                *octet_ptr++ = v3::details::hex_chars[hx8];
            }

            hx8          = x8 & 0x0fu;
            *octet_ptr++ = v3::details::hex_chars[hx8];
            hex_ptr += 5;



            // find runs of zeros for :: convention
            if (src[i + i] == 0u && src[i + i + 1] == 0u) {
                j++;
                if (j >= longest_count) {
                    longest_index = i - j + 1;
                    longest_count = j;
                }
            } else {
                j = 0;
            }
        }


        if (longest_index == 0) {
            *out++ = ':';

            // check for ipv4-mapped or ipv4-compatible addresses (which is deprecated now)
            if (longest_count == 6) {
                *out++ = ':';
                return v2::inet_ntop4(src + 12, out);
            } else if (longest_count == 5 && src[10] == 0xffu && src[11] == 0xffu) {
                *out++ = ':';
                *out++ = 'f';
                *out++ = 'f';
                *out++ = 'f';
                *out++ = 'f';
                *out++ = ':';
                return v2::inet_ntop4(src + 12, out);
            }
        }

        if (longest_index == -1) {
            for (int i = 0; i != 7; ++i) {
                for (hex_ptr = hexa + i * 5; *hex_ptr != '\0'; hex_ptr++) {
                    *out++ = *hex_ptr;
                }
                *out++ = ':';
            }
            for (hex_ptr = hexa + 7 * 5; *hex_ptr != '\0'; hex_ptr++) {
                *out++ = *hex_ptr;
            }
        } else {
            int i = 0;
            for (; i != longest_index; ++i) {
                for (hex_ptr = hexa + i * 5; *hex_ptr != '\0'; hex_ptr++) {
                    *out++ = *hex_ptr;
                }
                if (i != 7) {
                    *out++ = ':';
                }
            }
            // check for leading zero
            *out++ = ':';
            i += longest_count;
            for (; i != 8; ++i) {
                for (hex_ptr = hexa + i * 5; *hex_ptr != '\0'; hex_ptr++) {
                    *out++ = *hex_ptr;
                }
                if (i != 7) {
                    *out++ = ':';
                }
            }
        }


        *out = '\0';
        return out;
    }
} // namespace v5

////////////////////////////// IPv4 //////////////////////////////

static void IPv4ToStrApple(benchmark::State& state) {
    array<apple_in_addr, sizeof(valid_ipv4s) / sizeof(string_view)> ips;

    auto ip = ips.begin();
    for (auto const& _ip : valid_ipv4s) {
        inet_pton4(_ip.data(), _ip.data() + _ip.size(), (stl::uint8_t*) (&(ip++)->s_addr));
    }
    array<char, sizeof "255.255.255.255"> new_ip{};

    for (auto _ : state) {
        for (auto _ip : ips) {
            apple_inet_ntop4(&_ip, new_ip.data(), new_ip.size());
            benchmark::DoNotOptimize(_ip);
            benchmark::DoNotOptimize(new_ip);
        }
    }
}
BENCHMARK(IPv4ToStrApple);


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

static constexpr array<string_view, 115> valid_ipv6s{
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

static constexpr auto ip_count   = valid_ipv6s.size();
static constexpr auto ipv6_bytes = sizeof "ffff:ffff:ffff:ffff:ffff:ffff:255.255.255.255";


static void IPv6ToStrApple(benchmark::State& state) {
    array<apple_in6_addr, ip_count> ips{};

    auto ip = ips.begin();
    for (auto const& _ip : valid_ipv6s) {
        inet_pton6(_ip.data(), _ip.data() + _ip.size(), (ip++)->s6_addr);
    }
    array<char, ipv6_bytes> new_ip{};

    for (auto _ : state) {
        for (auto _ip : ips) {
            apple_inet_ntop6(&_ip, new_ip.data(), new_ip.size());
            benchmark::DoNotOptimize(_ip);
            benchmark::DoNotOptimize(new_ip);
        }
    }
}
BENCHMARK(IPv6ToStrApple);


static void IPv6ToStrGlibc(benchmark::State& state) {
    array<octets_t, ip_count> ips{};

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


static void IPv6ToStrManualV1(benchmark::State& state) {
    array<octets_t, ip_count> ips{};

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

// Disabled because it causes a segfault
// BENCHMARK(IPv6ToStrManualV1);



static void IPv6ToStrManualV2(benchmark::State& state) {
    array<octets_t, ip_count> ips{};

    auto ip = ips.begin();
    for (auto const& _ip : valid_ipv6s) {
        inet_pton6(_ip.data(), _ip.data() + _ip.size(), (ip++)->data());
    }
    array<char, ipv6_bytes> new_ip{};

    for (auto _ : state) {
        for (auto _ip : ips) {
            v2::inet_ntop6(_ip.data(), new_ip.data());
            benchmark::DoNotOptimize(_ip);
            benchmark::DoNotOptimize(new_ip);
        }
    }
}
BENCHMARK(IPv6ToStrManualV2);

static void IPv6ToStrManualV3(benchmark::State& state) {
    array<octets_t, ip_count> ips{};

    auto ip = ips.begin();
    for (auto const& _ip : valid_ipv6s) {
        inet_pton6(_ip.data(), _ip.data() + _ip.size(), (ip++)->data());
    }
    array<char, ipv6_bytes> new_ip{};

    for (auto _ : state) {
        for (auto _ip : ips) {
            v3::inet_ntop6(_ip.data(), new_ip.data());
            benchmark::DoNotOptimize(_ip);
            benchmark::DoNotOptimize(new_ip);
        }
    }
}
BENCHMARK(IPv6ToStrManualV3);


static void IPv6ToStrManualV4(benchmark::State& state) {
    array<octets_t, ip_count> ips{};

    auto ip = ips.begin();
    for (auto const& _ip : valid_ipv6s) {
        inet_pton6(_ip.data(), _ip.data() + _ip.size(), (ip++)->data());
    }
    array<char, ipv6_bytes> new_ip{};

    for (auto _ : state) {
        for (auto _ip : ips) {
            v4::inet_ntop6(_ip.data(), new_ip.data());
            benchmark::DoNotOptimize(_ip);
            benchmark::DoNotOptimize(new_ip);
        }
    }
}
BENCHMARK(IPv6ToStrManualV4);

static void IPv6ToStrManualV5(benchmark::State& state) {
    array<octets_t, ip_count> ips{};

    auto ip = ips.begin();
    for (auto const& _ip : valid_ipv6s) {
        inet_pton6(_ip.data(), _ip.data() + _ip.size(), (ip++)->data());
    }
    array<char, ipv6_bytes> new_ip{};

    for (auto _ : state) {
        for (auto _ip : ips) {
            v5::inet_ntop6(_ip.data(), new_ip.data());
            benchmark::DoNotOptimize(_ip);
            benchmark::DoNotOptimize(new_ip);
        }
    }
}
BENCHMARK(IPv6ToStrManualV5);
