// Created by moisrex on 1/20/25.

#ifndef WEBPP_URI_PARSE_URI_HPP
#define WEBPP_URI_PARSE_URI_HPP

#include "../../memory/allocator_concepts.hpp"
#include "./parse_authority.hpp"
#include "./parse_host.hpp"
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
                case valid_opaque_path: parse_opaque_path<Options>(ctx); break;
                case valid_path: parse_path<Options>(ctx); break;
                case valid_queries: parse_queries<Options>(ctx); break;
                case valid_fragment: parse_fragment<Options>(ctx); break;
                case unparsed: parse_scheme<Options>(ctx); break; // start from the beginning
                default: stl::unreachable(); break;               // should be impossible
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

    template <uri_options Options, URIContext CtxT>
    static constexpr void parse_uri(CtxT& ctx) noexcept(CtxT::is_nothrow) {
        details::continue_parsing_uri<Options>(ctx);
    }

    /// View-only
    template <uri_options Options, typename CharT = char>
    static constexpr auto parse_uri(stl::basic_string_view<CharT> const str) noexcept {
        using context_type = uri_context<uri_components_u32_view<CharT>>;
        auto context       = create<context_type>(str.begin(), str.end());
        parse_uri<Options>(context);
        return context;
    }

    template <uri_options Options, typename CharT, URIComponents BaseCompT>
    static constexpr auto parse_uri(stl::basic_string_view<CharT> const the_url, BaseCompT&& base_comps)
      noexcept(false) {
        using context_type = uri_context<uri_components_owning<CharT>, BaseCompT>;
        auto context       = create<context_type>(the_url.begin(), the_url.end(), stl::forward<BaseCompT>(base_comps));
        parse_uri<Options>(context);
        return context;
    }

    /// Owning String
    template <uri_options Options, typename CharT, typename AllocT, URIComponents BaseCompT>
    static constexpr auto parse_uri(stl::basic_string<CharT, stl::char_traits<CharT>, AllocT> const& the_url,
                                    BaseCompT&& base_comps) noexcept(false) {
        using context_type = uri_context<uri_components_owning<CharT, AllocT>, BaseCompT>;
        auto context       = create<context_type>(
          the_url.begin(),
          the_url.end(),
          stl::forward<BaseCompT>(base_comps),
          allocator_from(the_url));
        parse_uri<Options>(context);
        return context;
    }

    template <uri_options Options, istl::StringLike StrT, typename CharT>
    static constexpr auto parse_uri(StrT const& the_url, stl::basic_string_view<CharT> const base_uri)
      noexcept(istl::StringView<StrT>) {
        using iterator       = typename StrT::const_iterator;
        using const_iterator = stl::basic_string_view<CharT>::const_iterator;
        static_assert(stl::same_as<iterator, const_iterator>,
                      "Origin's string's char type must be the same as the specified URI's string's char type.");
        using base_context_type = uri_context<stl::uint32_t, iterator>;

        base_context_type origin_context{.beg = base_uri.begin(), .pos = base_uri.begin(), .end = base_uri.end()};
        parse_uri<Options>(origin_context);

        return parse_uri<Options>(the_url, origin_context.out);
    }

} // namespace webpp::uri

#endif // WEBPP_URI_PARSE_URI_HPP
