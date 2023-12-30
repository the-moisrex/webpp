#include "../../webpp/ip/ipv4.hpp"
#include "../../webpp/ip/ipv6.hpp"
#include "../../webpp/traits/std_traits.hpp"
#include "../benchmark.hpp"
#include "../boost_pch.hpp"

namespace using_memmove {
    using namespace webpp;

    /**
     * Convert IPv6 Presentation string into network order binary form.
     *
     * This implementation is very similar to the one in glibc, but our version has different side effects.
     *
     * @returns status of the parsing
     **/
    static constexpr inet_pton6_status
    inet_pton6(char const*& src, char const* src_endp, stl::uint8_t* out) noexcept {
        using enum inet_pton6_status;

        stl::uint8_t* colonp = nullptr;
        stl::uint8_t* endp   = out + ipv6_byte_count;

        // Handling Leading ::
        if (src == src_endp) {
            return bad_ending;
        }
        if (*src == ':') {
            ++src;
            if (src == src_endp || *src != ':') {
                return invalid_colon_usage;
            }
        }

        char const*  current_token = src;
        stl::size_t  hex_seen      = 0; // Number of hex digits since colon.
        unsigned int val           = 0;
        char         ch;                // NOLINT(cppcoreguidelines-init-variables)
        while (src != src_endp) {
            ch              = *src++;
            int const digit = ascii::hex_digit_value(ch);
            if (digit >= 0) {
                if (hex_seen == 4) {
                    return invalid_octet_range;
                }
                val <<= 4;
                val  |= static_cast<unsigned int>(digit);
                if (val > 0xffff) {
                    return invalid_octet_range; // todo: is this if stmt even possible?
                }
                ++hex_seen;
                continue;
            } else if (ch == ':') {
                current_token = src;
                if (hex_seen == 0) {
                    if (colonp) {
                        return invalid_colon_usage;
                    }
                    colonp = out;
                    continue;
                } else if (src == src_endp) {
                    return bad_ending;
                }
                if (out + uint16_byte_count > endp) {
                    return invalid_octet_range;
                }
                *out++   = static_cast<stl::uint8_t>((val >> 8) & 0xff);
                *out++   = static_cast<stl::uint8_t>(val & 0xff);
                hex_seen = 0;
                val      = 0;
                continue;
            } else if (ch == '.' && (out + ipv4_byte_count) <= endp) {
                src = current_token;
                switch (inet_pton4(src, src_endp, out)) {
                    case inet_pton4_status::valid: {
                        out      += ipv4_byte_count;
                        hex_seen  = 0;
                        break;
                    }
                    case inet_pton4_status::bad_ending:
                    // case inet_pton4_status::invalid_octet:
                    case inet_pton4_status::too_little_octets:
                    case inet_pton4_status::invalid_leading_zero:
                    case inet_pton4_status::too_many_octets: return bad_ending;
                    case inet_pton4_status::invalid_octet_range: return invalid_octet_range;
                    case inet_pton4_status::invalid_character: return invalid_character;
                    case inet_pton4_status::invalid_prefix: return invalid_prefix;
                }
                break;              // '\0' was seen by inet_pton4.
            } else if (ch == '/') { // handling prefixes
                --src;
                break;
            }
            --src;
            return invalid_character;
        }
        if (hex_seen > 0) {
            if (out + uint16_byte_count > endp) {
                return invalid_octet_range;
            }
            *out++ = static_cast<stl::uint8_t>((val >> 8) & 0xff);
            *out++ = static_cast<stl::uint8_t>(val & 0xff);
        }
        if (colonp != nullptr) {
            // Replace :: with zeros.
            if (out == endp) {
                // :: would expand to a zero-width field.
                return bad_ending;
            }

            // constexpr-friendly equivalent of:
            auto const n = static_cast<stl::size_t>(out - colonp);
            stl::memmove(endp - n, colonp, n);
            stl::memset(colonp, 0, static_cast<stl::size_t>(endp - n - colonp));
            out = endp;
        }
        if (out != endp) {
            return bad_ending;
        }
        if (ch == '/') {
            return invalid_character;
        }
        return valid;
    }

    /**
     * Parse a ipv6 + prefix
     */
    static constexpr inet_pton6_status
    inet_pton6(char const*& src, char const* end, stl::uint8_t* out, stl::uint8_t& prefix) noexcept {
        using enum inet_pton6_status;
        auto const res = inet_pton6(src, end, out);
        if (res == invalid_character && *src == '/') {
            ++src;
            int const prefix_tmp = details::parse_prefix(src, end);
            if (prefix_tmp == -1 || prefix_tmp > 128) {
                return invalid_prefix;
            }
            prefix = static_cast<stl::uint8_t>(prefix_tmp);
            return valid;
        }
        return res;
    }

    static constexpr inet_pton6_status
    inet_pton6(char const* const& inp_src, char const* src_endp, stl::uint8_t* out) noexcept {
        char const* src = inp_src;
        return inet_pton6(src, src_endp, out);
    }

    static constexpr inet_pton6_status inet_pton6(
      char const* const& inp_src,
      char const*        src_endp,
      stl::uint8_t*      out,
      stl::uint8_t&      prefix) noexcept {
        char const* src = inp_src;
        return inet_pton6(src, src_endp, out, prefix);
    }



} // namespace using_memmove

namespace manual_algorithm {
    using namespace webpp;

    /**
     * Convert IPv6 Presentation string into network order binary form.
     *
     * This implementation is very similar to the one in glibc, but our version has different side effects.
     *
     * @returns status of the parsing
     **/
    static constexpr inet_pton6_status
    inet_pton6(char const*& src, char const* src_endp, stl::uint8_t* out) noexcept {
        using enum inet_pton6_status;

        stl::uint8_t* colonp = nullptr;
        stl::uint8_t* endp   = out + ipv6_byte_count;

        // Handling Leading ::
        if (src == src_endp) {
            return bad_ending;
        }
        if (*src == ':') {
            ++src;
            if (src == src_endp || *src != ':') {
                return invalid_colon_usage;
            }
        }

        char const*  current_token = src;
        stl::size_t  hex_seen      = 0; // Number of hex digits since colon.
        unsigned int val           = 0;
        char         ch;                // NOLINT(cppcoreguidelines-init-variables)
        while (src != src_endp) {
            ch              = *src++;
            int const digit = ascii::hex_digit_value(ch);
            if (digit >= 0) {
                if (hex_seen == 4) {
                    return invalid_octet_range;
                }
                val <<= 4;
                val  |= static_cast<unsigned int>(digit);
                if (val > 0xffff) {
                    return invalid_octet_range; // todo: is this if stmt even possible?
                }
                ++hex_seen;
                continue;
            } else if (ch == ':') {
                current_token = src;
                if (hex_seen == 0) {
                    if (colonp) {
                        return invalid_colon_usage;
                    }
                    colonp = out;
                    continue;
                } else if (src == src_endp) {
                    return bad_ending;
                }
                if (out + uint16_byte_count > endp) {
                    return invalid_octet_range;
                }
                *out++   = static_cast<stl::uint8_t>((val >> 8) & 0xff);
                *out++   = static_cast<stl::uint8_t>(val & 0xff);
                hex_seen = 0;
                val      = 0;
                continue;
            } else if (ch == '.' && (out + ipv4_byte_count) <= endp) {
                src = current_token;
                switch (inet_pton4(src, src_endp, out)) {
                    case inet_pton4_status::valid: {
                        out      += ipv4_byte_count;
                        hex_seen  = 0;
                        break;
                    }
                    case inet_pton4_status::bad_ending:
                    // case inet_pton4_status::invalid_octet:
                    case inet_pton4_status::too_little_octets:
                    case inet_pton4_status::invalid_leading_zero:
                    case inet_pton4_status::too_many_octets: return bad_ending;
                    case inet_pton4_status::invalid_octet_range: return invalid_octet_range;
                    case inet_pton4_status::invalid_character: return invalid_character;
                    case inet_pton4_status::invalid_prefix: return invalid_prefix;
                }
                break;              // '\0' was seen by inet_pton4.
            } else if (ch == '/') { // handling prefixes
                --src;
                break;
            }
            --src;
            return invalid_character;
        }
        if (hex_seen > 0) {
            if (out + uint16_byte_count > endp) {
                return invalid_octet_range;
            }
            *out++ = static_cast<stl::uint8_t>((val >> 8) & 0xff);
            *out++ = static_cast<stl::uint8_t>(val & 0xff);
        }
        if (colonp != nullptr) {
            // Replace :: with zeros.
            if (out == endp) {
                // :: would expand to a zero-width field.
                return bad_ending;
            }

            // constexpr-friendly equivalent of:
            // auto const n      = static_cast<stl::size_t>(out - colonp);
            // stl::memmove(endp - n, colonp, n);
            // stl::memset(colonp, 0, static_cast<stl::size_t>(endp - n - colonp));
            auto rightp = endp;
            for (auto leftp = out; leftp != colonp;) {
                *--rightp = *--leftp;
                *leftp    = 0;
            }
            if (out < rightp) {
                *out++ = 0;
                for (; out != rightp; *out++ = 0)
                    ;
            }
            out = endp;
        }
        if (out != endp) {
            return bad_ending;
        }
        if (ch == '/') {
            return invalid_character;
        }
        return valid;
    }

    /**
     * Parse a ipv6 + prefix
     */
    static constexpr inet_pton6_status
    inet_pton6(char const*& src, char const* end, stl::uint8_t* out, stl::uint8_t& prefix) noexcept {
        using enum inet_pton6_status;
        auto const res = inet_pton6(src, end, out);
        if (res == invalid_character && *src == '/') {
            ++src;
            int const prefix_tmp = details::parse_prefix(src, end);
            if (prefix_tmp == -1 || prefix_tmp > 128) {
                return invalid_prefix;
            }
            prefix = static_cast<stl::uint8_t>(prefix_tmp);
            return valid;
        }
        return res;
    }

    static constexpr inet_pton6_status
    inet_pton6(char const* const& inp_src, char const* src_endp, stl::uint8_t* out) noexcept {
        char const* src = inp_src;
        return inet_pton6(src, src_endp, out);
    }

    static constexpr inet_pton6_status inet_pton6(
      char const* const& inp_src,
      char const*        src_endp,
      stl::uint8_t*      out,
      stl::uint8_t&      prefix) noexcept {
        char const* src = inp_src;
        return inet_pton6(src, src_endp, out, prefix);
    }


} // namespace manual_algorithm

using namespace std;
#ifdef webpp_has_boost
using namespace boost::asio;
#endif

///////////////////// IPv6 ///////////////////////////

#ifdef webpp_has_boost
static void IP_asio_v6(benchmark::State& state) {
    for (auto _ : state) {
        auto addr = ip::make_address_v6("::1");
        benchmark::DoNotOptimize(addr);
    }
}

BENCHMARK(IP_asio_v6);
#endif

static void IP_webpp_v6(benchmark::State& state) {
    for (auto _ : state) {
        auto addr = webpp::ipv6("::1");
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
    if (it == data.end()) {
        it = data.begin();
    }
    return *it;
}

#ifdef webpp_has_boost
static void IP_asio_v6_random(benchmark::State& state) {
    ipv6_data();
    for (auto _ : state) {
        auto addr = ip::make_address_v6(ipv6_data());
        benchmark::DoNotOptimize(addr);
    }
}

BENCHMARK(IP_asio_v6_random);
#endif

static void IP_webpp_v6_random(benchmark::State& state) {
    ipv6_data();
    for (auto _ : state) {
        auto addr = webpp::ipv6(ipv6_data());
        benchmark::DoNotOptimize(addr);
    }
}

BENCHMARK(IP_webpp_v6_random);

static void IP_webpp_v6_random_normal(benchmark::State& state) {
    ipv6_data();

    webpp::ipv6::octets_t data;

    for (auto _ : state) {
        auto  ip_str  = ipv6_data();
        auto* inp_ptr = ip_str.data();
        auto* out_ptr = data.data();

        // set the default value to valid
        auto status = webpp::inet_pton6(inp_ptr, inp_ptr + ip_str.size(), out_ptr);
        benchmark::DoNotOptimize(out_ptr);
        benchmark::DoNotOptimize(status);
    }
}

BENCHMARK(IP_webpp_v6_random_normal);

static void IP_webpp_v6_random_manual(benchmark::State& state) {
    ipv6_data();

    webpp::ipv6::octets_t data;

    for (auto _ : state) {
        auto  ip_str  = ipv6_data();
        auto* inp_ptr = ip_str.data();
        auto* out_ptr = data.data();

        // set the default value to valid
        auto status = manual_algorithm::inet_pton6(inp_ptr, inp_ptr + ip_str.size(), out_ptr);
        benchmark::DoNotOptimize(out_ptr);
        benchmark::DoNotOptimize(status);
    }
}

BENCHMARK(IP_webpp_v6_random_manual);

static void IP_webpp_v6_random_memmove(benchmark::State& state) {
    ipv6_data();

    webpp::ipv6::octets_t data;

    for (auto _ : state) {
        auto  ip_str  = ipv6_data();
        auto* inp_ptr = ip_str.data();
        auto* out_ptr = data.data();

        // set the default value to valid
        auto status = using_memmove::inet_pton6(inp_ptr, inp_ptr + ip_str.size(), out_ptr);
        benchmark::DoNotOptimize(out_ptr);
        benchmark::DoNotOptimize(status);
    }
}

BENCHMARK(IP_webpp_v6_random_memmove);
