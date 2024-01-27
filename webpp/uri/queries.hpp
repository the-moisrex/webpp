// Created by moisrex on 11/4/20.

#ifndef WEBPP_URI_QUERIES_HPP
#define WEBPP_URI_QUERIES_HPP

#include "../std/map.hpp"
#include "./details/constants.hpp"
#include "./details/uri_components_encoding.hpp"
#include "encoding.hpp"

namespace webpp::uri {


    template <uri_parsing_options Options = uri_parsing_options{}, typename... T>
    static constexpr void parse_queries(parsing_uri_context<T...>& ctx) noexcept(
      parsing_uri_context<T...>::is_nothrow) {
        // https://url.spec.whatwg.org/#query-state
        using ctx_type = parsing_uri_context<T...>;

        if constexpr (Options.parse_queries) {
            webpp_static_constexpr auto interesting_characters =
              !ctx_type::is_segregated ? details::ascii_bitmap('#', '%')
                                       : details::ascii_bitmap('#', '%', '=', '&');


            if (ctx.pos == ctx.end) {
                set_valid(ctx.status, uri_status::valid);
                return;
            }

            auto const query_percent_encode_set =
              ctx.is_special ? details::SPECIAL_QUERIES_ENCODE_SET : details::QUERIES_ENCODE_SET;

            bool in_value = false;

            details::component_encoder<details::components::queries, ctx_type> encoder{ctx};

            // find the end of the queries
            for (;;) {
                // find the next non-query character:
                if (encoder.template encode_or_validate_map<uri_encoding_policy::encode_chars>(
                      query_percent_encode_set,
                      interesting_characters,
                      in_value))
                {
                    break;
                }

                switch (*ctx.pos) {
                    case '#':
                        ctx.out.clear_fragment();
                        set_valid(ctx.status, uri_status::valid_fragment);
                        break;
                    case '%':
                        if (!encoder.validate_percent_encode()) {
                            set_warning(ctx.status, uri_status::invalid_character);
                        }
                        continue;
                    case '=':
                        if constexpr (ctx_type::is_segregated) {
                            if (!in_value) {
                                encoder.set_query_name();
                            }
                            in_value = true;
                        }
                        encoder.skip_separator();
                        continue;
                    case '&':
                        if constexpr (ctx_type::is_segregated) {
                            encoder.set_query_value();
                            in_value = false;
                        }
                        encoder.skip_separator();
                        continue;
                    default:
                        set_warning(ctx.status, uri_status::invalid_character);
                        encoder.skip_separator();
                        // invalid characters are not errors
                        continue;
                }
                break;
            }
            encoder.set_value();

            if (ctx.pos == ctx.end) {
                set_valid(ctx.status, uri_status::valid);
            } else {
                ++ctx.pos;
            }

        } else { // don't parse queries
            if (ctx.pos == ctx.end) {
                set_valid(ctx.status, uri_status::valid);
            } else {
                set_warning(ctx.status, uri_status::invalid_character);
            }
        }
    }

    template <typename StringType = stl::string,
              typename AllocType  = typename stl::remove_cvref_t<StringType>::allocator_type>
    struct basic_queries : istl::map_of_strings<StringType, AllocType> {
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
        explicit constexpr basic_queries(Args&&... args) noexcept(
          noexcept(super(stl::forward<Args>(args)...)))
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
            for (auto const& [key, value] : *this) {
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
