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
                        stl::unreachable(); // should be impossible.
                    }
                    break;
                case valid_port: parse_port<Options>(ctx); break;
                case valid_authority_end: parse_authority_end<Options>(ctx); break;
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
            while (!has_error(ctx.status)) {
                if (parse_uri_step<Options>(ctx)) {
                    break;
                }
            }
        }
    } // namespace details

    namespace details {
        template <typename CharT>
        [[nodiscard]] static constexpr bool is_tab_or_newline(CharT const ch) noexcept {
            return ch == static_cast<CharT>('\t') || ch == static_cast<CharT>('\n') || ch == static_cast<CharT>('\r');
        }

        /// Remove newlines and tabs from input before doing anything else, or at least validate that we don't have any
        /// of them in the input.
        template <uri_options Options, URIContext CtxT>
        [[nodiscard]] static constexpr bool preprocess_tabs_and_newlines(CtxT& ctx) noexcept(CtxT::is_nothrow) {
            // https://url.spec.whatwg.org/#concept-basic-url-parser
            if constexpr (!Options.ignore_tabs_or_newlines) {
                return true;
            } else {
                auto src = ctx.beg;
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
                      [&](char_type* buf, stl::size_t const max_length) noexcept {
                          // raw copy
                          for (auto pos = ctx.beg; pos != src;) {
                              *buf++ = *pos++; // NOLINT(*-pointer-arithmetic)
                          }

                          // skip copy
                          for (auto pos = src; pos != ctx.end; ++pos) {
                              if (!is_tab_or_newline(*pos)) {
                                  *buf++ = *pos; // NOLINT(*-pointer-arithmetic)
                              }
                          }
                          return max_length;
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
        if (!details::preprocess_tabs_and_newlines<Options>(ctx)) {
            return;
        }
        details::continue_parsing_uri<Options>(ctx);
    }

    /// View-only
    template <uri_options Options = {}, typename CharT = char>
    static constexpr auto parse_uri(stl::basic_string_view<CharT> const str) noexcept {
        using context_type = uri_context<uri_components_u32_view<CharT>>;
        auto context       = create<context_type>(str.data(), str.data() + str.size());
        parse_uri<Options>(context);
        return context;
    }

    template <uri_options Options = {}, typename CharT, URIComponents BaseCompT>
    static constexpr auto parse_uri(stl::basic_string_view<CharT> const the_url, BaseCompT&& base_comps)
      noexcept(false) {
        using context_type = uri_context<uri_components_owning<CharT>, BaseCompT>;
        auto context =
          create<context_type>(the_url.data(), the_url.data() + the_url.size(), stl::forward<BaseCompT>(base_comps));
        parse_uri<Options>(context);
        return context;
    }

    /// Owning String
    template <uri_options Options = {}, typename CharT, typename AllocT, URIComponents BaseCompT>
    static constexpr auto parse_uri(stl::basic_string<CharT, stl::char_traits<CharT>, AllocT> const& the_url,
                                    BaseCompT&& base_comps) noexcept(false) {
        using context_type = uri_context<uri_components_owning<CharT, AllocT>, BaseCompT>;
        auto context       = create<context_type>(
          the_url.data(),
          the_url.data() + the_url.size(),
          stl::forward<BaseCompT>(base_comps),
          allocator_from(the_url));
        parse_uri<Options>(context);
        return context;
    }

    template <uri_options Options = {}, istl::StringLike StrT, typename CharT>
    static constexpr auto parse_uri(StrT const& the_url, stl::basic_string_view<CharT> const base_uri)
      noexcept(istl::StringView<StrT>) {
        using url_char_type = typename stl::remove_cvref_t<StrT>::value_type;
        static_assert(stl::same_as<url_char_type, CharT>,
                      "Origin's string's char type must be the same as the specified URI's string's char type.");
        using base_context_type = uri_context<uri_components_u32_view<CharT>>;

        base_context_type origin_context{.beg = base_uri.data(),
                                         .pos = base_uri.data(),
                                         .end = base_uri.data() + base_uri.size()};
        parse_uri<Options>(origin_context);

        return parse_uri<Options>(the_url, origin_context.out);
    }

} // namespace webpp::uri

#endif // WEBPP_URI_PARSE_URI_HPP
