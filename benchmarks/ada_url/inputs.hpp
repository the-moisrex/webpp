// Shared corpora for the ada_url comparison suite.
//
// Every benchmark name is "piece/impl/config/input"; the "input" part comes from the
// `slug` fields below. Corpora are inline-constexpr so benchmark lambdas can capture
// stable addresses of them.

#ifndef WEBPP_BENCH_ADA_URL_INPUTS_HPP
#define WEBPP_BENCH_ADA_URL_INPUTS_HPP

#include <array>
#include <cstdint>
#include <string_view>

namespace bench::inputs {

    using sv = std::string_view;

    struct corpus_entry {
        sv slug;
        sv value;
    };

    struct credentials_entry {
        sv slug;
        sv username;
        sv password;
    };

    // Base URLs that setter benchmarks mutate. They are parsed once per benchmark
    // (outside the timed loop) and mutated repeatedly inside the loop.
    inline constexpr sv base_scheme = "http://example.com:8080/a";
    inline constexpr sv base_host   = "https://example.com/base/path";
    inline constexpr sv base_query  = "https://example.com/page";
    inline constexpr sv base_port   = "https://example.com/a";
    inline constexpr sv base_creds  = "https://example.com/";
    inline constexpr sv base_build  = "https://user:pass@www.example.com:8080/path/to/resource?foo=bar&baz=qux#section";

    // ---- full parse ----
    inline constexpr std::array<corpus_entry, 18> full_parse{
      {
       {.slug = "special_basic", .value = "https://example.com/path?query=1#frag"},
       {.slug = "special_nopath", .value = "https://example.com"},
       {.slug = "special_port", .value = "https://example.com:8443/a/b"},
       {.slug = "user_pass", .value = "https://user:pass@example.org/x"},
       {.slug = "ipv6_host", .value = "https://[2001:db8::1]/x?q=1"},
       {.slug = "ipv4_hex", .value = "http://0300.168.0xF0/"},
       {.slug = "idn_host", .value = "https://bücher.de/straße"},
       {.slug = "punycode_host", .value = "https://xn--bcher-kva.de/x"},
       {.slug = "dots_path", .value = "https://example.com/a/./b/../c/index.html"},
       {.slug = "backslash_path", .value = "https://example.com/a\\b\\c"},
       {.slug = "pct_path", .value = "https://example.com/a%20b/%E2%82%AC"},
       {.slug = "query_frag_heavy", .value = "http://example.org/p?x=1&y=%2F#s1%20s2"},
       {.slug = "file_url", .value = "file:///var/log/system.log"},
       {.slug = "mailto_opaque", .value = "mailto:user@example.com?subject=hi"},
       {.slug = "non_special_port", .value = "ssh://user@host:22/path"},
       {.slug = "invalid_host", .value = "https://bad host/x"},
       {.slug = "trailing_dot", .value = "https://example.com./"},
       {.slug = "long_path", .value = "https://example.com/a/b/c/d/e/f/g/h/i/j/k/l/m/n/o/p.html"},
       }
    };

    // ---- scheme (colon-terminated; special -> special transitions) ----
    inline constexpr std::array<corpus_entry, 4> scheme{
      {
       {.slug = "http", .value = "http:"},
       {.slug = "https", .value = "https:"},
       {.slug = "ftp", .value = "ftp:"},
       {.slug = "wss", .value = "wss:"},
       }
    };

    // ---- host (valid hosts; used by host_parser and the hostname setters) ----
    inline constexpr std::array<corpus_entry, 5> host{
      {
       {.slug = "domain", .value = "example.com"},
       {.slug = "idn", .value = "bücher.de"},
       {.slug = "punycode", .value = "xn--bcher-kva.de"},
       {.slug = "ipv4", .value = "192.168.1.1"},
       {.slug = "ipv6", .value = "[::1]"},
       }
    };

    // ---- domain to ascii ----
    inline constexpr std::array<corpus_entry, 4> domain_to_ascii{
      {
       {.slug = "idn", .value = "bücher.de"},
       {.slug = "idn_long", .value = "münchen.example.com"},
       {.slug = "ascii", .value = "example.com"},
       {.slug = "punycode", .value = "xn--bcher-kva.de"},
       }
    };

    // ---- ipv4 ----
    inline constexpr std::array<corpus_entry, 5> ipv4{
      {
       {.slug = "dotted", .value = "192.168.1.1"},
       {.slug = "hex_octal", .value = "0300.168.0xF0"},
       {.slug = "short", .value = "127.1"},
       {.slug = "max", .value = "255.255.255.255"},
       {.slug = "invalid", .value = "999.999.999.999"},
       }
    };

    // setters only get inputs that both libraries accept
    inline constexpr std::array<corpus_entry, 4> ipv4_setter{
      {
       {.slug = "dotted", .value = "192.168.1.1"},
       {.slug = "hex_octal", .value = "0300.168.0xF0"},
       {.slug = "short", .value = "127.1"},
       {.slug = "max", .value = "255.255.255.255"},
       }
    };

    // ---- ipv6 ----
    inline constexpr std::array<corpus_entry, 4> ipv6{
      {
       {.slug = "plain", .value = "::1"},
       {.slug = "full", .value = "2001:db8:85a3:8d3:1319:8a2e:370:7344"},
       {.slug = "compressed", .value = "2001:db8::1"},
       {.slug = "v4mapped", .value = "::ffff:192.168.1.1"},
       }
    };

    inline constexpr std::array<corpus_entry, 4> ipv6_setter{
      {
       {.slug = "plain", .value = "[::1]"},
       {.slug = "full", .value = "[2001:db8:85a3:8d3:1319:8a2e:370:7344]"},
       {.slug = "compressed", .value = "[2001:db8::1]"},
       {.slug = "v4mapped", .value = "[::ffff:192.168.1.1]"},
       }
    };

    // ---- path: direct parsers get the path without its leading slash (that is what
    //      the parsers see after path-start consumed it); setters get user-shaped
    //      input with the leading slash ----
    inline constexpr std::array<corpus_entry, 6> path_direct{
      {
       {.slug = "simple", .value = "index.html"},
       {.slug = "deep", .value = "a/b/c/d/e/f/g/h.html"},
       {.slug = "dots", .value = "a/./b/../c.html"},
       {.slug = "encoded", .value = "a%20b/c%2Fd.html"},
       {.slug = "backslash", .value = "a\\b\\c.html"},
       {.slug = "trailing", .value = "dir/"},
       },
    };

    inline constexpr std::array<corpus_entry, 6> path_setter{
      {
       {.slug = "simple", .value = "/index.html"},
       {.slug = "deep", .value = "/a/b/c/d/e/f/g/h.html"},
       {.slug = "dots", .value = "/a/./b/../c.html"},
       {.slug = "encoded", .value = "/a%20b/c%2Fd.html"},
       {.slug = "backslash", .value = "/a\\b\\c.html"},
       {.slug = "trailing", .value = "/dir/"},
       },
    };

    // ---- queries (without '?') ----
    inline constexpr std::array<corpus_entry, 4> query{
      {
       {.slug = "simple", .value = "q=1&x=2"},
       {.slug = "encoded", .value = "q=a%20b&x=%2F"},
       {
          .slug  = "long",
          .value = "alpha=one&beta=two&gamma=three&delta=four&epsilon=five&zeta=six&eta=seven&theta=eight",
        }, {.slug = "empty", .value = ""},
       },
    };

    // ---- fragment (without '#') ----
    inline constexpr std::array<corpus_entry, 3> fragment{
      {
       {.slug = "simple", .value = "section-1"},
       {.slug = "encoded", .value = "a%20b"},
       {.slug = "empty", .value = ""},
       },
    };

    // ---- port ----
    inline constexpr std::array<corpus_entry, 4> port_setter{
      {
       {.slug = "custom", .value = "8080"},
       {.slug = "alt", .value = "8443"},
       {.slug = "zero", .value = "0"},
       {.slug = "empty", .value = ""},
       },
    };

    inline constexpr std::array<corpus_entry, 6> port_direct{
      {
       {.slug = "custom", .value = "8080"},
       {.slug = "alt", .value = "8443"},
       {.slug = "zero", .value = "0"},
       {.slug = "empty", .value = ""},
       {.slug = "overflow", .value = "99999"},
       {.slug = "alpha", .value = "abc"},
       },
    };

    // ---- credentials ----
    inline constexpr std::array<credentials_entry, 3> credentials{
      {
       {.slug = "simple", .username = "alice", .password = "pw"},
       {.slug = "space", .username = "us er", .password = "p w"},
       {.slug = "at", .username = "a@b", .password = "x:y"},
       },
    };

    // ---- percent encoding (ASCII corpora; see README for the non-ASCII caveat) ----
    inline constexpr std::array<corpus_entry, 4> percent_encode{
      {
       {.slug = "space_amp", .value = "a b&c=d?x#y"},
       {.slug = "plus_eq", .value = "a+b=c"},
       {.slug = "double_pct", .value = "%20%2F%25"},
       {.slug = "spaces_only", .value = "   "},
       },
    };

    inline constexpr std::array<corpus_entry, 4> percent_decode{
      {
       {.slug = "space", .value = "a%20b"},
       {.slug = "utf8", .value = "caf%C3%A9"},
       {.slug = "euro", .value = "%E2%82%AC"},
       {.slug = "plain", .value = "no-percent"},
       },
    };

    // ---- serialize href ----
    inline constexpr std::array<corpus_entry, 4> serialize_href{
      {
       {.slug = "special_basic", .value = "https://example.com/path?query=1#frag"},
       {.slug = "ipv6_host", .value = "https://[2001:db8::1]/x?q=1"},
       {.slug = "idn_host", .value = "https://bücher.de/straße"},
       {.slug = "user_pass", .value = "https://user:pass@example.org/x"},
       },
    };

    // ---- address serialization ----
    struct ip4_entry {
        sv                          slug;
        std::array<std::uint8_t, 4> bytes;
    };

    struct ip6_entry {
        sv                           slug;
        std::array<std::uint8_t, 16> bytes;
    };

    constexpr std::uint32_t packed_ip4(ip4_entry const& e) noexcept {
        return (static_cast<std::uint32_t>(e.bytes[0]) << 24U) | (static_cast<std::uint32_t>(e.bytes[1]) << 16U) |
               (static_cast<std::uint32_t>(e.bytes[2]) << 8U) | static_cast<std::uint32_t>(e.bytes[3]);
    }

    // ada's ipv6 serializer consumes eight 16-bit pieces in host order
    constexpr std::array<std::uint16_t, 8> ip6_pieces(ip6_entry const& e) noexcept {
        std::array<std::uint16_t, 8> pieces{};
        for (std::size_t i = 0; i < 8; ++i) {
            pieces[i] = static_cast<std::uint16_t>(
              (static_cast<std::uint16_t>(e.bytes[2 * i]) << 8U) | static_cast<std::uint16_t>(e.bytes[2 * i + 1]));
        }
        return pieces;
    }

    inline constexpr std::array<ip4_entry, 5> ip4_addresses{
      {
       {.slug = "private", .bytes = {192, 168, 1, 1}},
       {.slug = "loopback", .bytes = {127, 0, 0, 1}},
       {.slug = "any", .bytes = {0, 0, 0, 0}},
       {.slug = "max", .bytes = {255, 255, 255, 255}},
       {.slug = "dns", .bytes = {8, 8, 8, 8}},
       }
    };

    inline constexpr std::array<ip6_entry, 5> ip6_addresses{
      {
       {.slug = "loopback", .bytes = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}},
       {.slug = "any", .bytes = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
       {.slug = "compressed_mid", .bytes = {0x20, 0x01, 0x0d, 0xb8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 1}},
       {.slug = "v4mapped", .bytes = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xff, 0xff, 192, 168, 1, 1}},
       {.slug  = "full",
         .bytes = {0x20, 0x01, 0x0d, 0xb8, 0x85, 0xa3, 0x8d, 0x3, 0x13, 0x19, 0x8a, 0x2e, 0x37, 0x0, 0x73, 0x44}},
       }
    };

} // namespace bench::inputs

#endif // WEBPP_BENCH_ADA_URL_INPUTS_HPP
