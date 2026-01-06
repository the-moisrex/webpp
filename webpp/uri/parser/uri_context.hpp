// Created by moisrex on 11/12/23.

#ifndef URI_COMPONENTS_ENCODING_HPP
#define URI_COMPONENTS_ENCODING_HPP

#include "../../std/string_like.hpp"
#include "../../strings/append.hpp"
#include "../../strings/to_case.hpp"
#include "../encoding.hpp"
#include "./uri_components.hpp"

namespace webpp::uri {

    /**
     * URI Context is everything we need during parsing of a URL
     */
    template <typename T>
    concept URIContext = requires(T ctx) {
        typename T::iterator;
        typename T::out_type;
        typename T::out_seg_type;
        typename T::base_seg_type;
        typename T::base_type;
        typename T::char_type;
        typename T::vec_iterator;

        T::is_nothrow;
        T::is_modifiable;
        T::is_segregated;

        ctx.pos;
        ctx.beg;
        ctx.end;
        ctx.base;
        ctx.status;
    };

    /**
     * A class used during parsing a URI
     */
    template <URIComponents CompType, URIComponents BaseType = CompType>
    struct uri_context {
        using component_type = CompType;
        using base_type      = BaseType;
        using seg_type       = typename component_type::seg_type;
        using iterator       = typename component_type::iterator;
        using char_type      = stl::iter_value_t<iterator>;

        iterator                        beg{}; // the beginning of the string, not going to change during parsing
        iterator                        pos{}; // current position
        iterator                        end{}; // the end of the string
        component_type                  out{}; // the output uri components
        [[no_unique_address]] base_type base{};
        uri_status_type                 status = +uri_status::unparsed;
    };

} // namespace webpp::uri

namespace webpp::uri::details {

    template <URIContext CtxT>
    using diff_type_of = stl::iter_difference_t<typename CtxT::iterator>;


    /// if it's segregated:
    ///   if it's modifiable path, vector::iterator
    /// else if modifiable, a simple string:
    /// otherwise, nothing_type
    template <typename T, typename CtxT>
    concept CtxBufferOf = URIContext<CtxT> && (requires {
                              typename CtxT::vec_iterator;
                              requires istl::cvref_as<T, typename CtxT::vec_iterator>;
                          } || istl::cvref_as<T, istl::nothing_type> || istl::StringLike<T>);

    template <typename T, typename CtxT>
    concept CtxModifiableBuffer = CtxBufferOf<T, CtxT> && CtxT::is_modifiable && istl::String<T>;

    template <typename T, typename CtxT>
    concept CtxModifiableStringOutput = URIContext<CtxT> && CtxT::is_modifiable && istl::String<T>;

    template <typename T, typename CtxT>
    concept CtxMappedBuffer = CtxBufferOf<T, CtxT> && istl::cvref_as<T, typename CtxT::map_value_type>;

    template <typename T, typename CtxT>
    concept CtxNonModifiableBuffer =
      CtxBufferOf<T, CtxT> && !istl::cvref_as<T, istl::nothing_type> && !CtxT::is_modifiable;

    /// call this when encoding/decoding is done
    template <components Comp, URIContext CtxT>
    static constexpr void set_component_value(CtxT& ctx, typename CtxT::iterator beg, typename CtxT::iterator end)
      noexcept(CtxT::is_nothrow) {
        webpp_static_constexpr bool is_vec = CtxT::is_segregated && components::path == Comp;
        webpp_static_constexpr bool is_map = CtxT::is_segregated && components::queries == Comp;
        webpp_static_constexpr bool is_seg = is_vec || is_map;
        if constexpr (!is_seg && !CtxT::is_modifiable) {
            uri::set_value<Comp>(ctx, beg, end);
        }
    }

    template <components Comp, URIContext CtxT>
    static constexpr void set_component_value(CtxT& ctx, typename CtxT::iterator beg) noexcept(CtxT::is_nothrow) {
        set_component_value<Comp>(ctx, beg, ctx.pos);
    }

    template <URIContext CtxT, CtxBufferOf<CtxT> BufT>
    [[nodiscard]] static constexpr bool encode_or_validate(
      [[maybe_unused]] CtxT&   ctx,
      BufT&                    buffer,
      typename CtxT::iterator& pos,
      typename CtxT::iterator  end,
      CharSet auto const&      policy_chars,
      CharSet auto const&      invalid_chars) noexcept(CtxT::is_nothrow) {
        if constexpr (CtxModifiableBuffer<BufT, CtxT>) {
            return encode_uri_component<uri_encoding_policy::encode_chars>(
              pos,
              end,
              buffer,
              policy_chars,
              invalid_chars);
        } else {
            pos = invalid_chars.find_first_in(pos, end);
            return pos == end;
        }
    }

    template <URIContext CtxT, CtxBufferOf<CtxT> BufT>
    [[nodiscard]] static constexpr bool encode_or_validate(
      [[maybe_unused]] CtxT&   ctx,
      BufT&                    buffer,
      typename CtxT::iterator& pos,
      typename CtxT::iterator  end,
      CharSet auto const&      policy_chars) noexcept(CtxT::is_nothrow) {
        if constexpr (CtxModifiableBuffer<BufT, CtxT>) {
            encode_uri_component<uri_encoding_policy::encode_chars>(pos, end, buffer, policy_chars);
            return pos == end;
        } else {
            pos = policy_chars.find_first_in(pos, end);
            return pos == end;
        }
    }

    /**
     * @brief Encode if the context is modifiable, otherwise just validate the invalid characters
     * @param ctx parsing context
     * @param buffer
     * @param policy_chars encode these characters if encoding is possible
     * @param invalid_chars invalid character or allowed characters depending on the policy
     * @returns successful until the end (== didn't find any invalid chars)
     */
    template <URIContext CtxT>
    [[nodiscard]] static constexpr bool encode_or_validate(
      CtxT&                   ctx,
      CtxBufferOf<CtxT> auto& buffer,
      CharSet auto const&     policy_chars,
      CharSet auto const&     invalid_chars) noexcept(CtxT::is_nothrow) {
        return encode_or_validate(ctx, buffer, ctx.pos, ctx.end, policy_chars, invalid_chars);
    }

    template <URIContext CtxT>
    [[nodiscard]] static constexpr bool
    encode_or_validate(CtxT& ctx, CtxBufferOf<CtxT> auto& buffer, CharSet auto const& policy_chars)
      noexcept(CtxT::is_nothrow) {
        return encode_or_validate(ctx, buffer, ctx.pos, ctx.end, policy_chars);
    }

    template <URIContext CtxT, CtxBufferOf<CtxT> BufT>
    [[nodiscard]] static constexpr bool encode_or_validate_map(
      CtxT&                                ctx,
      [[maybe_unused]] CharSet auto const& policy_chars,
      CharSet auto const&                  invalid_chars,
      BufT&                                buffer) noexcept(CtxT::is_nothrow) {
        if constexpr (CtxModifiableStringOutput<BufT, CtxT>) {
            return encode_uri_component<uri_encoding_policy::encode_chars>(
              ctx.pos,
              ctx.end,
              buffer,
              policy_chars,
              invalid_chars);
        } else {
            return encode_or_validate(ctx, buffer, policy_chars, invalid_chars);
        }
    }

    /**
     * @brief Decode if the context is modifiable, otherwise just validate the invalid characters
     * @param ctx context
     * @param buffer
     * @param policy_chars invalid character or allowed characters depending on the policy
     * @returns successful until the end (== didn't find any invalid chars)
     */
    template <URIContext CtxT, CtxBufferOf<CtxT> BufT>
    [[nodiscard]] static constexpr bool decode_or_validate(CtxT& ctx, BufT& buffer, CharSet auto const& policy_chars)
      noexcept(CtxT::is_nothrow) {
        if constexpr (CtxModifiableBuffer<BufT, CtxT>) {
            return decode_uri_component<uri_encoding_policy::encode_chars>(ctx.pos, ctx.end, buffer, policy_chars);
        } else {
            ctx.pos = policy_chars.find_first_in(ctx.pos, ctx.end);
            return ctx.pos == ctx.end;
        }
    }

    /// Convert to lowercase and also decode
    /// @returns true when we reach the end
    template <URIContext CtxT, CtxBufferOf<CtxT> BufT>
    [[nodiscard]] static constexpr bool decode_or_tolower(CtxT& ctx, BufT& buffer, CharSet auto const& policy_chars)
      noexcept(CtxT::is_nothrow) {
        using char_type = typename CtxT::char_type;
        if constexpr (CtxModifiableBuffer<BufT, CtxT>) {
            while (ctx.pos != ctx.end) {
                if (decode_uri_component<uri_encoding_policy::encode_chars>(ctx.pos, ctx.end, buffer, policy_chars)) {
                    return true;
                }
                webpp_static_constexpr char_type diff = 'a' - 'A';
                if (*ctx.pos >= 'A' && *ctx.pos <= 'Z') {
                    buffer += *ctx.pos + diff;
                    ++ctx.pos;
                    continue;
                }

                [[unlikely]] { return false; }
            }
            return true;
        } else {
            return decode_or_validate(ctx, buffer, policy_chars);
        }
    }

    /// Set the beginning to current position
    template <URIContext CtxT>
    static constexpr void reset_begin(CtxT& ctx, typename CtxT::iterator& beg) noexcept {
        beg = ctx.pos;
    }

    template <URIContext CtxT>
    [[nodiscard]] static constexpr bool is_segment_empty(CtxT& ctx, typename CtxT::iterator beg) noexcept {
        return beg == ctx.pos;
    }

    template <URIContext CtxT>
    static constexpr void reset_segment_start(CtxT ctx, typename CtxT::iterator& beg) noexcept {
        if constexpr (CtxT::is_modifiable) {
            reset_begin(ctx, beg);
        }
    }

    template <URIContext CtxT, ParsingOutput OutT>
    static constexpr void skip_separator(CtxT& ctx, OutT& out, diff_type_of<CtxT> count) noexcept {
        if constexpr (CtxModifiableStringOutput<OutT, CtxT>) {
            for (; count != 0; --count) {
                append_to(out, *ctx.pos++);
            }
        } else {
            ctx.pos += count;
        }
    }

    /// Parsing path requires this so we can make sure the modifiable strings' separator is always '/' and
    /// not '\\' if the input contains that separator
    template <URIContext CtxT, ParsingOutput OutT>
    static constexpr void
    skip_separator(CtxT& ctx, OutT& out, typename CtxT::char_type separator, diff_type_of<CtxT> count = 1)
      noexcept(CtxT::is_nothrow) {
        if constexpr (CtxModifiableStringOutput<OutT, CtxT>) {
            append_to(out, separator);
            ctx.pos += count;
        } else {
            return skip_separator(ctx, out, count);
        }
    }

    template <URIContext CtxT, ParsingOutput OutT>
    static constexpr void skip_separator(CtxT& ctx, OutT& out) noexcept(CtxT::is_nothrow) {
        if constexpr (CtxModifiableStringOutput<OutT, CtxT>) {
            append_to(out, *ctx.pos++);
        } else {
            ++ctx.pos;
        }
    }

    template <URIContext CtxT>
    static constexpr void ignore_character(CtxT& ctx, diff_type_of<CtxT> count = 1) noexcept {
        ctx.pos += count;
    }

    template <URIContext CtxT, CtxBufferOf<CtxT> BufT>
    static constexpr void append_n(CtxT& ctx, BufT& buffer, diff_type_of<CtxT> count) noexcept {
        if constexpr (CtxModifiableStringOutput<BufT, CtxT>) {
            for (; count != 0; --count) {
                append_to(buffer, *ctx.pos++);
            }
        } else {
            ctx.pos += count;
        }
    }

    template <URIContext CtxT, CtxBufferOf<CtxT> BufT>
    static constexpr void append([[maybe_unused]] CtxT& ctx, BufT& buffer, typename CtxT::char_type inp_char) noexcept {
        if constexpr (CtxModifiableStringOutput<BufT, CtxT>) {
            append_to(buffer, inp_char);
        }
    }

    template <URIContext CtxT, CtxBufferOf<CtxT> BufT>
    constexpr void append_inplace_of(
      CtxT&                    ctx,
      BufT&                    buffer,
      typename CtxT::char_type inp_char,
      diff_type_of<CtxT>       count = 1) noexcept {
        if constexpr (CtxModifiableStringOutput<BufT, CtxT>) {
            append_to(buffer, inp_char);
        }
        ctx.pos += count;
    }

    /// Check if the next 2 characters are valid percent encoded ascii-hex digits.
    template <URIContext CtxT, typename OutT>
        requires(ParsingOutput<OutT> || CtxBufferOf<OutT, CtxT>)
    [[nodiscard]] constexpr bool validate_percent_encode(CtxT& ctx, OutT& out) noexcept {
        using ascii::is_hex_digit;

        // NOLINTBEGIN(*-inc-dec-in-conditions)
        auto       cur      = ctx.pos;
        bool const is_valid = cur++ + 2 <= ctx.end && is_hex_digit(*cur++) && is_hex_digit(*cur);
        append_n(ctx, out, cur - ctx.pos);
        return is_valid;
        // NOLINTEND(*-inc-dec-in-conditions)
    }


} // namespace webpp::uri::details

#endif // URI_COMPONENTS_ENCODING_HPP
