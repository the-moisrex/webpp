// Created by moisrex on 11/12/23.

#ifndef URI_COMPONENTS_ENCODING_HPP
#define URI_COMPONENTS_ENCODING_HPP

#include "../encoding.hpp"
#include "./uri_components.hpp"

namespace webpp::uri::details {

    /**
     * @brief Encode if the context is modifiable, otherwise just validate the invalid characters
     * @tparam Policy
     * @param ctx the context
     * @param policy_chars encode these characters if encoding is possible
     * @param invalid_chars invalid character or allowed characters depending on the policy
     * @param output the string output for the encoded chars
     * @returns successful until the end (== didn't find any invalid chars)
     */
    template <uri_encoding_policy Policy = uri_encoding_policy::skip_chars, typename... T>
    [[nodiscard]] static constexpr bool
    encode_or_validate(parsing_uri_context<T...>&           ctx,
                       CharSet auto const&                  policy_chars,
                       [[maybe_unused]] CharSet auto const& invalid_chars,
                       [[maybe_unused]] auto& output) noexcept(parsing_uri_context<T...>::is_nothrow) {
        using ctx_type    = parsing_uri_context<T...>;
        using iterator    = typename ctx_type::iterator;
        using output_type = stl::remove_cvref_t<decltype(output)>;
        using output_iter = typename output_type::iterator;

        if constexpr (ctx_type::is_segregated && ctx_type::is_modifiable &&
                      !stl::same_as<output_iter, iterator>) {
            istl::collection::emplace_one(output, output.get_allocator());
            return encode_uri_component<Policy>(ctx.pos, ctx.end, output.back(), policy_chars, invalid_chars);
        } else if constexpr (ctx_type::is_modifiable) {
            // encode_uri_component_set_capacity(ctx.pos, ctx.end, ctx.out.host_ref());
            return encode_uri_component<Policy>(ctx.pos, ctx.end, output, policy_chars, invalid_chars);
        } else {
            if constexpr (Policy == uri_encoding_policy::skip_chars) {
                ctx.pos = invalid_chars.find_first_not_in(ctx.pos, ctx.end);
            } else {
                ctx.pos = invalid_chars.find_first_in(ctx.pos, ctx.end);
            }
            return ctx.pos == ctx.end;
        }
    }

    /**
     * @brief Decode if the context is modifiable, otherwise just validate the invalid characters
     * @tparam Policy
     * @param ctx the context
     * @param policy_chars invalid character or allowed characters depending on the policy
     * @param output the string output for the decoded chars
     * @returns successfull until the end (== didn't find any invalid chars)
     */
    template <uri_encoding_policy Policy = uri_encoding_policy::skip_chars, typename... T>
    [[nodiscard]] static constexpr bool
    decode_or_validate(parsing_uri_context<T...>& ctx,
                       CharSet auto const&        policy_chars,
                       [[maybe_unused]] auto&     output) noexcept(parsing_uri_context<T...>::is_nothrow) {
        using ctx_type    = parsing_uri_context<T...>;
        using iterator    = typename ctx_type::iterator;
        using output_type = stl::remove_cvref_t<decltype(output)>;
        using output_iter = typename output_type::iterator;

        if constexpr (ctx_type::is_segregated && ctx_type::is_modifiable &&
                      !stl::same_as<output_iter, iterator>) {
            istl::collection::emplace_one(output, output.get_allocator());
            return decode_uri_component<Policy>(ctx.pos, ctx.end, output.back(), policy_chars);
        } else if constexpr (ctx_type::is_modifiable) {
            // decode_uri_component_set_capacity(ctx.pos, ctx.end, ctx.out.host_ref());
            return decode_uri_component<Policy>(ctx.pos, ctx.end, output, policy_chars);
        } else {
            if constexpr (Policy == uri_encoding_policy::skip_chars) {
                ctx.pos = policy_chars.find_first_not_in(ctx.pos, ctx.end);
            } else {
                ctx.pos = policy_chars.find_first_in(ctx.pos, ctx.end);
            }
            return ctx.pos == ctx.end;
        }
    }

} // namespace webpp::uri::details

#endif // URI_COMPONENTS_ENCODING_HPP
