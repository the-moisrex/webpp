// webpp-side entry points for the ada_url comparison suite.
//
// Everything here is a public (shipped-header) webpp API: the full parser overloads
// plus the individual piece parsers, exactly as webpp's own tests drive them.
// Setters live on `webpp::uri::uri` itself and are called directly by the benches.

#ifndef WEBPP_BENCH_ADA_URL_WEBPP_ADAPTER_HPP
#define WEBPP_BENCH_ADA_URL_WEBPP_ADAPTER_HPP

#include "../../../webpp/ip/inet_ntop.hpp"
#include "../../../webpp/ip/inet_pton.hpp"
#include "../../../webpp/uri/encoding.hpp"
#include "../../../webpp/uri/parser/host_ip.hpp"
#include "../../../webpp/uri/parser/idna_to_ascii.hpp"
#include "../../../webpp/uri/parser/parse_fragment.hpp"
#include "../../../webpp/uri/parser/parse_host.hpp"
#include "../../../webpp/uri/parser/parse_path.hpp"
#include "../../../webpp/uri/parser/parse_port.hpp"
#include "../../../webpp/uri/parser/parse_queries.hpp"
#include "../../../webpp/uri/parser/parse_scheme.hpp"
#include "../../../webpp/uri/uri.hpp"

#include <array>
#include <cstdint>
#include <string>
#include <string_view>

namespace bench::webpp_side {

    namespace uri = webpp::uri;

    inline constexpr uri::uri_options standard_opts = uri::standard_uri_parsing_options;
    inline constexpr uri::uri_options strict_opts   = uri::strict_uri_parsing_options;
    inline constexpr uri::uri_options loose_opts    = uri::loose_uri_parsing_options;

    using owning_components     = uri::uri_components_owning<char>;
    using structured_components = uri::uri_components_structured<char>;

    template <class Comp = owning_components>
    auto make_ctx(std::string_view const input) {
        using ctx_type = uri::uri_context<Comp>;
        return ctx_type{.beg = input.data(), .pos = input.data(), .end = input.data() + input.size()};
    }

    // ---- full parse ----

    template <uri::uri_options Opts>
    auto full_parse_owning(std::string_view const input) {
        return uri::parse_uri<Opts, owning_components>(input);
    }

    template <uri::uri_options Opts>
    auto full_parse_structured(std::string_view const input) {
        auto ctx = make_ctx<structured_components>(input);
        uri::parse_uri<Opts>(ctx);
        return ctx;
    }

    // ---- piece parsers (fresh context per call) ----

    inline auto scheme_piece(std::string_view const input) {
        auto ctx = make_ctx(input);
        uri::parse_scheme<standard_opts>(ctx);
        return ctx;
    }

    inline auto host_piece(std::string_view const input) {
        auto ctx = make_ctx(input);
        uri::set_flag(ctx.status, uri::uri_status::special_scheme);
        uri::host_parser<standard_opts>(ctx);
        return ctx;
    }

    inline auto path_piece(std::string_view const input) {
        auto ctx = make_ctx(input);
        uri::set_flag(ctx.status, uri::uri_status::special_scheme);
        uri::parse_path<standard_opts>(ctx);
        return ctx;
    }

    inline auto queries_piece(std::string_view const input) {
        auto ctx = make_ctx(input);
        uri::set_flag(ctx.status, uri::uri_status::special_scheme);
        uri::parse_queries<standard_opts>(ctx);
        return ctx;
    }

    inline auto fragment_piece(std::string_view const input) {
        auto ctx = make_ctx(input);
        uri::parse_fragment<standard_opts>(ctx);
        return ctx;
    }

    inline auto port_piece(std::string_view const input) {
        auto ctx = make_ctx(input);
        uri::parse_port<standard_opts>(ctx);
        return ctx;
    }

    inline bool host_ipv4_piece(std::string_view const input) {
        auto                        ctx = make_ctx(input);
        std::array<std::uint8_t, 4> octets{};
        return uri::details::parse_host_ipv4<standard_opts>(input.begin(), input.end(), octets.data(), ctx);
    }

    // ---- address parsing / formatting ----

    inline bool pton4(std::string_view const input) {
        auto         pos = input.begin();
        std::uint8_t out[4]{};
        auto const   status = webpp::inet_pton4(pos, input.end(), out);
        return webpp::is_valid(status);
    }

    inline bool pton6(std::string_view const input) {
        auto         pos = input.begin();
        std::uint8_t out[16]{};
        auto const   status = webpp::inet_pton6(pos, input.end(), out);
        return webpp::is_valid(status);
    }

    inline std::size_t ntop4(std::array<std::uint8_t, 4> const& bytes, char (&out)[32]) {
        auto const end = webpp::inet_ntop4(bytes.data(), out);
        return static_cast<std::size_t>(end - out);
    }

    inline std::size_t ntop6(std::array<std::uint8_t, 16> const& bytes, char (&out)[64]) {
        auto const end = webpp::inet_ntop6(bytes.data(), out);
        return static_cast<std::size_t>(end - out);
    }

    // ---- idna ----

    inline void domain_to_ascii(std::string_view const input, std::string& out) {
        out.clear();
        (void) uri::idna::domain_to_ascii<standard_opts>(input.begin(), input.end(), out);
    }

    // ---- percent encoding ----

    inline void percent_encode(std::string_view const input, std::string& out) {
        out.clear();
        uri::encode_uri_component<uri::uri_encoding_policy::encode_chars>(input, out, uri::details::QUERIES_ENCODE_SET);
    }

    /// Full-string percent decoding through the same primitive webpp's host parser
    /// uses (decode_percent_encoded decodes one %XX sequence per call).
    inline bool percent_decode(std::string_view const input, std::string& out) {
        out.clear();
        auto pos = input.begin();
        while (pos != input.end()) {
            if (*pos == '%') {
                if (!uri::details::decode_percent_encoded(pos, input.end(), out)) {
                    return false;
                }
                ++pos; // decode_percent_encoded leaves pos on the last hex digit
            } else {
                out.push_back(*pos);
                ++pos;
            }
        }
        return true;
    }

    // ---- URLs for setter rows ----

    inline uri::uri make_url(std::string_view const base) {
        return uri::uri{base};
    }

} // namespace bench::webpp_side

#endif // WEBPP_BENCH_ADA_URL_WEBPP_ADAPTER_HPP
