// Created by moisrex on 1/20/25.

#ifndef WEBPP_URI_PARSE_URI_HPP
#define WEBPP_URI_PARSE_URI_HPP

#include "./parse_authority.hpp"
#include "./parse_host.hpp"

namespace webpp::uri {

    namespace details {
        template <uri_options Options = uri_options{}, URIContext CtxT>
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

        template <uri_options Options = uri_options{}, URIContext CtxT>
        static constexpr void continue_parsing_uri(CtxT& ctx) noexcept(CtxT::is_nothrow) {
            while (!has_error(ctx.status)) {
                if (parse_uri_step<Options>(ctx)) {
                    break;
                }
            }
        }
    } // namespace details

    template <uri_options Options = uri_options{}, URIContext CtxT>
    static constexpr void parse_uri(CtxT& ctx) noexcept(CtxT::is_nothrow) {
        details::continue_parsing_uri<Options>(ctx);
    }

    template <uri_options Options = uri_options{}, istl::StringView StrV = stl::string_view>
    static constexpr auto parse_uri(StrV str) noexcept {
        using iterator     = typename StrV::const_iterator;
        using context_type = parsing_uri_context<StrV, iterator>;
        context_type context{.beg = str.begin(),
                             .pos = str.begin(), // current position is start
                             .end = str.end()};
        parse_uri<Options>(context);
        return context;
    }

    template <uri_options Options = uri_options{}, istl::StringLike StrT, typename SegType>
    static constexpr auto parse_uri(StrT const& the_url, uri_components<SegType> const& origin_context)
      noexcept(istl::StringView<StrT>) {
        using iterator             = typename StrT::const_iterator;
        using base_components_type = uri_components<SegType>;
        using base_seg_type        = typename base_components_type::seg_type;
        using context_type         = parsing_uri_context<StrT, iterator, base_seg_type>;

        context_type context{.beg  = the_url.begin(),
                             .pos  = the_url.begin(), // current position is start
                             .end  = the_url.end(),
                             .base = origin_context};
        parse_uri<Options>(context);
        return context;
    }

    template <uri_options Options = uri_options{}, istl::StringLike StrT, istl::StringViewifiable OStrV>
    static constexpr auto parse_uri(StrT const& the_url, OStrV&& base_uri) noexcept(istl::StringView<StrT>) {
        using iterator = typename StrT::const_iterator;
        static_assert(stl::same_as<iterator, typename OStrV::const_iterator>,
                      "Origin's string's char type must be the same as the specified URI's string's char type.");
        auto const base         = istl::string_viewify(stl::forward<OStrV>(base_uri));
        using base_context_type = parsing_uri_context<stl::uint32_t, iterator>;

        base_context_type origin_context{.beg = base.begin(), .pos = base.begin(), .end = base.end()};
        parse_uri<Options>(origin_context);

        return parse_uri<Options>(the_url, origin_context.out);
    }

} // namespace webpp::uri

#endif // WEBPP_URI_PARSE_URI_HPP
