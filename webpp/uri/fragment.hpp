// Created by moisrex on 11/4/20.

#ifndef WEBPP_URI_FRAGMENT_HPP
#define WEBPP_URI_FRAGMENT_HPP

#include "../std/string.hpp"
#include "./details/constants.hpp"
#include "./details/uri_components.hpp"
#include "encoding.hpp"

namespace webpp::uri {

    template <typename... T>
    static constexpr void
    parse_fragment(uri::parsing_uri_context<T...>& ctx) noexcept(uri::parsing_uri_context<T...>::is_nothrow) {
        // https://url.spec.whatwg.org/#fragment-state
        using ctx_type         = uri::parsing_uri_context<T...>;
        using char_type        = typename ctx_type::char_type;
        using string_view_type = stl::basic_string_view<char_type>;

        if constexpr (ctx_type::is_modifiable) {
            auto& output = ctx.out.get_fragment_ref();
            // todo: this is encode, not decode
            bool const is_valid = decode_uri_component(string_view_type{ctx.beg, ctx.end},
                                                       output,
                                                       uri::details::FRAGMENT_ENCODE_SET);
            if (!is_valid) {
                ctx.status = stl::to_underlying(uri_status::invalid_character);
                return;
            }
        } else {
            ctx.out.set_fragment(ctx.pos, ctx.end);
        }
        ctx.status |= stl::to_underlying(uri_status::valid);
    }

    template <istl::String StringType = stl::string>
    struct basic_fragment : stl::remove_cvref_t<StringType> {
        using string_type = stl::remove_cvref_t<StringType>;
        using char_type   = typename string_type::value_type;

        // todo: check RFC and see if this is the right one; remove this comment if it's all right
        static constexpr auto allowed_chars = details::QUERY_OR_FRAGMENT_NOT_PCT_ENCODED<char_type>;

        template <typename... T>
        constexpr basic_fragment(T&&... args) : string_type{stl::forward<T>(args)...} {}


        void append_to(istl::String auto& str) const {
            if (!this->empty()) {
                str.push_back('#');
                encode_uri_component(*this, str, allowed_chars);
            }
        }

        // todo: add to and from json, xml, and other string types that makes sense to use in uri's fragments
    };


} // namespace webpp::uri

#endif // WEBPP_URI_FRAGMENT_HPP
