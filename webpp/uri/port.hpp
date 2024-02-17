// Created by moisrex on 11/5/20.

#ifndef WEBPP_URI_PORT_HPP
#define WEBPP_URI_PORT_HPP

#include "../convert/casts.hpp"
#include "../std/string.hpp"
#include "./details/special_schemes.hpp"
#include "./details/uri_components.hpp"
#include "host_authority.hpp"

namespace webpp::uri {

    static constexpr stl::uint16_t max_port_number = 65'535U;

    template <uri_parsing_options Options = uri_parsing_options{}, ParsingURIContext CtxT>
    static constexpr void parse_port(CtxT& ctx) noexcept(CtxT::is_nothrow) {
        // https://url.spec.whatwg.org/#port-state

        using ctx_type  = CtxT;
        using seg_type  = typename ctx_type::out_seg_type;
        using port_type = stl::uint32_t; // we use a bigger size to detect overflows from 65535-99999

        if (ctx.pos == ctx.end) {
            // It's still valid:
            //   scheme://example.com:
            set_valid(ctx.status, uri_status::valid);
            return;
        }

        if constexpr (!Options.parse_port) {
            set_warning(ctx.status, uri_status::invalid_character);
        } else {
            auto const beg        = ctx.pos;
            port_type  port_value = 0;
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
                        // "65535".count() == 5
                        port_value *= 10U; // NOLINT(*-magic-numbers)
                        port_value += static_cast<port_type>(*ctx.pos - '0');
                        ++ctx.pos;
                        if (port_value > max_port_number) {
                            set_error(ctx.status, uri_status::port_out_of_range);
                            return;
                        }
                        continue;
                    case '\\':
                        if (is_special_scheme(ctx.scheme)) {
                            break; // invalid port
                        }
                        [[fallthrough]];
                    case '\0':
                    case '/':
                    case '?':
                    case '#': break;
                    default: set_error(ctx.status, uri_status::port_invalid); return;
                }
                break;
            }

            // it's unsigned, we don't need to check for it being lower than 0
            if (port_value == known_port(get_output_view<components::scheme>(ctx))) {
                clear<components::port>(ctx);
            } else if constexpr (requires { ctx.out.set_port(static_cast<stl::uint16_t>(port_value)); }) {
                // store the integer port value
                ctx.out.set_port(static_cast<stl::uint16_t>(port_value));
            } else if constexpr (requires {
                                     ctx.out.set_port(static_cast<seg_type>(beg - ctx.beg),
                                                      static_cast<seg_type>(ctx.pos - ctx.beg));
                                 })
            {
                // store the position of it relative to the beginning of the URI
                set_value<components::port>(ctx,
                                            static_cast<seg_type>(beg - ctx.beg),
                                            static_cast<seg_type>(ctx.pos - ctx.beg));
            } else {
                // store it as a string
                set_value<components::port>(ctx, beg, ctx.pos);
            }

            // https://url.spec.whatwg.org/#path-start-state
            set_valid(ctx.status, uri_status::valid_authority_end);
        }
    }

    /**
     * Basic port is designed as a string because it also should be able to handle services,
     * not that URIs can handle services but that operating systems APIs like Unix systems can
     * handle services instead of only port numbers.
     *
     * todo: implement handling of services at construction and to convert port number to a service
     */
    template <istl::StringLike StringType = stl::string_view>
    struct basic_port {
        using string_type = StringType;
        using char_type   = istl::char_type_of_t<string_type>;
        using iterator    = typename string_type::iterator;
        using size_type   = typename string_type::size_type;

        static constexpr bool is_modifiable   = istl::ModifiableString<string_type>;
        static constexpr bool is_nothrow      = !is_modifiable;
        static constexpr bool needs_allocator = requires { typename string_type::allocator_type; };


        static constexpr uint16_t max_port_number       = 65'535;
        static constexpr uint16_t well_known_upper_port = 1024;

      private:
        // we're not making this public, because we want this value to be always correct, unless the user
        // explicitly puts invalid values with assign
        string_type storage;

      public:
        template <uri_parsing_options Options = uri_parsing_options{}, typename Iter = iterator>
        constexpr uri_status_type parse(Iter beg, Iter end) noexcept(is_nothrow) {
            parsing_uri_component_context<components::port, string_type*, stl::remove_cvref_t<Iter>> ctx{};
            ctx.beg = beg;
            ctx.pos = beg;
            ctx.end = end;
            ctx.out = stl::addressof(storage);
            parse_port<Options>(ctx);
            return ctx.status;
        }

        template <Allocator AllocT = allocator_type_from_t<string_type>>
            requires needs_allocator
        explicit constexpr basic_port(AllocT const& alloc = {}) noexcept : storage{alloc} {}

        template <Allocator AllocT = allocator_type_from_t<string_type>>
            requires(!needs_allocator)
        explicit constexpr basic_port([[maybe_unused]] AllocT const& alloc = {}) noexcept {}

        template <istl::StringViewifiable InpStr = stl::basic_string_view<char_type>>
        explicit constexpr basic_port(InpStr&& inp_str) noexcept(is_nothrow) {
            auto const str = istl::string_viewify(stl::forward<InpStr>(inp_str));
            parse(str.begin(), str.end());
        }

        template <istl::StringViewifiable InpStr = stl::basic_string_view<char_type>>
        constexpr basic_port& operator=(InpStr&& inp_str) noexcept(is_nothrow) {
            auto const str = istl::string_viewify(stl::forward<InpStr>(inp_str));
            parse(str.begin(), str.end());
            return *this;
        }

        [[nodiscard]] constexpr size_type size() const noexcept {
            return storage.size();
        }

        template <istl::StringView StrVT = stl::basic_string_view<char_type>>
        [[nodiscard]] constexpr bool is_default_port(StrVT const scheme) const noexcept {
            return known_port(scheme) == value();
        }

        template <istl::StringView StrVT = stl::basic_string_view<char_type>>
        [[nodiscard]] constexpr StrVT view() const noexcept {
            return StrVT{storage.data(), storage.size()};
        }

        template <istl::StringLike NStrT = stl::basic_string_view<char_type>>
        constexpr void to_string(NStrT& out, bool const append_separators = false) const
          noexcept(!istl::ModifiableString<NStrT>) {
            // out.reserve(out.size() + storage.size() + 1);
            if constexpr (istl::ModifiableString<NStrT>) {
                if (append_separators) {
                    if (!storage.empty()) {
                        out.push_back(':');
                    }
                }
            }
            istl::append(out, storage);
        }

        template <istl::StringLike NStrT = stl::basic_string_view<char_type>, typename... Args>
        [[nodiscard]] constexpr NStrT as_string(Args&&... args) const
          noexcept(!istl::ModifiableString<NStrT>) {
            NStrT out{stl::forward<Args>(args)...};
            to_string(out);
            return out;
        }

        [[nodiscard]] constexpr bool is_valid() const noexcept {
            auto const val = try_to_int(storage);
            return val && *val >= 0 && *val < max_port_number;
        }

        [[nodiscard]] constexpr bool is_well_known() const noexcept {
            auto const val = try_to_int(storage);
            return val && *val >= 0 && *val < well_known_upper_port;
        }

        [[nodiscard]] constexpr stl::uint16_t value() const noexcept {
            return to_uint16(storage);
        }

        /**
         * @brief Replace the value with the specified raw data, without parsing
         * @param beg start of the value
         * @param end the end of the value
         */
        constexpr void assign(iterator beg, iterator end) noexcept(!is_modifiable) {
            istl::assign(storage, beg, end);
        }

        constexpr void clear() {
            storage.clear();
        }

        /**
         * @brief check if we have value
         * @return false if we don't have anything
         */
        [[nodiscard]] constexpr bool has_value() const noexcept {
            return !storage.empty();
        }
    };


} // namespace webpp::uri

#endif // WEBPP_URI_PORT_HPP
