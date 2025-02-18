// Created by moisrex on 11/12/23.

#ifndef URI_COMPONENTS_ENCODING_HPP
#define URI_COMPONENTS_ENCODING_HPP

#include "../../std/string_like.hpp"
#include "../../strings/append.hpp"
#include "../../strings/to_case.hpp"
#include "../encoding.hpp"
#include "./uri_components.hpp"
#include "build-release/_deps/fmt-src/include/fmt/base.h"

#include <boost/asio/buffer.hpp>

namespace webpp::uri::details {

    template <ParsingURIContext CtxT>
    using diff_type_of = typename stl::iterator_traits<typename CtxT::iterator>::difference_type;


    /// if it's segregated:
    ///   if it's modifiable queries, map::value_type (pair<string, string>),
    ///   if it's modifiable path/host, vector::iterator
    /// else if it's not segregated but still modifiable:
    ///   vec_iterator which is seg_type*
    /// otherwise, nothing_type
    template <typename T, typename CtxT>
    concept CtxBufferOf =
      ParsingURIContext<CtxT> &&
      (istl::part_of<T,
                     typename CtxT::map_value_type,
                     typename CtxT::vec_iterator,
                     typename CtxT::iterator,
                     istl::nothing_type> ||
       istl::String<T>);


    template <typename T, typename CtxT>
    concept CtxModifiableBuffer =
      CtxBufferOf<T, CtxT> && CtxT::is_modifiable && !stl::same_as<T, istl::nothing_type>;

    template <typename T, typename CtxT>
    concept CtxMappedBuffer = CtxBufferOf<T, CtxT> && stl::same_as<T, typename CtxT::map_value_type>;

    template <typename T, typename CtxT>
    concept CtxVectorBuffer = CtxBufferOf<T, CtxT> && stl::same_as<T, typename CtxT::vec_iterator>;

    /// call this when encoding/decoding is done
    template <components Comp, ParsingURIContext CtxT>
    static constexpr void
    set_component_value(CtxT& ctx, typename CtxT::iterator beg, typename CtxT::iterator end)
      noexcept(CtxT::is_nothrow) {
        webpp_static_constexpr bool is_vec = CtxT::is_segregated && components::path == Comp;
        webpp_static_constexpr bool is_map = CtxT::is_segregated && components::queries == Comp;
        webpp_static_constexpr bool is_seg = is_vec || is_map;
        if constexpr (!is_seg && !CtxT::is_modifiable) {
            uri::set_value<Comp>(ctx, beg, end);
        }
    }

    template <components Comp, ParsingURIContext CtxT>
    static constexpr void set_component_value(CtxT& ctx, typename CtxT::iterator beg)
      noexcept(CtxT::is_nothrow) {
        set_component_value<Comp>(ctx, beg, ctx.pos);
    }

    template <uri_encoding_policy Policy = uri_encoding_policy::skip_chars,
              ParsingURIContext   CtxT,
              CtxBufferOf<CtxT>   BufT>
    [[nodiscard]] static constexpr bool encode_or_validate(
      [[maybe_unused]] CtxT&   ctx,
      BufT&                    buffer,
      typename CtxT::iterator& pos,
      typename CtxT::iterator  end,
      CharSet auto const&      policy_chars,
      CharSet auto const&      invalid_chars) noexcept(CtxT::is_nothrow) {
        if constexpr (CtxModifiableBuffer<BufT, CtxT>) {
            return encode_uri_component<Policy>(pos, end, buffer, policy_chars, invalid_chars);
        } else {
            if constexpr (Policy == uri_encoding_policy::skip_chars) {
                pos = invalid_chars.find_first_not_in(pos, end);
            } else {
                pos = invalid_chars.find_first_in(pos, end);
            }
            return pos == end;
        }
    }

    template <uri_encoding_policy Policy = uri_encoding_policy::skip_chars,
              ParsingURIContext   CtxT,
              CtxBufferOf<CtxT>   BufT>
    [[nodiscard]] static constexpr bool encode_or_validate(
      [[maybe_unused]] CtxT&   ctx,
      BufT&                    buffer,
      typename CtxT::iterator& pos,
      typename CtxT::iterator  end,
      CharSet auto const&      policy_chars) noexcept(CtxT::is_nothrow) {
        if constexpr (CtxModifiableBuffer<BufT, CtxT>) {
            encode_uri_component<Policy>(pos, end, buffer, policy_chars);
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
     * @param ctx parsing context
     * @param buffer
     * @param policy_chars encode these characters if encoding is possible
     * @param invalid_chars invalid character or allowed characters depending on the policy
     * @returns successful until the end (== didn't find any invalid chars)
     */
    template <uri_encoding_policy Policy = uri_encoding_policy::skip_chars, ParsingURIContext CtxT>
    [[nodiscard]] static constexpr bool encode_or_validate(
      CtxT&                   ctx,
      CtxBufferOf<CtxT> auto& buffer,
      CharSet auto const&     policy_chars,
      CharSet auto const&     invalid_chars) noexcept(CtxT::is_nothrow) {
        return encode_or_validate<Policy>(ctx, buffer, ctx.pos, ctx.end, policy_chars, invalid_chars);
    }

    template <uri_encoding_policy Policy = uri_encoding_policy::skip_chars, ParsingURIContext CtxT>
    [[nodiscard]] static constexpr bool encode_or_validate(CtxT& ctx, CharSet auto const& policy_chars)
      noexcept(CtxT::is_nothrow) {
        return encode_or_validate<Policy>(ctx, ctx.pos, ctx.end, policy_chars);
    }

    template <uri_encoding_policy Policy = uri_encoding_policy::skip_chars,
              ParsingURIContext   CtxT,
              CtxBufferOf<CtxT>   BufT>
    [[nodiscard]] static constexpr bool encode_or_validate_map(
      CtxT&                                ctx,
      [[maybe_unused]] CharSet auto const& policy_chars,
      CharSet auto const&                  invalid_chars,
      [[maybe_unused]] bool const          in_value,
      BufT&                                buffer) noexcept(CtxT::is_nothrow) {
        if constexpr (CtxT::is_modifiable && CtxMappedBuffer<BufT, CtxT>) {
            return encode_uri_component<Policy>(
              ctx,
              ctx.pos,
              ctx.end,
              !in_value ? buffer.first : buffer.second,
              policy_chars,
              invalid_chars);
        } else {
            return encode_or_validate<Policy>(ctx, policy_chars, invalid_chars);
        }
    }

    /**
     * @brief Decode if the context is modifiable, otherwise just validate the invalid characters
     * @tparam Policy
     * @param ctx context
     * @param buffer
     * @param policy_chars invalid character or allowed characters depending on the policy
     * @returns successful until the end (== didn't find any invalid chars)
     */
    template <uri_encoding_policy Policy = uri_encoding_policy::skip_chars,
              ParsingURIContext   CtxT,
              CtxBufferOf<CtxT>   BufT>
    [[nodiscard]] static constexpr bool
    decode_or_validate(CtxT& ctx, BufT& buffer, CharSet auto const& policy_chars) noexcept(CtxT::is_nothrow) {
        if constexpr (CtxModifiableBuffer<BufT, CtxT>) {
            return decode_uri_component<Policy>(ctx.pos, ctx.end, buffer, policy_chars);
        } else {
            if constexpr (Policy == uri_encoding_policy::skip_chars) {
                ctx.pos = policy_chars.find_first_not_in(ctx.pos, ctx.end);
            } else {
                ctx.pos = policy_chars.find_first_in(ctx.pos, ctx.end);
            }
            return ctx.pos == ctx.end;
        }
    }

    /// Convert to lowercase and also decode
    template <uri_encoding_policy Policy = uri_encoding_policy::skip_chars,
              ParsingURIContext   CtxT,
              CtxBufferOf<CtxT>   BufT>
    [[nodiscard]] static constexpr bool
    decode_or_tolower(CtxT& ctx, BufT& buffer, CharSet auto const& policy_chars) noexcept(CtxT::is_nothrow) {
        using char_type = typename CtxT::char_type;
        if constexpr (CtxModifiableBuffer<BufT, CtxT>) {
            while (ctx.pos != ctx.end) {
                if (decode_uri_component<Policy>(ctx.pos, ctx.end, buffer, policy_chars)) {
                    return true;
                }
                webpp_static_constexpr char_type diff = 'a' - 'A';
                if (*ctx.pos >= 'A' && *ctx.pos <= 'Z') {
                    buffer += *ctx.pos + diff;
                    ++ctx.pos;
                    continue;
                }
                return false;
            }
            return true;
        } else {
            return decode_or_validate<Policy>(ctx, policy_chars);
        }
    }

    /// Set the beginning to current position
    template <ParsingURIContext CtxT>
    static constexpr void reset_begin(CtxT& ctx, typename CtxT::iterator& beg) noexcept {
        beg = ctx.pos;
    }

    template <ParsingURIContext CtxT>
    [[nodiscard]] static constexpr bool is_segment_empty(CtxT& ctx, CtxBufferOf<CtxT> auto beg) noexcept {
        return beg == ctx.pos;
    }

    template <ParsingURIContext CtxT, CtxBufferOf<CtxT> BufT>
    static constexpr void reset_segment_start(CtxT ctx, BufT& beg) noexcept {
        if constexpr (CtxVectorBuffer<BufT, CtxT> || CtxT::is_modifiable) {
            reset_begin(ctx, beg);
        }
    }

    template <ParsingURIContext CtxT, ParsingOutput OutT>
    static constexpr void skip_separator(CtxT& ctx, OutT& out, diff_type_of<CtxT> count) noexcept {
        if constexpr (CtxT::is_modifiable && !SegregatedOutput<OutT>) {
            for (; count != 0; --count) {
                append_to(out, *ctx.pos++);
            }
        } else {
            ctx.pos += count;
        }
    }

    /// Parsing path requires this so we can make sure the modifiable strings' separator is always '/' and
    /// not '\\' if the input contains that separator
    template <ParsingURIContext CtxT, ParsingOutput OutT>
    static constexpr void
    skip_separator(CtxT& ctx, OutT& out, typename CtxT::char_type separator, diff_type_of<CtxT> count = 1)
      noexcept(CtxT::is_nothrow) {
        if constexpr (CtxT::is_modifiable && !SegregatedOutput<OutT>) {
            append_to(out, separator);
            ctx.pos += count;
        } else {
            return skip_separator(ctx, out, count);
        }
    }

    template <ParsingURIContext CtxT, ParsingOutput OutT>
    static constexpr void skip_separator(CtxT& ctx, OutT& out) noexcept(CtxT::is_nothrow) {
        if constexpr (CtxT::is_modifiable && !SegregatedOutput<OutT>) {
            append_to(out, *ctx.pos++);
        } else {
            ++ctx.pos;
        }
    }

    template <ParsingURIContext CtxT>
    static constexpr void ignore_character(CtxT& ctx, diff_type_of<CtxT> count = 1) noexcept {
        ctx.pos += count;
    }

    template <ParsingURIContext CtxT, CtxBufferOf<CtxT> BufT>
    static constexpr void append_n(CtxT& ctx, BufT& buffer, diff_type_of<CtxT> count) noexcept {
        if constexpr (CtxT::is_modifiable && !CtxMappedBuffer<BufT, CtxT>) {
            for (; count != 0; --count) {
                append_to(buffer, *ctx.pos++);
            }
        } else {
            ctx.pos += count;
        }
    }

    template <ParsingURIContext CtxT, CtxBufferOf<CtxT> BufT>
    static constexpr void
    append([[maybe_unused]] CtxT& ctx, BufT& buffer, typename CtxT::char_type inp_char) noexcept {
        if constexpr (CtxT::is_modifiable && !CtxMappedBuffer<BufT, CtxT>) {
            append_to(buffer, inp_char);
        }
    }

    template <ParsingURIContext CtxT, CtxBufferOf<CtxT> BufT>
    constexpr void append_inplace_of(
      CtxT&                    ctx,
      BufT&                    buffer,
      typename CtxT::char_type inp_char,
      diff_type_of<CtxT>       count = 1) noexcept {
        if constexpr (CtxT::is_modifiable && !CtxMappedBuffer<BufT, CtxT>) {
            append_to(buffer, inp_char);
        }
        ctx.pos += count;
    }

    /// Check if the next 2 characters are valid percent encoded ascii-hex digits.
    template <bool CheckNewlinesAndTabs = false, ParsingURIContext CtxT>
    [[nodiscard]] constexpr bool validate_percent_encode(CtxT& ctx, ParsingOutput auto& out) noexcept {
        using ascii::is_hex_digit;

        // NOLINTBEGIN(*-inc-dec-in-conditions)
        if constexpr (!CheckNewlinesAndTabs) {
            auto       cur      = ctx.pos;
            bool const is_valid = cur++ + 2 <= ctx.end && is_hex_digit(*cur++) && is_hex_digit(*cur);
            append_n(ctx, out, cur - ctx.pos);
            return is_valid;
        } else {
            append_n(ctx, out, 1);
            switch (ctx.pos - ctx.end) {
                case 0:
                case 1: return false;
                case 2: return is_hex_digit(*ctx.pos++) && is_hex_digit(*ctx.pos);
                default: {
                    int count = 0;
                    for (;;) {
                        switch (*ctx.pos) {
                            case '\t':
                            case '\r':
                            case '\n':
                                ++ctx.pos;
                                if (ctx.pos == ctx.end) {
                                    return false;
                                }
                                continue;
                            default: {
                                bool const is_valid_char = is_hex_digit(*ctx.pos);
                                append_inplace_of(ctx, out, *ctx.pos, 1);
                                if (!is_valid_char) {
                                    return false;
                                }
                                ++count;
                                if (count == 2 || ctx.pos == ctx.end) {
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

    template <ParsingURIContext CtxT, ParsingOutput OutT>
    static constexpr void pop_back(
      CtxT&                               ctx,
      OutT&                               out,
      CtxBufferOf<CtxT> auto&             buffer,
      typename CtxT::iterator&            beg,
      [[maybe_unused]] diff_type_of<CtxT> hint = 0) noexcept {
        using difference_type = diff_type_of<CtxT>;
        if constexpr (CtxT::is_modifiable && VectorOutput<OutT>) {
            if (out.size() > 2) {
                out.pop_back();
                buffer = out.begin() + static_cast<difference_type>(out.size() - 1);
            } else if (out.size() == 1) {
                buffer->clear();
            }
        } else if constexpr (VectorOutput<OutT>) {
            if (out.size() > 1) {
                out.pop_back();
            } else {
                istl::clear(out.back());
            }
            reset_segment_start(ctx, beg);
        } else if constexpr (CtxT::is_modifiable) {
            using output_t  = stl::remove_cvref_t<decltype(out)>;
            using size_type = typename output_t::size_type;
            if (!out.empty()) {
                out.erase(out.size() - static_cast<size_type>(hint));
            }
        }
    }

    template <ParsingURIContext CtxT, ParsingOutput OutT>
    constexpr void start_segment([[maybe_unused]] CtxT& ctx, OutT& out, CtxBufferOf<CtxT> auto& buffer)
      noexcept(CtxT::is_nothrow || !VectorOutput<OutT>) {
        if constexpr (VectorOutput<OutT> && CtxT::is_modifiable) {
            // the non-modifiable version is the one that needs to be set, the modified versions already
            // contain the right value at this point in time
            istl::emplace_one(out, out.get_allocator());
            buffer = out.begin() + static_cast<diff_type_of<CtxT>>(out.size() - 1);
        }
    }

    /// Call this when you're done with the current segment (e.g.: reaching a dot for host, or a slash
    /// for path)
    template <ParsingURIContext CtxT, ParsingOutput OutT>
    static constexpr void
    end_segment(CtxT& ctx, OutT& out, typename CtxT::iterator inp_beg, typename CtxT::iterator end)
      noexcept(CtxT::is_nothrow || !VectorOutput<OutT>) {
        if constexpr (VectorOutput<OutT> && !CtxT::is_modifiable) {
            // the non-modifiable version is the one that needs to be set, the modified versions already
            // contain the right value at this point in time
            istl::emplace_one(out, inp_beg, end);
            reset_begin(ctx);
        }
    }

    template <ParsingURIContext CtxT, ParsingOutput OutT>
    static constexpr void end_segment(CtxT& ctx, OutT& out, CtxBufferOf<CtxT> auto& beg)
      noexcept(CtxT::is_nothrow || !VectorOutput<OutT>) {
        end_segment(ctx, out, beg, ctx.pos);
    }

    /// 1. Skip the separator, and
    /// 2. Set the segment start
    template <ParsingURIContext CtxT, ParsingOutput OutT>
    static constexpr void
    next_segment(CtxT& ctx, OutT& out, CtxBufferOf<CtxT> auto& beg, diff_type_of<CtxT> sep_count = 1)
      noexcept(CtxT::is_nothrow) {
        if constexpr (SegregatedOutput<OutT>) {
            if constexpr (CtxT::is_modifiable) {
                skip_separator(ctx, out, sep_count);
                reset_segment_start(ctx, beg);
                start_segment(ctx);
            } else {
                end_segment(ctx, out, beg);
                skip_separator(ctx, out, sep_count);
                reset_segment_start(ctx, beg);
            }
        } else {
            skip_separator(ctx, out, sep_count);
            end_segment(ctx, out, beg);
            reset_segment_start(ctx, beg);
        }
    }

    template <ParsingURIContext CtxT, ParsingOutput OutT>
    static constexpr void next_segment_of(
      CtxT&                    ctx,
      OutT&                    out,
      CtxBufferOf<CtxT> auto&  beg,
      typename CtxT::char_type separator,
      diff_type_of<CtxT>       sep_count = 1) noexcept(CtxT::is_nothrow) {
        if constexpr (SegregatedOutput<OutT>) {
            if constexpr (CtxT::is_modifiable) {
                skip_separator(ctx, out, sep_count);
                reset_segment_start(ctx, beg);
                start_segment(ctx);
            } else {
                end_segment(ctx, out, beg);
                skip_separator(ctx, out, sep_count);
                reset_segment_start(ctx, beg);
            }
        } else {
            if constexpr (CtxT::is_modifiable) {
                skip_separator(ctx, out, separator, sep_count);
            } else {
                skip_separator(ctx, out, sep_count);
            }
            end_segment(ctx, out, beg);
            reset_segment_start(ctx, beg);
        }
    }

} // namespace webpp::uri::details

#endif // URI_COMPONENTS_ENCODING_HPP
