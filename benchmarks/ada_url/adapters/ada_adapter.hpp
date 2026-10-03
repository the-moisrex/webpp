// ada-side entry points for the ada_url comparison suite.
//
// Everything here is a public (shipped-header) ada API callable from outside
// libada. Function availability is guarded per ada version where necessary:
//   - ada::checkers::try_parse_ipv4_fast exists since v4/main only (not v2.9.2)
//
// Deliberately NOT wrapped here, even though they are declared in shipped headers:
//   - ada::checkers::is_ipv4, ada::helpers::parse_prepared_path,
//     ada::url_aggregator::parse_path/parse_host are `ada_really_inline`
//     (always_inline) declarations whose bodies live in ada's .cpp units, so GCC
//     rejects calls from any external translation unit ("function body not
//     available"). See README.md.
//
// Only the umbrella header may be used: the shipped ada headers are not all
// self-contained (ada/url.h friend-declares ada::parser;
// ada/implementation.h needs ada/url_pattern_regex.h), so <ada.h> fixes the
// include order for every ref.

#ifndef WEBPP_BENCH_ADA_URL_ADA_ADAPTER_HPP
#define WEBPP_BENCH_ADA_URL_ADA_ADAPTER_HPP

#include <ada.h>
#include <ada/ada_version.h>
#include <array>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>

// Version-gated APIs (e.g. ada::checkers::try_parse_ipv4_fast, ada >= v4) are
// guarded by WEBPP_BENCH_ADA_HAS_IPV4_FAST, a compile definition CMake derives
// from the ref's version string: ada::ADA_VERSION_MAJOR is a C++ enum constant,
// so no #if can read it.

namespace bench::ada_side {

    using sv = std::string_view;

    // ---- full parse ----

    template <class T>
    auto full_parse(sv input) {
        return ada::parse<T>(input);
    }

    // ---- setters (call the same entry points a user would) ----

    template <class T>
    bool set_protocol(T& url, sv input) {
        return url.set_protocol(input);
    }

    template <class T>
    bool set_hostname(T& url, sv input) {
        return url.set_hostname(input);
    }

    template <class T>
    bool set_pathname(T& url, sv input) {
        return url.set_pathname(input);
    }

    template <class T>
    bool set_port(T& url, sv input) {
        return url.set_port(input);
    }

    template <class T>
    bool set_username(T& url, sv input) {
        return url.set_username(input);
    }

    template <class T>
    bool set_password(T& url, sv input) {
        return url.set_password(input);
    }

    template <class T>
    void set_search(T& url, sv input) {
        url.set_search(input);
    }

    template <class T>
    void set_hash(T& url, sv input) {
        url.set_hash(input);
    }

    // ---- idna ----

    inline void to_ascii(sv input, std::string& out) {
        std::optional<std::string> result;
        (void) ada::unicode::to_ascii(result, input, input.find('%'));
        if (result) {
            out = std::move(*result);
        } else {
            out.clear();
        }
    }

    // ---- percent encoding ----

    inline void percent_encode(sv input, std::string& out) {
        // append=false overwrites `out`, but leaves it untouched when no byte needs
        // encoding, so start from a known state each iteration.
        out.clear();
        (void) ada::unicode::percent_encode<false>(input, ada::character_sets::QUERY_PERCENT_ENCODE, out);
    }

    inline std::string percent_decode(sv input) {
        return ada::unicode::percent_decode(input, input.find('%'));
    }

    // ---- ip pieces ----

    inline std::string ipv4_to_string(std::uint32_t packed) {
        return ada::serializers::ipv4(packed);
    }

    inline std::string ipv6_to_string(std::array<std::uint16_t, 8> const& pieces) {
        return ada::serializers::ipv6(pieces);
    }

    // ---- URLs for setter rows ----

    template <class T>
    T make_url(sv base) {
        auto parsed = ada::parse<T>(base);
        return std::move(parsed).value();
    }

} // namespace bench::ada_side

#endif // WEBPP_BENCH_ADA_URL_ADA_ADAPTER_HPP
