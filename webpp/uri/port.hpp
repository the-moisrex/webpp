// Created by moisrex on 11/5/20.

#ifndef WEBPP_URI_PORT_HPP
#define WEBPP_URI_PORT_HPP

#include "../convert/casts.hpp"
#include "../std/string.hpp"
#include "../strings/append.hpp"
#include "./details/special_schemes.hpp"
#include "./details/uri_components.hpp"

#include <charconv>

namespace webpp::uri {

    static constexpr stl::uint16_t max_port_number = 65535U;

    template <typename... T>
    static constexpr void
    parse_port(uri::parsing_uri_context<T...>& ctx) noexcept(uri::parsing_uri_context<T...>::is_nothrow) {
        // https://url.spec.whatwg.org/#port-state

        using ctx_type  = uri::parsing_uri_context<T...>;
        using seg_type  = typename ctx_type::out_seg_type;

        auto const    beg        = ctx.pos;
        stl::uint16_t port_value = 0;
        while (ctx.pos != ctx.end) {
            switch (*ctx.pos) {
                case '0':
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9':
                    port_value *= 10; // NOLINT(*-magic-numbers)
                    port_value += *ctx.pos - '0';
                    ++ctx.pos;
                    break;
                case '\\':
                    if (is_special_scheme(ctx.out.get_scheme(ctx.whole()))) {
                        break; // invalid port
                    }
                    [[fallthrough]];
                case '\0':
                case '/':
                case '?':
                case '#':
                    if (port_value < 0 || port_value > max_port_number) {
                        ctx.status = stl::to_underlying(uri_status::port_out_of_range);
                        return;
                    }
                    if (port_value == known_port(ctx.out.get_scheme(ctx.whole()))) {
                        ctx.out.clear_port();
                    } else if constexpr (requires { ctx.out.set_port(port_value); }) {
                        // store the integer port value
                        ctx.out.set_port(port_value);
                    } else if constexpr (requires {
                                             ctx.out.set_port(static_cast<seg_type>(beg - ctx.beg),
                                                              static_cast<seg_type>(ctx.pos - ctx.beg));
                                         }) {
                        // store the position of it relative to the beginning of the URI
                        ctx.out.set_port(static_cast<seg_type>(beg - ctx.beg),
                                         static_cast<seg_type>(ctx.pos - ctx.beg));
                    } else {
                        // store it as a string
                        ctx.out.set_port(beg, ctx.pos);
                    }

                    // https://url.spec.whatwg.org/#path-start-state
                    ctx.status |= stl::to_underlying(uri_status::valid_authority_end);
                    return;
            }
        }

        ctx.status = stl::to_underlying(uri_status::port_invalid);
    }


    /**
     * Basic port is designed as a string because it also should be able to handle services,
     * not that URIs can handle services but that operating systems APIs like Unix systems can
     * handle services instead of only port numbers.
     *
     * todo: implement handling of services at construction and to convert port number to a service
     */
    template <istl::String StringType = stl::string>
    struct basic_port : stl::remove_cvref_t<StringType> {
        using string_type = stl::remove_cvref_t<StringType>;

        static constexpr uint16_t max_port_number       = 65535;
        static constexpr uint16_t well_known_upper_port = 1024;

        template <typename... T>
        constexpr basic_port(T&&... args) : string_type{stl::forward<T>(args)...} {
            // todo: make sure if it's a valid port number
            // if (!is::digit(*this)) {
            // convert the service name to port number
            // }
        }

        template <typename T>
            requires(stl::is_integral_v<stl::remove_cvref_t<T>> &&
                     (sizeof(stl::remove_cvref_t<T>) > sizeof(char)) &&
                     !stl::is_floating_point_v<stl::remove_cvref_t<T>>)
        constexpr explicit basic_port(T port_num) : string_type{} {
            if (port_num < 0 || port_num > max_port_number) {
                throw stl::invalid_argument("The specified port number is not in a valid range.");
            }

            webpp::append_to(*this, port_num, stl::chars_format::fixed);
        }

        constexpr basic_port& operator=(stl::integral auto val) {
            this->clear();
            webpp::append_to(*this, val, stl::chars_format::fixed);
            return *this;
        }

        void append_to(istl::String auto& out) {
            if (this->empty()) {
                return; // nothing to add
            }
            // out.reserve(out.size() + this->size() + 1);
            out.push_back(':');
            out.append(*this);
        }

        [[nodiscard]] constexpr bool is_valid() const noexcept {
            auto const val = value();
            return val >= 0 && val <= stl::numeric_limits<uint16_t>::max();
        }

        [[nodiscard]] constexpr bool is_well_known() const noexcept {
            auto const val = value();
            return val >= 0 && val < well_known_upper_port;
        }

        [[nodiscard]] constexpr stl::uint16_t value() const noexcept {
            return to_uint16(*this);
        }
    };


} // namespace webpp::uri

#endif // WEBPP_URI_PORT_HPP
