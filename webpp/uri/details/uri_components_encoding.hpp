// Created by moisrex on 11/12/23.

#ifndef URI_COMPONENTS_ENCODING_HPP
#define URI_COMPONENTS_ENCODING_HPP

#include "../../std/string_like.hpp"
#include "../../strings/append.hpp"
#include "../../strings/to_case.hpp"
#include "../encoding.hpp"
#include "./uri_components.hpp"

namespace webpp::uri::details {

    enum struct components : stl::uint8_t {
        scheme,
        host,
        username,
        password,
        port,
        path,
        queries,
        fragment
    };

    template <components Comp, typename... T>
    [[nodiscard]] constexpr decltype(auto) get_output(parsing_uri_context<T...>& ctx) noexcept {
        if constexpr (components::scheme == Comp) {
            return ctx.out.scheme_ref();
        } else if constexpr (components::username == Comp) {
            return ctx.out.username_ref();
        } else if constexpr (components::password == Comp) {
            return ctx.out.password_ref();
        } else if constexpr (components::port == Comp) {
            return ctx.out.port_ref();
        } else if constexpr (components::host == Comp) {
            return ctx.out.hostname_ref();
        } else if constexpr (components::path == Comp) {
            return ctx.out.path_ref();
        } else if constexpr (components::queries == Comp) {
            return ctx.out.queries_ref();
        } else if constexpr (components::fragment == Comp) {
            return ctx.out.fragment_ref();
        }
    }

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
        using difference_type = typename stl::iterator_traits<iterator>::difference_type;


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
        using output_type = stl::conditional_t<
          is_map,
          typename ctx_type::out_type::map_value_type,
          stl::conditional_t<is_vec, typename ctx_type::out_type::vec_iterator, istl::nothing_type>>;


        [[no_unique_address]] output_type output{};
        ctx_type*                         ctx;
        iterator                          beg = ctx->pos;

        [[nodiscard]] constexpr decltype(auto) get_output() const noexcept {
            return details::get_output<Comp>(*ctx);
        }

        [[nodiscard]] constexpr decltype(auto) get_out_seg() const noexcept {
            if constexpr (is_seg) {
                return *output;
            } else {
                return details::get_output<Comp>(*ctx);
            }
        }

      public:
        /// call this when encoding/decoding is done; I'm not putting this into the destructor because of
        /// explicitness
        constexpr void set_value(iterator start, iterator end) noexcept(
          ctx_type::is_nothrow || is_seg || ctx_type::is_modifiable) {
            if constexpr (!is_seg && !ctx_type::is_modifiable) {
                if constexpr (components::scheme == Comp) {
                    ctx->out.set_scheme(start, end);
                } else if constexpr (components::username == Comp) {
                    ctx->out.set_username(start, end);
                } else if constexpr (components::password == Comp) {
                    ctx->out.set_password(start, end);
                } else if constexpr (components::port == Comp) {
                    ctx->out.set_port(start, end);
                } else if constexpr (components::host == Comp) {
                    ctx->out.set_hostname(start, end);
                } else if constexpr (components::path == Comp) {
                    ctx->out.set_path(start, end);
                } else if constexpr (components::queries == Comp) {
                    ctx->out.set_queries(start, end);
                } else if constexpr (components::fragment == Comp) {
                    ctx->out.set_fragment(start, end);
                }
            }
        }

        constexpr void set_value() noexcept(ctx_type::is_nothrow || is_seg) {
            set_value(beg, ctx->pos);
        }

        template <typename... T>
        explicit constexpr component_encoder(parsing_uri_context<T...>& inp_ctx) noexcept(
          ctx_type::is_nothrow)
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
          iterator&                            pos,
          iterator                             end,
          [[maybe_unused]] CharSet auto const& policy_chars,
          CharSet auto const&                  invalid_chars) noexcept(ctx_type::is_nothrow) {
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

        /**
         * @brief Encode if the context is modifiable, otherwise just validate the invalid characters
         * @tparam Policy
         * @param policy_chars encode these characters if encoding is possible
         * @param invalid_chars invalid character or allowed characters depending on the policy
         * @returns successful until the end (== didn't find any invalid chars)
         */
        template <uri_encoding_policy Policy = uri_encoding_policy::skip_chars>
        [[nodiscard]] constexpr bool encode_or_validate(
          [[maybe_unused]] CharSet auto const& policy_chars,
          CharSet auto const&                  invalid_chars) noexcept(ctx_type::is_nothrow) {
            return encode_or_validate<Policy>(ctx->pos, ctx->end, policy_chars, invalid_chars);
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

        /// Set the beginning to current position
        constexpr void reset_begin() noexcept {
            beg = ctx->pos;
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
        }

        [[nodiscard]] constexpr iterator segment_begin() const noexcept {
            return beg;
        }

        constexpr void skip_separator(difference_type index = 1) noexcept {
            if constexpr (ctx_type::is_modifiable && !is_seg) {
                append_to(get_output(), *ctx->pos);
            }
            ctx->pos += index;
        }

        constexpr void pop_back() noexcept {
            if constexpr (is_vec && ctx_type::is_modifiable) {
                // using difference_type = typename seg_type::difference_type;
                if (!get_output().empty()) {
                    get_output().pop_back();
                }
                output = get_output().begin() + static_cast<difference_type>(get_output().size() - 1);
            } else if constexpr (is_vec) {
                if (!get_output().empty()) {
                    get_output().pop_back();
                }
            } else if constexpr (ctx_type::is_modifiable) {
                if (!get_output().empty()) {
                    get_output().erase(static_cast<stl::size_t>(ctx->pos - beg));
                }
            }
        }

        constexpr void start_segment() noexcept(ctx_type::is_nothrow || !is_vec) {
            if constexpr (is_vec) {
                // the non-modifiable version is the one that needs to be set, the modified versions already
                // contain the right value at this point in time
                if constexpr (ctx_type::is_modifiable) {
                    istl::collection::emplace_one(get_output(), get_output().get_allocator());
                    // using difference_type = typename seg_type::difference_type;
                    output = get_output().begin() + static_cast<difference_type>(get_output().size() - 1);
                }
            }
        }

        /// Call this when you're done with the current segment (e.g.: reaching a dot for host, or a slash
        /// for path) This is the same as next_segment, except it also sets the result first
        constexpr void end_segment(iterator inp_beg, iterator end) noexcept(ctx_type::is_nothrow || !is_vec) {
            if constexpr (is_vec) {
                // the non-modifiable version is the one that needs to be set, the modified versions already
                // contain the right value at this point in time
                if constexpr (!ctx_type::is_modifiable) {
                    istl::collection::emplace_one(get_output(), inp_beg, end);
                    reset_begin();
                }
            }
        }

        constexpr void end_segment() noexcept(ctx_type::is_nothrow || !is_vec) {
            end_segment(beg, ctx->pos);
        }

        constexpr void set_query_name() noexcept(ctx_type::is_nothrow)
            requires(is_map)
        {
            if constexpr (!ctx_type::is_modifiable) {
                istl::assign(output.first, beg, ctx->pos);
                reset_begin();
            }
        }

        constexpr void set_query_value() noexcept(ctx_type::is_nothrow)
            requires(is_map)
        {
            if constexpr (!ctx_type::is_modifiable) {
                istl::assign(output.second, beg, ctx->pos);
                beg = ctx->pos + 1;
            }

            // next query
            get_output().emplace(output);
        }
    };

    template <components Comp, typename... T>
    static constexpr void next_append_lowered(parsing_uri_context<T...>& ctx) noexcept(
      parsing_uri_context<T...>::is_nothrow) {
        using ctx_type = parsing_uri_context<T...>;
        if constexpr (ctx_type::is_modifiable) {
            if constexpr (ctx_type::is_segregated) {
                // todo
            } else {
                append_to(get_output<Comp>(ctx), ascii::to_lower_copy(*ctx.pos));
            }
        }

        ++ctx.pos;
    }


} // namespace webpp::uri::details

#endif // URI_COMPONENTS_ENCODING_HPP
