// Created by moisrex on 1/20/25.

#ifndef WEBPP_URI_PARSE_URI_HPP
#define WEBPP_URI_PARSE_URI_HPP

#include "../../memory/allocator_concepts.hpp"
#include "./parse_authority.hpp"
#include "./parse_host.hpp"
#include "./parse_scheme.hpp"
#include "uri_components.hpp"

namespace webpp::uri {

    namespace details {
        template <uri_options Options, URIContext CtxT>
        static constexpr bool parse_uri_step(CtxT& ctx) noexcept(CtxT::is_nothrow) {
            switch (get_value(ctx.status)) {
                using enum uri_status;
                case valid:                       // we're done parsing
                case valid_punycode: return true; // todo?
                case valid_authority: parse_authority<Options>(ctx); break;
                case valid_file_host:
                    if constexpr (Options.allow_file_hosts) {
                        parse_file_host<Options>(ctx);
                    } else {
                        assert(false);
                        stl::unreachable(); // should be impossible.
                    }
                    break;
                case valid_port: parse_port<Options>(ctx); break;
                case valid_path_start: parse_path_start<Options>(ctx); break;
                case valid_opaque_path: parse_opaque_path(ctx); break;
                case valid_path: parse_path<Options>(ctx); break;
                case valid_queries: parse_queries<Options>(ctx); break;
                case valid_fragment: parse_fragment<Options>(ctx); break;
                case unparsed: parse_scheme<Options>(ctx); break; // start from the beginning
                default:
                    assert(false);
                    stl::unreachable();
                    break; // should be impossible
            }
            return false;
        }

        template <uri_options Options, URIContext CtxT>
        static constexpr void continue_parsing_uri(CtxT& ctx) noexcept(CtxT::is_nothrow) {
            while (!has_error(ctx.status) || is_valid(ctx.status)) {
                if (parse_uri_step<Options>(ctx)) {
                    break;
                }
            }
        }
    } // namespace details

    namespace details {
        template <typename CharT>
        [[nodiscard]] static constexpr bool is_tab_or_newline(CharT const code_unit) noexcept {
            return code_unit == static_cast<CharT>('\t') || code_unit == static_cast<CharT>('\n') ||
                   code_unit == static_cast<CharT>('\r');
        }

        template <typename CharT>
        [[nodiscard]] static constexpr bool is_c0_whitespace(CharT const code_unit) noexcept {
            using uchar_type = stl::make_unsigned_t<CharT>;
            return static_cast<uchar_type>(code_unit) <= ' ';
        }

        /// Remove newlines and tabs from input before doing anything else, or at least validate that we don't have any
        /// of them in the input.
        template <uri_options Options, URIContext CtxT>
        [[nodiscard]] static constexpr bool preprocess_whitespaces(CtxT& ctx) noexcept(CtxT::is_nothrow) {
            // https://url.spec.whatwg.org/#concept-basic-url-parser
            if constexpr (!Options.ignore_tabs_or_newlines) {
                return true;
            } else {
                // left trim C0 Control
                while (ctx.beg != ctx.end && is_c0_whitespace(*ctx.beg)) [[unlikely]] {
                    ++ctx.beg;
                    ctx.pos = ctx.beg;
                }
                // right trim C0 Control
                while (ctx.end != ctx.beg && is_c0_whitespace(*(ctx.end - 1))) [[unlikely]] {
                    --ctx.end;
                }

                // check if we have tabs and newlines in between the URL or not
                auto* src = ctx.beg;
                for (; src != ctx.end; ++src) {
                    if (is_tab_or_newline(*src)) [[unlikely]] {
                        break;
                    }
                }

                if (src == ctx.end) [[likely]] {
                    return true;
                }

                // If input contains any ASCII tab or newline, invalid-URL-unit validation error.
                set_warning(ctx.status, uri_status::invalid_character);

                if constexpr (!CtxT::is_modifiable) {
                    set(ctx.status, uri_status::found_tabs_or_newlines);
                    return false;
                } else {
                    // Remove all ASCII tab or newline from input.
                    using string_type  = typename CtxT::component_type::string_type;
                    using char_type    = typename string_type::value_type;
                    auto const  length = static_cast<typename string_type::size_type>(stl::distance(ctx.beg, ctx.end));
                    string_type sanitized{get_allocator(ctx)};
                    istl::resize_and_overwrite(
                      sanitized,
                      length,
                      [&](char_type* buf, stl::size_t const /*max_length*/) noexcept {
                          char_type* const start = buf;
                          // raw copy
                          for (char_type const* pos = ctx.beg; pos != src;) {
                              *buf++ = *pos++; // NOLINT(*-pointer-arithmetic)
                          }

                          // skip copy: skip new lines and tabs
                          // don't need to handle C0 Controls here since they're trimmed already
                          for (char_type const* pos = src; pos != ctx.end; ++pos) {
                              if (!is_tab_or_newline(*pos)) {
                                  *buf++ = *pos; // NOLINT(*-pointer-arithmetic)
                              }
                          }
                          return static_cast<stl::size_t>(buf - start);
                      });

                    ctx.beg = sanitized.data();
                    ctx.pos = ctx.beg;
                    ctx.end = ctx.beg + sanitized.size();
                    continue_parsing_uri<Options>(ctx);
                    return false;
                }
            }
        }
    } // namespace details

    template <uri_options Options = {}, URIContext CtxT>
    static constexpr void parse_uri(CtxT& ctx) noexcept(CtxT::is_nothrow) {
        if (!details::preprocess_whitespaces<Options>(ctx)) {
            return;
        }
        details::continue_parsing_uri<Options>(ctx);
    }

    /// View-only
    template <uri_options Options = {}, URIComponents CompT = uri_components_u32_view<>>
    static constexpr auto parse_uri(string_view_type_of<CompT> const str) noexcept {
        auto context = create<uri_context<CompT>>(str.data(), str.data() + str.size());
        parse_uri<Options>(context);
        return context;
    }

    template <uri_options Options = {}, URIComponents CompT = uri_components_owning<>, URIComponents BaseCompT>
    static constexpr auto parse_uri(string_view_type_of<CompT> const the_url, BaseCompT&& base_comps) noexcept(false) {
        using context_type = uri_context<CompT, stl::remove_cvref_t<BaseCompT>>;
        auto context       = create<context_type>(
          the_url.data(),
          the_url.data() + the_url.size(),
          stl::forward<BaseCompT>(base_comps),
          allocator_from(base_comps));
        parse_uri<Options>(context);
        return context;
    }

    /// Owning String
    template <uri_options Options = {}, URIComponents CompT = uri_components_owning<>, URIComponents BaseCompT>
    static constexpr auto parse_uri(string_type_of<CompT> const& the_url, BaseCompT&& base_comps) noexcept(false) {
        using context_type = uri_context<CompT, stl::remove_cvref_t<BaseCompT>>;
        auto context       = create<context_type>(
          the_url.data(),
          the_url.data() + the_url.size(),
          stl::forward<BaseCompT>(base_comps),
          allocator_from(the_url));
        parse_uri<Options>(context);
        return context;
    }

    template <uri_options Options = {}, URIComponents CompT = uri_components_owning<>, istl::StringLike StrT>
    static constexpr auto parse_uri(StrT const& the_url, string_view_type_of<CompT> const base_uri)
      noexcept(istl::StringView<StrT>) {
        using url_char_type = typename stl::remove_cvref_t<StrT>::value_type;
        using char_type     = typename CompT::char_type;
        static_assert(stl::same_as<url_char_type, char_type>,
                      "Origin's string's char type must be the same as the specified URI's string's char type.");

        auto origin_context = create<uri_context<CompT>>(base_uri.data(), base_uri.data() + base_uri.size());
        parse_uri<Options>(origin_context);

        return parse_uri<Options, CompT>(the_url, stl::move(origin_context.out));
    }

} // namespace webpp::uri

#endif // WEBPP_URI_PARSE_URI_HPP
