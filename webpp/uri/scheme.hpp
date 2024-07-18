// Created by moisrex on 10/30/20.

#ifndef WEBPP_URI_SCHEME_HPP
#define WEBPP_URI_SCHEME_HPP

#include "../common/meta.hpp"
#include "../std/string.hpp"
#include "../std/string_like.hpp"
#include "../std/utility.hpp"
#include "../strings/charset.hpp"
#include "../strings/peek.hpp"
#include "./details/special_schemes.hpp"
#include "./details/uri_components.hpp"
#include "./details/uri_helpers.hpp"
#include "uri_status.hpp"

#include <cassert>

namespace webpp::uri {

    namespace details {

        template <uri_parsing_options Options = uri_parsing_options{}, ParsingURIContext CtxT>
        static constexpr void relative_state(CtxT& ctx) noexcept {
            // relative scheme state (https://url.spec.whatwg.org/#relative-state)
            // https://url.spec.whatwg.org/#relative-slash-state

            using ctx_type = CtxT;
            if (ctx.pos == ctx.end) {
                set_valid(ctx.status, uri_status::valid);
                return;
            }

            if constexpr (ctx_type::has_base_uri) {
                // Assert base's scheme is not file
                assert(!is_file_scheme(ctx.base.scheme()));

                set_value<components::scheme>(ctx, ctx.base.get_scheme());
            }
            switch (*ctx.pos) {
                case '/': break;
                case '\\':
                    if (is_special_scheme(ctx.scheme)) {
                        set_warning(ctx.status, uri_status::reverse_solidus_used);
                    }
                    break;
                case '\0':
                    if constexpr (Options.eof_is_valid) {
                        set_valid(ctx.status, uri_status::valid);
                        return;
                    } else {
                        set_warning(ctx.status, uri_status::invalid_character);
                    }
                    break;
                [[unlikely]] case '\r':
                [[unlikely]] case '\n':
                [[unlikely]] case '\t':
                    if constexpr (Options.ignore_tabs_or_newlines) {
                        set_warning(ctx.status, uri_status::invalid_character);
                    }
                    [[fallthrough]];
                default: break;
            }
            ++ctx.pos;
            if (ctx.pos == ctx.end) {
                set_valid(ctx.status, uri_status::valid);
                return;
            }


            // from now on in the algorithms: realtive slash state
            // https://url.spec.whatwg.org/#relative-slash-state
            if constexpr (ctx_type::has_base_uri) {
                set_value<components::username>(ctx, ctx.base.get_username());
                set_value<components::password>(ctx, ctx.base.get_password());
                set_value<components::host>(ctx, ctx.base.get_hostname());
                set_value<components::port>(ctx, ctx.base.get_port());
                set_value<components::path>(
                  ctx,
                  ctx.base.get_path()); // todo: https://infra.spec.whatwg.org/#list-clone
                set_value<components::queries>(ctx, ctx.base.get_queries());
            }
            switch (*ctx.pos) {
                case '?':
                    clear<components::queries>(ctx);
                    set_valid(ctx.status, uri_status::valid_queries);
                    ++ctx.pos;
                    return;
                case '#':
                    clear<components::fragment>(ctx);
                    set_valid(ctx.status, uri_status::valid_fragment);
                    ++ctx.pos;
                    return;
                case '\0':
                    if constexpr (Options.eof_is_valid) {
                        set_valid(ctx.status, uri_status::valid);
                        return;
                    } else {
                        set_warning(ctx.status, uri_status::invalid_character);
                    }
                    break;
                [[unlikely]] case '\r':
                [[unlikely]] case '\n':
                [[unlikely]] case '\t':
                    if constexpr (Options.ignore_tabs_or_newlines) {
                        set_warning(ctx.status, uri_status::invalid_character);
                    }
                    [[fallthrough]];
                default: break;
            }
            clear<components::queries>(ctx);
            // todo: https://url.spec.whatwg.org/#shorten-a-urls-path
            set_valid(ctx.status, uri_status::valid_path);
        }

        template <uri_parsing_options Options = uri_parsing_options{}, ParsingURIContext CtxT>
        static constexpr void file_slash_state(CtxT& ctx) noexcept(CtxT::is_nothrow) {
            // https://url.spec.whatwg.org/#file-slash-state

            using ctx_type = CtxT;
            if (ctx.pos != ctx.end) {
                switch (*ctx.pos) {
                    case '\\': set_warning(ctx.status, uri_status::reverse_solidus_used); [[fallthrough]];
                    case '/':
                        if constexpr (Options.allow_file_hosts) {
                            set_valid(ctx.status, uri_status::valid_file_host);
                        } else {
                            set_valid(ctx.status, uri_status::valid_path);
                        }
                        return;
                    [[unlikely]] case '\r':
                    [[unlikely]] case '\n':
                    [[unlikely]] case '\t':
                        if constexpr (Options.ignore_tabs_or_newlines) {
                            set_warning(ctx.status, uri_status::invalid_character);
                        }
                        [[fallthrough]];
                    default: break;
                }
            }
            if constexpr (ctx_type::has_base_uri) {
                if (is_file_scheme(ctx.base.get_scheme())) {
                    set_value<components::scheme>(ctx, ctx.base.get_scheme());

                    // todo:
                    // 2. If the code point substring from pointer to the end of input does not
                    //    start with a Windows drive letter and base's path[0] is a normalized
                    //    Windows drive letter, then append base's path[0] to url's path.
                    //    This is a (platform-independent) Windows drive letter quirk.
                }
            }
            set_valid(ctx.status, uri_status::valid_path);
        }

        template <uri_parsing_options Options = uri_parsing_options{}, ParsingURIContext CtxT>
        static constexpr void file_state(CtxT& ctx) noexcept(CtxT::is_nothrow) {
            // https://url.spec.whatwg.org/#file-state

            using ctx_type = CtxT;

            if constexpr (ctx_type::has_base_uri) {
                // set scheme to "file"
                set_value<components::scheme>(ctx,
                                              ctx.base.get_scheme().data(),
                                              ctx.base.get_scheme().data() + ctx.base.get_scheme().size());
            }
            clear<components::host>(ctx);

            for (;; ++ctx.pos) {
                if (ctx.pos == ctx.end) {
                    set_valid(ctx.status, uri_status::valid);
                    return;
                }

                switch (*ctx.pos) {
                    case '\\': set_warning(ctx.status, uri_status::reverse_solidus_used); [[fallthrough]];
                    case '/':
                        file_slash_state<Options>(ctx);
                        return;
                    [[unlikely]] case '\0':
                        if constexpr (Options.eof_is_valid) {
                            set_valid(ctx.status, uri_status::valid);
                            return;
                        } else {
                            set_warning(ctx.status, uri_status::invalid_character);
                        }
                        break;
                    [[unlikely]] case '\r':
                    [[unlikely]] case '\n':
                    [[unlikely]] case '\t':
                        if constexpr (Options.ignore_tabs_or_newlines) {
                            set_warning(ctx.status, uri_status::invalid_character);
                            continue;
                        }
                        [[fallthrough]];
                    default: break;
                }
                if constexpr (Options.allow_file_hosts) {
                    set_valid(ctx.status, uri_status::valid_file_host);
                    return;
                }
                break;
            }

            if constexpr (ctx_type::has_base_uri) {
                if (is_file_scheme(ctx.base.get_scheme())) {
                    // todo
                }
            }

            set_valid(ctx.status, uri_status::valid_path);
        }

        template <uri_parsing_options Options = uri_parsing_options{}, ParsingURIContext CtxT>
        static constexpr void no_scheme_state(CtxT& ctx) noexcept(CtxT::is_nothrow) {
            // https://url.spec.whatwg.org/#no-scheme-state

            using ctx_type = CtxT;

            if constexpr (ctx_type::has_base_uri) {
                if (ctx.base.has_path()) { // todo: specs say opaque path
                    for (; ctx.pos != ctx.end; ++ctx.pos) {
                        switch (*ctx.pos) {
                            [[likely]] case '#':
                                set_value<components::scheme>(ctx, ctx.base.get_scheme());
                                set_value<components::path>(ctx, ctx.base.get_path());
                                set_value<components::queries>(ctx, ctx.base.get_queries());
                                clear<components::fragment>(ctx);
                                set_valid(ctx.status, uri_status::valid_fragment);
                                return;

                            [[unlikely]] case '\0':
                                if constexpr (Options.eof_is_valid) {
                                    set_error(ctx.status, uri_status::empty_string);
                                    return;
                                }
                                break;
                            [[unlikely]] case '\r':
                            [[unlikely]] case '\n':
                            [[unlikely]] case '\t':
                                if constexpr (Options.ignore_tabs_or_newlines) {
                                    set_warning(ctx.status, uri_status::invalid_character);
                                    continue;
                                }
                                [[fallthrough]];
                            default: break;
                        }
                        break;
                    }
                } else if (!is_file_scheme(ctx.base.get_scheme())) {
                    relative_state<Options>(ctx);
                    return;
                } else {
                    file_state<Options>(ctx);
                    return;
                }
            }
            set_error(ctx.status, uri_status::missing_scheme_non_relative_url);
        }

        template <uri_parsing_options Options = uri_parsing_options{}, ParsingURIContext CtxT>
        static constexpr void special_authority_ignore_slashes_state(CtxT& ctx) noexcept {
            // special authority ignore slashes state
            // (https://url.spec.whatwg.org/#special-authority-ignore-slashes-state)
            for (; ctx.pos != ctx.end; ++ctx.pos) {
                switch (*ctx.pos) {
                    case '\\':
                    case '/':
                        set_warning(ctx.status, uri_status::missing_following_solidus);
                        continue;
                    [[unlikely]] case '\0':
                        if constexpr (Options.eof_is_valid) {
                            set_error(ctx.status, uri_status::scheme_ended_unexpectedly);
                            return;
                        }
                        break;
                    [[unlikely]] case '\r':
                    [[unlikely]] case '\n':
                    [[unlikely]] case '\t':
                        if constexpr (Options.ignore_tabs_or_newlines) {
                            set_warning(ctx.status, uri_status::invalid_character);
                            continue;
                        }
                        [[fallthrough]];
                    [[likely]] default:
                        break;
                }
                break;
            }
            set_valid(ctx.status, uri_status::valid_authority);
        }

        template <uri_parsing_options Options = uri_parsing_options{}, ParsingURIContext CtxT>
        static constexpr void special_relative_or_authority_state(CtxT& ctx) noexcept {
            // special authority slashes state
            // (https://url.spec.whatwg.org/#special-authority-slashes-state):
            if (safely_inc_if<Options>(ctx, '/', '/')) {
                special_authority_ignore_slashes_state<Options>(ctx);
                return;
            }
            set_warning(ctx.status, uri_status::missing_following_solidus);
            relative_state<Options>(ctx);
        }

        template <typename CharT = char>
        static constexpr stl::array<CharT, 4> file_scheme{'f', 'i', 'l', 'e'};
        template <typename CharT = char>
        static constexpr stl::array<CharT, 4> http_scheme{'h', 't', 't', 'p'};
        template <typename CharT = char>
        static constexpr stl::array<CharT, 5> https_scheme{'h', 't', 't', 'p', 's'};
        template <typename CharT = char>
        static constexpr stl::array<CharT, 3> ftp_scheme{'f', 't', 'p'};
        template <typename CharT = char>
        static constexpr stl::array<CharT, 3> wss_scheme{'w', 's', 's'};
        template <typename CharT = char>
        static constexpr stl::array<CharT, 2> ws_scheme{'w', 's'};

        template <ParsingURIContext CtxT, stl::size_t N>
        constexpr void set_scheme(CtxT& ctx, stl::array<typename CtxT::char_type, N> const scheme) noexcept(
          CtxT::is_nothrow) {
            using ctx_type = CtxT;

            if constexpr (ctx_type::is_modifiable) {
                set_value<components::scheme>(ctx, scheme.begin(), scheme.end());
            } else {
                set_value<components::scheme>(ctx, ctx.beg, ctx.pos);
            }
        }

        template <uri_parsing_options Options, ParsingURIContext CtxT>
        constexpr void set_scheme(CtxT& ctx) noexcept(CtxT::is_nothrow) {
            using ctx_type  = CtxT;
            using char_type = typename ctx_type::char_type;
            using iterator  = typename ctx_type::iterator;

            if constexpr (!ctx_type::is_modifiable) {
                set_value<components::scheme>(ctx, ctx.beg, ctx.pos);
            } else if constexpr (!Options.ignore_tabs_or_newlines) {
                auto& out_str = get_output_from<components::scheme>(get_output_ref(ctx));
                ascii::lower_to(out_str, ctx.beg, ctx.pos);
            } else {
                // this algorithm is the same as "lower_to" except it ignores newlines and tabs

                auto& out_str         = get_output_from<components::scheme>(get_output_ref(ctx));
                using string_type     = stl::remove_cvref_t<decltype(out_str)>;
                using iter_traits     = stl::iterator_traits<typename string_type::iterator>;
                using difference_type = typename iter_traits::difference_type;
                using size_type       = typename string_type::size_type;

                iterator   beg   = ctx.beg;
                auto const end   = ctx.pos;
                auto const count = static_cast<size_type>(end - beg);

#if __cpp_lib_string_resize_and_overwrite
                out_str.resize_and_overwrite(
                  count,
                  [beg](auto* out, [[maybe_unused]] stl::size_t length) mutable constexpr noexcept {
                      auto const endp = beg + static_cast<difference_type>(length);
                      for (; beg != endp; ++beg) {
                          switch (*beg) {
                              case '\r':
                              case '\t':
                              case '\n':
                                  --length;
                                  continue;
                              [[likely]] default:
                                  *out++ = ascii::to_lower_copy<char_type>(*beg);
                                  break;
                          }
                      }
                      return length;
                  });
#else
                out_str.resize(count);
                auto        out        = out_str.begin();
                auto const  endp       = beg + static_cast<difference_type>(count);
                stl::size_t new_length = count;
                for (; beg != endp; ++beg) {
                    switch (*beg) {
                        case '\r':
                        case '\t':
                        case '\n':
                            --new_length;
                            continue;
                        [[likely]] default:
                            *out++ = ascii::to_lower_copy<char_type>(*beg);
                            break;
                    }
                }
                out_str.resize(new_length);
#endif
            }
        }

    } // namespace details

    /**
     * Parse scheme (or sometimes called Protocol)
     */
    template <uri_parsing_options Options = uri_parsing_options{}, ParsingURIContext CtxT>
    static constexpr void parse_scheme(CtxT& ctx) noexcept(CtxT::is_nothrow) {
        using details::encoded_scheme;
        using details::safely_inc_if;

        using ctx_type  = CtxT;
        using char_type = typename ctx_type::char_type;
        using enum uri_status;

        webpp_static_constexpr auto alnum_plus =
          details::ascii_bitmap(details::ascii_bitmap{ALPHA_DIGIT<char_type>}, '+', '-', '.');


        // scheme start (https://url.spec.whatwg.org/#scheme-start-state)
        if (ctx.pos == ctx.end) [[unlikely]] {
            ctx.status = stl::to_underlying(empty_string);
            return;
        }

        // this is designed to find out which scheme type we're dealing with here
        stl::uint64_t scheme_code = 0ULL;

        // handling of the first character:
        for (;;) {
            if (!details::ASCII_ALPHA.contains(*ctx.pos)) [[unlikely]] {
                if constexpr (Options.ignore_tabs_or_newlines) {
                    if (ascii::inc_until_any(ctx.pos, ctx.end, '\n', '\t', '\r')) {
                        set_warning(ctx.status, invalid_character);
                        continue;
                    }
                }

                // if state override is not given, set buffer to the empty string, state to no
                // scheme state, and start over (from the first code point in input).
                //
                // no scheme state (https://url.spec.whatwg.org/#no-scheme-state)
                ctx.pos = ctx.beg;
                clear<components::scheme>(ctx);
                details::no_scheme_state<Options>(ctx);
                return;
            }
            break;
        }

        scheme_code  |= static_cast<stl::uint64_t>(ascii::to_lower_copy(*ctx.pos));
        scheme_code <<= details::one_byte;
        ++ctx.pos;

        // scheme state (https://url.spec.whatwg.org/#scheme-state)
        // handling alpha, num, +, -, .
        for (;; ++ctx.pos) {
            if (ctx.pos == ctx.end) {
                set_error(ctx.status, scheme_ended_unexpectedly);
                return;
            }
            switch (*ctx.pos) {
                case ':':
                    break;
                [[unlikely]] case '\0':
                    if constexpr (Options.eof_is_valid) {
                        set_error(ctx.status, scheme_ended_unexpectedly);
                    } else {
                        set_error(ctx.status, invalid_scheme_character);
                    }
                    return;
                [[unlikely]] case '\r':
                [[unlikely]] case '\n':
                [[unlikely]] case '\t':
                    if constexpr (Options.ignore_tabs_or_newlines) {
                        set_warning(ctx.status, invalid_character);
                        continue;
                    }
                    [[fallthrough]];
                [[likely]] default: {
                    if (!alnum_plus.contains(*ctx.pos)) [[unlikely]] {
                        set_error(ctx.status, invalid_scheme_character);
                        return;
                    }
                    scheme_code  |= static_cast<stl::uint64_t>(ascii::to_lower_copy(*ctx.pos));
                    scheme_code <<= details::one_byte;
                    continue;
                }
            }
            if (ctx.pos == ctx.end) [[unlikely]] {
                set_error(ctx.status, scheme_ended_unexpectedly);
                return;
            }
            break;
        }



        switch (scheme_code) {
            [[likely]] case encoded_scheme("http"):
                details::set_scheme(ctx, details::http_scheme<char_type>);
                break;
            [[likely]] case encoded_scheme("https"):
                details::set_scheme(ctx, details::https_scheme<char_type>);
                break;
            case encoded_scheme("ws"): details::set_scheme(ctx, details::ws_scheme<char_type>); break;
            case encoded_scheme("wss"): details::set_scheme(ctx, details::wss_scheme<char_type>); break;
            case encoded_scheme("ftp"): details::set_scheme(ctx, details::ftp_scheme<char_type>); break;
            case encoded_scheme("file"): {
                details::set_scheme(ctx, details::file_scheme<char_type>);
                ++ctx.pos;
                ctx.scheme = scheme_type::file;
                // If remaining does not start with "//", special-scheme-missing-following-solidus
                // validation error.
                if (!safely_inc_if<Options>(ctx, '/', '/')) [[unlikely]] {
                    set_warning(ctx.status, missing_following_solidus);
                }
                details::file_state<Options>(ctx);
                return;
            }
            [[unlikely]] default: {
                details::set_scheme<Options>(ctx);
                ++ctx.pos;
                ctx.scheme = scheme_type::not_special;
                if (safely_inc_if<Options>(ctx, '/')) {
                    // https://url.spec.whatwg.org/#path-or-authority-state
                    if (safely_inc_if<Options>(ctx, '/')) [[likely]] {
                        set_valid(ctx.status, valid_authority);
                        return;
                    }
                    set_valid(ctx.status, valid_path);
                    return;
                }

                clear<components::path>(ctx);
                set_valid(ctx.status, valid_opaque_path);
                return;
            }
        }


        ++ctx.pos;
        ctx.scheme = scheme_type::special_scheme;

        if constexpr (ctx_type::has_base_uri) {
            if (get_output_view<components::scheme>(ctx) == ctx.base.get_scheme()) {
                // todo: Assert: base is special (and therefore does not have an opaque path).
                details::special_relative_or_authority_state<Options>(ctx);
                return;
            }
        }

        /// https://url.spec.whatwg.org/#special-authority-slashes-state
        if (!safely_inc_if<Options>(ctx, '/', '/')) [[unlikely]] {
            set_warning(ctx.status, missing_following_solidus);
        }
        details::special_authority_ignore_slashes_state<Options>(ctx);
    }

    /// Serialize scheme
    template <istl::StringLike StorageStrT, istl::StringLike StrT>
    static constexpr void render_scheme(
      StorageStrT const& storage,
      StrT&              out,
      bool const         add_separators = false) noexcept(!istl::ModifiableString<StrT>) {
        // https://url.spec.whatwg.org/#url-serializing

        if (storage.empty()) {
            return;
        }
        istl::append(out, storage);
        if constexpr (istl::ModifiableString<StrT>) {
            if (add_separators) {
                out.push_back(':');
            }
        }
    }

    /**
     * Scheme or Protocol
     * @tparam StringType
     */
    template <istl::StringLike StringType = stl::string_view>
    struct basic_scheme {
        using string_type = StringType;
        using char_type   = istl::char_type_of_t<string_type>;
        using iterator    = typename string_type::iterator;
        using size_type   = typename string_type::size_type;

        static constexpr bool is_modifiable   = istl::ModifiableString<string_type>;
        static constexpr bool is_nothrow      = !is_modifiable;
        static constexpr bool needs_allocator = requires { typename string_type::allocator_type; };


      private:
        string_type storage;

      public:
        template <uri_parsing_options Options = uri_parsing_options{}, typename Iter = iterator>
        constexpr uri_status_type parse(Iter beg, Iter end) noexcept(is_nothrow) {
            parsing_uri_component_context<components::scheme, basic_scheme*, stl::remove_cvref_t<Iter>> ctx{};
            ctx.beg = beg;
            ctx.pos = beg;
            ctx.end = end;
            ctx.out = this;
            parse_scheme<Options>(ctx);
            return ctx.status;
        }

        template <Allocator AllocT = allocator_type_from_t<string_type>>
            requires needs_allocator
        explicit constexpr basic_scheme(AllocT const& alloc = {}) noexcept : storage{alloc} {}

        template <Allocator AllocT = allocator_type_from_t<string_type>>
            requires(!needs_allocator)
        explicit constexpr basic_scheme([[maybe_unused]] AllocT const& alloc = {}) noexcept {}

        template <Allocator AllocT = allocator_type_from_t<string_type>>
            requires needs_allocator
        constexpr basic_scheme([[maybe_unused]] stl::allocator_arg_t tag, AllocT const& alloc) noexcept
          : storage{alloc} {}

        template <Allocator AllocT = allocator_type_from_t<string_type>>
            requires(!needs_allocator)
        constexpr basic_scheme([[maybe_unused]] stl::allocator_arg_t tag,
                               [[maybe_unused]] AllocT const&        alloc) noexcept {}

        template <istl::StringLike InpStr = stl::basic_string_view<char_type>>
        explicit constexpr basic_scheme(InpStr const& inp_str) noexcept(is_nothrow) {
            parse(inp_str.begin(), inp_str.end());
        }

        template <Allocator        AllocT = allocator_type_from_t<string_type>,
                  istl::StringLike InpStr = stl::basic_string_view<char_type>>
            requires needs_allocator
        constexpr basic_scheme([[maybe_unused]] stl::allocator_arg_t tag,
                               AllocT const&                         alloc,
                               InpStr const&                         inp_str) noexcept(is_nothrow)
          : storage{alloc} {
            parse(inp_str.begin(), inp_str.end());
        }

        template <Allocator        AllocT = allocator_type_from_t<string_type>,
                  istl::StringLike InpStr = stl::basic_string_view<char_type>>
            requires(!needs_allocator)
        constexpr basic_scheme([[maybe_unused]] stl::allocator_arg_t tag,
                               [[maybe_unused]] AllocT const&        alloc,
                               InpStr const&                         inp_str) noexcept(is_nothrow) {
            parse(inp_str.begin(), inp_str.end());
        }

        template <istl::StringLike InpStr = stl::basic_string_view<char_type>>
        constexpr basic_scheme& operator=(InpStr const& inp_str) noexcept(is_nothrow) {
            parse(inp_str.begin(), inp_str.end());
            return *this;
        }

        [[nodiscard]] constexpr size_type size() const noexcept {
            return storage.size();
        }

        constexpr void clear() noexcept {
            istl::clear(storage);
        }

        /**
         * @brief checks if the URI is a relative reference
         */
        [[nodiscard]] constexpr bool is_relative_reference() const noexcept {
            return storage.empty();
        }

        /**
         * @brief Replace the value with the specified raw data, without parsing
         * @param beg start of the value
         * @param end the end of the value
         */
        template <typename Iter = iterator>
        constexpr void assign(Iter beg, Iter end) noexcept(!is_modifiable) {
            istl::assign(storage, beg, end);
        }

        template <istl::StringView StrVT = stl::basic_string_view<char_type>>
        [[nodiscard]] constexpr StrVT view() const noexcept {
            return StrVT{storage.data(), storage.size()};
        }

        [[nodiscard]] constexpr bool is_special() const noexcept {
            return is_special_scheme(this->view());
        }

        /**
         * @brief check if we have value
         * @return false if we don't have anything
         */
        [[nodiscard]] constexpr bool has_value() const noexcept {
            return !storage.empty();
        }

        template <istl::StringLike NStrT = stl::basic_string_view<char_type>>
        constexpr void to_string(NStrT& out, bool const append_separators = false) const
          noexcept(!istl::ModifiableString<NStrT>) {
            render_scheme(storage, out, append_separators);
        }

        template <istl::StringLike NStrT = stl::basic_string_view<char_type>, typename... Args>
        [[nodiscard]] constexpr NStrT as_string(Args&&... args) const
          noexcept(!istl::ModifiableString<NStrT>) {
            NStrT out{stl::forward<Args>(args)...};
            to_string(out);
            return out;
        }

        [[nodiscard]] constexpr auto& storage_ref() noexcept {
            return storage;
        }

        [[nodiscard]] constexpr auto const& storage_ref() const noexcept {
            return storage;
        }

        template <istl::StringViewifiable NStrT = stl::basic_string_view<char_type>>
        [[nodiscard]] constexpr bool operator==(NStrT&& inp_str) const noexcept {
            if constexpr (is_modifiable) {
                return iiequals_fl<details::TABS_OR_NEWLINES<char_type>>(storage,
                                                                         stl::forward<NStrT>(inp_str));
            } else {
                return iiequals<details::TABS_OR_NEWLINES<char_type>>(storage, stl::forward<NStrT>(inp_str));
            }
        }

        [[nodiscard]] constexpr bool operator==(basic_scheme const& other) const noexcept {
            return storage == other.storage_ref();
        }
    };

} // namespace webpp::uri

#endif // WEBPP_URI_SCHEME_HPP
