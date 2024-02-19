// Created by moisrex on 11/4/20.

#ifndef WEBPP_URI_QUERIES_HPP
#define WEBPP_URI_QUERIES_HPP

#include "../std/map.hpp"
#include "./details/constants.hpp"
#include "./details/uri_components_encoding.hpp"
#include "encoding.hpp"

namespace webpp::uri {


    template <uri_parsing_options Options = uri_parsing_options{}, ParsingURIContext CtxT>
    static constexpr void parse_queries(CtxT& ctx) noexcept(CtxT::is_nothrow) {
        // https://url.spec.whatwg.org/#query-state
        using ctx_type = CtxT;

        if constexpr (Options.parse_queries) {
            webpp_static_constexpr auto interesting_characters =
              !ctx_type::is_segregated ? details::ascii_bitmap('#', '%')
                                       : details::ascii_bitmap('#', '%', '=', '&');


            if (ctx.pos == ctx.end) {
                set_valid(ctx.status, uri_status::valid);
                return;
            }

            auto const query_percent_encode_set =
              is_special_scheme(ctx.scheme)
                ? details::SPECIAL_QUERIES_ENCODE_SET
                : details::QUERIES_ENCODE_SET;

            bool in_value = false;

            details::component_encoder<components::queries, ctx_type> encoder{ctx};

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
                        clear<components::fragment>(ctx);
                        set_valid(ctx.status, uri_status::valid_fragment);
                        break;
                    case '%':
                        if (!encoder.validate_percent_encode()) {
                            if constexpr (Options.allow_invalid_characters) {
                                set_warning(ctx.status, uri_status::invalid_character);
                            } else {
                                set_error(ctx.status, uri_status::invalid_queries_character);
                                return;
                            }
                        }
                        continue;
                    case '=':
                        if (!in_value) {
                            if constexpr (ctx_type::is_segregated) {
                                encoder.set_query_name();
                            }
                            encoder.skip_separator();
                            encoder.reset_begin();
                        } else {
                            encoder.append_query_value(1);
                        }
                        in_value = true;
                        continue;
                    case '&':
                        if constexpr (ctx_type::is_segregated) {
                            encoder.set_query_value();
                            in_value = false;
                        }
                        encoder.skip_separator();
                        encoder.next_query();
                        continue;
                    default:
                        if constexpr (Options.allow_invalid_characters) {
                            set_warning(ctx.status, uri_status::invalid_character);
                        } else {
                            set_error(ctx.status, uri_status::invalid_queries_character);
                            return;
                        }
                        encoder.skip_separator();
                        // invalid characters are not errors
                        continue;
                }
                break;
            }
            if constexpr (ctx_type::is_segregated) {
                if (in_value) {
                    encoder.set_query_value();
                } else {
                    encoder.set_query_name();
                }
            }
            encoder.set_value();

            if (ctx.pos == ctx.end) {
                set_valid(ctx.status, uri_status::valid);
            } else {
                ++ctx.pos;
            }
            encoder.next_query();

        } else { // don't parse queries
            if (ctx.pos == ctx.end) {
                set_valid(ctx.status, uri_status::valid);
            } else {
                if constexpr (Options.allow_invalid_characters) {
                    set_warning(ctx.status, uri_status::invalid_character);
                } else {
                    set_error(ctx.status, uri_status::invalid_queries_character);
                }
            }
        }
    }

    /**
     * @brief Basic Queries (or sometimes called Searches, like in WHATWG) stored as key-values
     * @tparam StringType String or String View type used as the storage type
     * @tparam AllocType Allocator type
     */
    template <istl::StringLike StringType = stl::string,
              Allocator        AllocType  = allocator_type_from_t<StringType>>
    struct basic_queries : istl::map_of_strings<StringType, AllocType> {
        using map_type       = istl::map_of_strings<StringType, AllocType>;
        using string_type    = StringType;
        using key_type       = typename map_type::key_type;
        using mapped_type    = typename map_type::mapped_type;
        using char_type      = typename string_type::value_type;
        using value_type     = stl::pair<key_type, mapped_type>;
        using pack_type      = typename map_type::value_type;
        using allocator_type = typename map_type::allocator_type;

        using iterator       = typename map_type::iterator;
        using const_iterator = typename map_type::const_iterator;

        static constexpr bool is_modifiable = istl::ModifiableString<string_type>;
        static constexpr bool is_segregated = true;
        static constexpr auto allowed_chars = details::QUERY_OR_FRAGMENT_NOT_PCT_ENCODED<char_type>;

        template <typename... Args>
            requires requires(Args... args) { map_type{stl::forward<Args>(args)...}; }
        explicit constexpr basic_queries(Args&&... args) noexcept(
          noexcept(map_type(stl::forward<Args>(args)...)))
          : map_type{stl::forward<Args>(args)...} {}

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

        /**
         * @brief Replace the values with the specified raw data, without parsing
         * @param beg start of the value
         * @param end the end of the value
         */
        constexpr void assign(iterator beg, iterator end) {
            // first "name" is chosen for the whole value, because of the algorithm that gets the queries will
            // be correct that way

            this->clear();
            if constexpr (is_modifiable) {
                istl::emplace_one(static_cast<basic_queries&>(*this),
                                  pack_type{
                                    key_type{beg, end, this->get_allocator()},
                                    mapped_type{this->get_allocator()}
                });
            } else {
                istl::emplace_one(static_cast<basic_queries&>(*this),
                                  pack_type{
                                    key_type{beg, end},
                                    mapped_type{}
                });
            }
        }

        [[nodiscard]] constexpr auto& storage_ref() noexcept {
            return static_cast<map_type&>(*this);
        }

        [[nodiscard]] constexpr auto const& storage_ref() const noexcept {
            return static_cast<map_type const&>(*this);
        }

        /**
         * @brief check if we have value
         * @return false if we don't have anything
         */
        [[nodiscard]] constexpr bool has_value() const noexcept {
            return !this->empty();
        }

        template <istl::String NStrT = stl::string>
        constexpr void to_string(NStrT& out) const {
            if (this->empty()) {
                return;
            }
            for (auto pos = this->begin();;) {
                auto const [name, value]  = *pos;
                out                      += name;
                if (!value.empty()) {
                    out += '=';
                    out += value;
                }
                if (++pos == this->end()) {
                    break;
                }
                out += '&';
            }
        }

        template <istl::String NStrT = stl::string, typename... Args>
        [[nodiscard]] constexpr NStrT as_string(Args&&... args) const {
            NStrT out{stl::forward<Args>(args)...};
            to_string(out);
            return out;
        }
    };

} // namespace webpp::uri

#endif // WEBPP_URI_QUERIES_HPP
