// Created by moisrex on 11/4/20.

#ifndef WEBPP_URI_QUERIES_HPP
#define WEBPP_URI_QUERIES_HPP

#include "../memory/allocators.hpp"
#include "../std/map.hpp"
#include "./details/constants.hpp"
#include "./details/uri_components.hpp"
#include "encoding.hpp"

namespace webpp::uri {


    template <typename... T>
    static constexpr void
    parse_queries(uri::parsing_uri_context<T...>& ctx) noexcept(uri::parsing_uri_context<T...>::is_nothrow) {
        // https://url.spec.whatwg.org/#query-state
        using ctx_type         = uri::parsing_uri_context<T...>;
        using char_type        = typename ctx_type::char_type;
        using string_view_type = stl::basic_string_view<char_type>;

        if (ctx.pos == ctx.end) {
            // todo
            return;
        }

        auto* const beg = ctx.pos;

        // find the end of the queries
        for (;;) {

            // find the next non-query character:
            ctx.pos =
              uri::details::QUERY_OR_FRAGMENT_NOT_PCT_ENCODED<char_type>.find_first_not_in(ctx.pos, ctx.end);

            if (ctx.pos == ctx.end) {
                break; // the end of the queries, is the end of the URI
            }

            switch (*ctx.pos) {
                case '#':
                    ctx.out.clear_fragment();
                    uri::set_valid(ctx.status, uri_status::valid_fragment);
                    ++ctx.pos;
                    break;
                default:
                    uri::set_warning(ctx.status, uri_status::invalid_character);
                    ++ctx.pos;
                    // invalid characters are not errors
                    continue;
            }
            break;
        }

        if constexpr (ctx_type::is_modifiable) {
            auto const             query_percent_encode_set = is_special_scheme(ctx.out.scheme())
                                                                ? uri::details::SPECIAL_QUERIES_ENCODE_SET
                                                                : uri::details::QUERIES_ENCODE_SET;
            string_view_type const input{beg, ctx.pos};
            auto&                  output = ctx.out.queries();
            encode_uri_component(input, output, query_percent_encode_set);
        } else {
            ctx.out.queries(beg, ctx.pos);
        }
    }


    template <typename StringType = stl::string,
              typename AllocType  = typename stl::remove_cvref_t<StringType>::allocator_type>
    struct basic_queries : public istl::map_of_strings<StringType, AllocType> {
        using super                         = istl::map_of_strings<StringType, AllocType>;
        using string_type                   = stl::remove_cvref_t<StringType>;
        using key_type                      = typename super::key_type;
        using mapped_type                   = typename super::mapped_type;
        using char_type                     = typename string_type::value_type;
        using value_type                    = stl::pair<key_type, mapped_type>;
        using allocator_type                = typename super::allocator_type;
        static constexpr auto allowed_chars = details::QUERY_OR_FRAGMENT_NOT_PCT_ENCODED<char_type>;

        template <typename... Args>
            requires requires(Args... args) { super{stl::forward<Args>(args)...}; }
        constexpr basic_queries(Args&&... args) noexcept(noexcept(super(stl::forward<Args>(args)...)))
          : super{stl::forward<Args>(args)...} {}


        /**
         * Get the raw string non-decoded size
         */
        [[nodiscard]] stl::size_t raw_string_size() const noexcept {
            // todo: we could remove lambda; or we even can use an iterator_wrapper and use "std::reduce"
            return [this]() noexcept -> stl::size_t {
                stl::size_t sum = 0;
                for (auto const& [key, value] : *this) {
                    sum += key.size();
                    sum += value.size();
                }
                return sum;
            }() + (this->size() * 2) - 2;
        }

        void append_to(istl::String auto& str) const {
            str.append("?");
            for (const auto& [key, value] : *this) {
                encode_uri_component(key, str, allowed_chars);
                str.append("=");
                encode_uri_component(value, str, allowed_chars);
                str.append("&");
            }
            str.pop_back(); // remove the "&" (or remove the unneeded "?" if it's empty)
        }
    };

} // namespace webpp::uri

#endif // WEBPP_URI_QUERIES_HPP
