// Created by moisrex on 11/4/20.

#ifndef WEBPP_URI_FRAGMENT_HPP
#define WEBPP_URI_FRAGMENT_HPP

#include "../std/string.hpp"
#include "./details/constants.hpp"
#include "./details/uri_components_encoding.hpp"
#include "encoding.hpp"

namespace webpp::uri {

    template <uri_parsing_options Options = uri_parsing_options{}, typename... T>
    static constexpr void parse_fragment(parsing_uri_context<T...>& ctx) noexcept(
      parsing_uri_context<T...>::is_nothrow) {
        // https://url.spec.whatwg.org/#fragment-state
        using ctx_type  = parsing_uri_context<T...>;
        using char_type = typename ctx_type::char_type;

        if constexpr (Options.parse_fragment) {
            details::component_encoder<details::components::fragment, ctx_type> encoder{ctx};
            for (;;) {
                if (encoder.template encode_or_validate<uri_encoding_policy::encode_chars>(
                      details::FRAGMENT_ENCODE_SET,
                      charset<char_type, 1>('%')))
                {
                    break;
                }
                switch (*ctx.pos) {
                    case '%':
                        if (validate_percent_encode(ctx.pos, ctx.end)) {
                            continue;
                        }
                        break;
                }
                set_warning(ctx.status, uri_status::invalid_character);
            }
            encoder.set_value();
            set_valid(ctx.status, uri_status::valid);
        } else {
            if (ctx.pos == ctx.end) {
                set_valid(ctx.status, uri_status::valid);
            } else {
                set_warning(ctx.status, uri_status::invalid_character);
            }
        }
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
