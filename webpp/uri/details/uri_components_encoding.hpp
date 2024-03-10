// Created by moisrex on 11/12/23.

#ifndef URI_COMPONENTS_ENCODING_HPP
#define URI_COMPONENTS_ENCODING_HPP

#include "../../std/string_like.hpp"
#include "../../strings/append.hpp"
#include "../../strings/to_case.hpp"
#include "../encoding.hpp"
#include "./uri_components.hpp"

namespace webpp::uri::details {

    /**
     * @brief Encode/Decode a piece of URI
     * @tparam Comp URI Component that's being parsed
     * @tparam CtxType uri_parsing_context type
     */
    template <components Comp, typename CtxType>
    struct component_encoder {
        using ctx_type        = CtxType;
        using seg_type        = typename ctx_type::seg_type;
        using iterator        = typename ctx_type::iterator;
        using iter_traits     = stl::iterator_traits<iterator>;
        using difference_type = typename iter_traits::difference_type;
        using value_type      = typename stl::iterator_traits<iterator>::value_type;
        using char_type       = typename ctx_type::char_type;


      private:
        static constexpr bool is_vec =
          ctx_type::is_segregated && (components::host == Comp || components::path == Comp);
        static constexpr bool is_map = ctx_type::is_segregated && components::queries == Comp;
        static constexpr bool is_seg = is_vec || is_map;

        /// if it's segregated:
        ///   if it's modifiable queries, map::value_type (pair<string, string>),
        ///   if it's modifiable path/host, vector::iterator
        /// else if it's not segregated but still modifiable:
        ///   vec_iterator which is seg_type*
        /// otherwise, nothing_type
        using output_type =
          stl::conditional_t<is_map,
                             typename ctx_type::map_value_type,
                             stl::conditional_t<is_vec, typename ctx_type::vec_iterator, istl::nothing_type>>;


        [[no_unique_address]] output_type output{};
        ctx_type*                         ctx;
        iterator                          beg = ctx->pos;

      public:
        [[nodiscard]] constexpr auto const& context() const noexcept {
            return *ctx;
        }

        [[nodiscard]] constexpr auto& context() noexcept {
            return *ctx;
        }

        [[nodiscard]] constexpr decltype(auto) get_output() const noexcept {
            return uri::get_output<Comp>(*ctx);
        }

        [[nodiscard]] constexpr decltype(auto) get_out_seg() const noexcept {
            if constexpr (is_seg) {
                return *output;
            } else {
                return uri::get_output<Comp>(*ctx);
            }
        }

        /// call this when encoding/decoding is done; I'm not putting this into the destructor because of
        /// explicitness
        constexpr void set_value(iterator start, iterator end) noexcept(
          ctx_type::is_nothrow || is_seg || ctx_type::is_modifiable) {
            if constexpr (!is_seg && !ctx_type::is_modifiable) {
                uri::set_value<Comp>(*ctx, start, end);
            }
        }

        constexpr void set_value() noexcept(ctx_type::is_nothrow || is_seg) {
            set_value(beg, ctx->pos);
        }

        template <ParsingURIContext InpCtxT>
        explicit constexpr component_encoder(InpCtxT& inp_ctx) noexcept(ctx_type::is_nothrow)
          : ctx{&inp_ctx},
            beg{ctx->pos} {}

        template <uri_encoding_policy Policy = uri_encoding_policy::skip_chars>
        constexpr void encode_or_set(
          iterator                             pos,
          iterator                             end,
          [[maybe_unused]] CharSet auto const& policy_chars) noexcept(ctx_type::is_nothrow) {
            if constexpr (ctx_type::is_modifiable) {
                encode_uri_component<Policy>(pos, end, get_out_seg(), policy_chars);
            } else {
                set_value(pos, end);
            }
        }

        template <uri_encoding_policy Policy = uri_encoding_policy::skip_chars>
        [[nodiscard]] constexpr bool encode_or_validate(
          iterator&           pos,
          iterator            end,
          CharSet auto const& policy_chars,
          CharSet auto const& invalid_chars) noexcept(ctx_type::is_nothrow) {
            if constexpr (ctx_type::is_modifiable) {
                return encode_uri_component<Policy>(pos, end, get_out_seg(), policy_chars, invalid_chars);
            } else {
                if constexpr (Policy == uri_encoding_policy::skip_chars) {
                    pos = invalid_chars.find_first_not_in(pos, end);
                } else {
                    pos = invalid_chars.find_first_in(pos, end);
                }
                return pos == end;
            }
        }

        template <uri_encoding_policy Policy = uri_encoding_policy::skip_chars>
        [[nodiscard]] constexpr bool encode_or_validate(
          iterator&           pos,
          iterator            end,
          CharSet auto const& policy_chars) noexcept(ctx_type::is_nothrow) {
            if constexpr (ctx_type::is_modifiable) {
                encode_uri_component<Policy>(pos, end, get_out_seg(), policy_chars);
                return pos == end;
            } else {
                if constexpr (Policy == uri_encoding_policy::skip_chars) {
                    pos = policy_chars.find_first_not_in(pos, end);
                } else {
                    pos = policy_chars.find_first_in(pos, end);
                }
                return pos == end;
            }
        }

        /**
         * @brief Encode if the context is modifiable, otherwise just validate the invalid characters
         * @tparam Policy
         * @param policy_chars encode these characters if encoding is possible
         * @param invalid_chars invalid character or allowed characters depending on the policy
         * @returns successful until the end (== didn't find any invalid chars)
         */
        template <uri_encoding_policy Policy = uri_encoding_policy::skip_chars>
        [[nodiscard]] constexpr bool encode_or_validate(
          CharSet auto const& policy_chars,
          CharSet auto const& invalid_chars) noexcept(ctx_type::is_nothrow) {
            return encode_or_validate<Policy>(ctx->pos, ctx->end, policy_chars, invalid_chars);
        }

        template <uri_encoding_policy Policy = uri_encoding_policy::skip_chars>
        [[nodiscard]] constexpr bool encode_or_validate(CharSet auto const& policy_chars) noexcept(
          ctx_type::is_nothrow) {
            return encode_or_validate<Policy>(ctx->pos, ctx->end, policy_chars);
        }

        template <uri_encoding_policy Policy = uri_encoding_policy::skip_chars>
        [[nodiscard]] constexpr bool encode_or_validate_map(
          [[maybe_unused]] CharSet auto const& policy_chars,
          CharSet auto const&                  invalid_chars,
          [[maybe_unused]] bool const          in_value) noexcept(ctx_type::is_nothrow) {
            if constexpr (ctx_type::is_modifiable && is_map) {
                return encode_uri_component<Policy>(
                  ctx->pos,
                  ctx->end,
                  !in_value ? output.first : output.second,
                  policy_chars,
                  invalid_chars);
            } else {
                return encode_or_validate<Policy>(policy_chars, invalid_chars);
            }
        }

        /**
         * @brief Decode if the context is modifiable, otherwise just validate the invalid characters
         * @tparam Policy
         * @param policy_chars invalid character or allowed characters depending on the policy
         * @returns successful until the end (== didn't find any invalid chars)
         */
        template <uri_encoding_policy Policy = uri_encoding_policy::skip_chars>
        [[nodiscard]] constexpr bool decode_or_validate(CharSet auto const& policy_chars) noexcept(
          ctx_type::is_nothrow) {
            if constexpr (ctx_type::is_modifiable) {
                return decode_uri_component<Policy>(ctx->pos, ctx->end, get_out_seg(), policy_chars);
            } else {
                if constexpr (Policy == uri_encoding_policy::skip_chars) {
                    ctx->pos = policy_chars.find_first_not_in(ctx->pos, ctx->end);
                } else {
                    ctx->pos = policy_chars.find_first_in(ctx->pos, ctx->end);
                }
                return ctx->pos == ctx->end;
            }
        }

        /// Convert to lowercase and also decode
        template <uri_encoding_policy Policy = uri_encoding_policy::skip_chars>
        [[nodiscard]] constexpr bool decode_or_tolower(CharSet auto const& policy_chars) noexcept(
          ctx_type::is_nothrow) {
            if constexpr (ctx_type::is_modifiable) {
                while (ctx->pos != ctx->end) {
                    if (decode_uri_component<Policy>(ctx->pos, ctx->end, get_out_seg(), policy_chars)) {
                        return true;
                    }
                    webpp_static_constexpr char_type diff = 'a' - 'A';
                    if (*ctx->pos >= 'A' && *ctx->pos <= 'Z') {
                        get_out_seg() += *ctx->pos + diff;
                        ++ctx->pos;
                        continue;
                    }
                    return false;
                }
                return true;
            } else {
                return decode_or_validate<Policy>(policy_chars);
            }
        }

        /// Set the beginning to current position
        constexpr void reset_begin() noexcept {
            beg = ctx->pos;
        }

        [[nodiscard]] constexpr bool is_segment_empty() const noexcept {
            return beg == ctx->pos;
        }

        constexpr void reset_segment_start() noexcept {
            if constexpr (is_vec || ctx_type::is_modifiable) {
                reset_begin();
            }
        }

        constexpr void clear_segment() noexcept {
            if constexpr (is_vec && ctx_type::is_modifiable) {
                output->clear();
            }
            reset_segment_start();
        }

        [[nodiscard]] constexpr iterator segment_begin() const noexcept {
            return beg;
        }

        constexpr void skip_separator(difference_type count) noexcept {
            if constexpr (ctx_type::is_modifiable && !is_seg) {
                for (; count != 0; --count) {
                    append_to(get_output(), *ctx->pos++);
                }
            } else {
                ctx->pos += count;
            }
        }

        /// parsing path requires this so we can make sure the modifable strings's separator is always '/' and
        /// not '\\' if the input contains that separator
        constexpr void skip_separator(char_type separator, difference_type count = 1) noexcept {
            if constexpr (ctx_type::is_modifiable && !is_seg) {
                append_to(get_output(), separator);
                ctx->pos += count;
            } else {
                return skip_separator(count);
            }
        }

        constexpr void skip_separator() noexcept {
            if constexpr (ctx_type::is_modifiable && !is_seg) {
                append_to(get_output(), *ctx->pos++);
            } else {
                ++ctx->pos;
            }
        }

        constexpr void ignore_character(difference_type count = 1) noexcept {
            ctx->pos += count;
        }

        constexpr void append_n(difference_type count) noexcept {
            if constexpr (ctx_type::is_modifiable && !is_map) {
                for (; count != 0; --count) {
                    append_to(get_out_seg(), *ctx->pos++);
                }
            } else {
                ctx->pos += count;
            }
        }

        constexpr void append(char_type inp_char) noexcept {
            if constexpr (ctx_type::is_modifiable && !is_map) {
                append_to(get_out_seg(), inp_char);
            }
        }

        constexpr void append_inplace_of(char_type inp_char, difference_type count = 1) noexcept {
            if constexpr (ctx_type::is_modifiable && !is_map) {
                append_to(get_out_seg(), inp_char);
            }
            ctx->pos += count;
        }

        /// Check if the next 2 characters are valid percent encoded ascii-hex digits.
        template <bool CheckNewlinesAndTabs = false>
        [[nodiscard]] constexpr bool validate_percent_encode() noexcept {
            using ascii::is_hex_digit;

            // NOLINTBEGIN(*-inc-dec-in-conditions)
            if constexpr (!CheckNewlinesAndTabs) {
                auto       cur      = ctx->pos;
                bool const is_valid = cur++ + 2 <= ctx->end && is_hex_digit(*cur++) && is_hex_digit(*cur);
                append_n(cur - ctx->pos);
                return is_valid;
            } else {
                append_n(1);
                switch (ctx->pos - ctx->end) {
                    case 0:
                    case 1: return false;
                    case 2: return is_hex_digit(*ctx->pos++) && is_hex_digit(*ctx->pos);
                    default: {
                        int count = 0;
                        for (;;) {
                            switch (*ctx->pos) {
                                case '\t':
                                case '\r':
                                case '\n':
                                    ++ctx->pos;
                                    if (ctx->pos == ctx->end) {
                                        return false;
                                    }
                                    continue;
                                default: {
                                    bool const is_valid_char = is_hex_digit(*ctx->pos);
                                    append_inplace_of(*ctx->pos, 1);
                                    if (!is_valid_char) {
                                        return false;
                                    }
                                    ++count;
                                    if (count == 2 || ctx->pos == ctx->end) {
                                        break;
                                    }
                                    continue;
                                }
                            }
                            break;
                        }
                        return count == 2;
                    }
                }
            }
            // NOLINTEND(*-inc-dec-in-conditions)
        }

        constexpr void pop_back([[maybe_unused]] difference_type hint = 0) noexcept {
            if constexpr (is_vec && ctx_type::is_modifiable) {
                if (get_output().size() > 1) {
                    get_output().pop_back();
                    output = get_output().begin() + static_cast<difference_type>(get_output().size() - 1);
                } else if (get_output().size() == 1) {
                    output->clear();
                }
            } else if constexpr (is_vec) {
                if (is_segment_empty()) {
                    if (!get_output().empty()) {
                        get_output().pop_back();
                    }
                } else {
                    reset_segment_start();
                }
            } else if constexpr (ctx_type::is_modifiable) {
                using output_t  = stl::remove_cvref_t<decltype(get_output())>;
                using size_type = typename output_t::size_type;
                if (!get_output().empty()) {
                    get_output().erase(get_output().size() - static_cast<size_type>(hint));
                }
            }
        }

        constexpr void start_segment() noexcept(ctx_type::is_nothrow || !is_vec) {
            if constexpr (is_vec && ctx_type::is_modifiable) {
                // the non-modifiable version is the one that needs to be set, the modified versions already
                // contain the right value at this point in time
                istl::emplace_one(get_output(), get_output().get_allocator());
                output = get_output().begin() + static_cast<difference_type>(get_output().size() - 1);
            }
        }

        /// Call this when you're done with the current segment (e.g.: reaching a dot for host, or a slash
        /// for path)
        constexpr void end_segment(iterator inp_beg, iterator end) noexcept(ctx_type::is_nothrow || !is_vec) {
            if constexpr (is_vec && !ctx_type::is_modifiable) {
                // the non-modifiable version is the one that needs to be set, the modified versions already
                // contain the right value at this point in time
                istl::emplace_one(get_output(), inp_beg, end);
                reset_begin();
            }
        }

        constexpr void end_segment() noexcept(ctx_type::is_nothrow || !is_vec) {
            end_segment(beg, ctx->pos);
        }

        /// 1. Skip the separator, and
        /// 2. Set the segment start
        constexpr void next_segment(difference_type sep_count = 1) noexcept(ctx_type::is_nothrow) {
            if constexpr (is_seg) {
                if constexpr (ctx_type::is_modifiable) {
                    skip_separator(sep_count);
                    reset_segment_start();
                    start_segment();
                } else {
                    end_segment();
                    skip_separator(sep_count);
                    reset_segment_start();
                }
            } else {
                skip_separator(sep_count);
                end_segment();
                reset_segment_start();
            }
        }

        constexpr void next_segment_of(char_type separator, difference_type sep_count = 1) noexcept(
          ctx_type::is_nothrow) {
            if constexpr (is_seg) {
                if constexpr (ctx_type::is_modifiable) {
                    skip_separator(sep_count);
                    reset_segment_start();
                    end_segment();
                } else {
                    end_segment();
                    skip_separator(sep_count);
                    reset_segment_start();
                }
            } else {
                if constexpr (ctx_type::is_modifiable) {
                    skip_separator(separator, sep_count);
                } else {
                    skip_separator(sep_count);
                }
                end_segment();
                reset_segment_start();
            }
            start_segment();
        }

        constexpr void set_query_name() noexcept(ctx_type::is_nothrow)
            requires(is_map)
        {
            if constexpr (!ctx_type::is_modifiable) {
                istl::assign(output.first, beg, ctx->pos);
            }
        }

        constexpr void set_query_value() noexcept(ctx_type::is_nothrow)
            requires(is_map)
        {
            if constexpr (!ctx_type::is_modifiable) {
                istl::assign(output.second, beg, ctx->pos);
                beg = ctx->pos + 1;
            }
        }

        constexpr void append_query_value(difference_type count) noexcept(ctx_type::is_nothrow) {
            if constexpr (is_map) {
                if constexpr (!ctx_type::is_modifiable) {
                    ctx->pos += count;
                    istl::assign(output.second, beg, ctx->pos);
                } else {
                    output.second += *ctx->pos;
                    ctx->pos      += count;
                }
            }
        }

        constexpr void next_query() noexcept(ctx_type::is_nothrow) {
            if constexpr (is_map) {
                if (!output.first.empty() || !output.second.empty()) {
                    get_output().emplace(output);
                }
                istl::clear(output.first);
                istl::clear(output.second);
            }
            reset_begin();
        }
    };


} // namespace webpp::uri::details

#endif // URI_COMPONENTS_ENCODING_HPP
