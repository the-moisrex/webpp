// Created by moisrex on 11/12/23.

#ifndef URI_COMPONENTS_ENCODING_HPP
#define URI_COMPONENTS_ENCODING_HPP

#include "../../std/string.hpp"
#include "../encoding.hpp"
#include "./uri_components.hpp"

#include <type_traits>

namespace webpp::uri {

    /**
     * URI Context is everything we need during parsing of a URL
     */
    template <typename T, typename U = stl::remove_cvref_t<T>>
    concept URIContext = requires(U ctx) {
        typename U::iterator;
        typename U::char_type;

        // Component type
        requires URIComponents<typename U::component_type>;

        // Base type (that component type would inherit from)
        requires URIComponents<typename U::base_type> || stl::is_void_v<typename U::base_type>;

        U::is_nothrow;
        U::is_modifiable;
        U::is_segregated;

        { ctx.beg } -> stl::convertible_to<typename U::iterator>;
        { ctx.pos } -> stl::convertible_to<typename U::iterator>;
        { ctx.end } -> stl::convertible_to<typename U::iterator>;
        { ctx.out } -> stl::convertible_to<typename U::component_type>;
        ctx.status;
        requires requires {
            { ctx.base } -> stl::convertible_to<typename U::base_type>;
        } || requires {
            { ctx.base } -> stl::convertible_to<istl::nothing_type>;
        };

        // Compatibility Check: If base type is modifiable, then component type must be modifiable as well.
        requires(URIModifiableComponents<typename U::component_type> &&
                 URIModifiableComponents<typename U::base_type>) ||
                  (!URIModifiableComponents<typename U::base_type>);
    };

    /**
     * A class used during parsing a URI
     */
    template <URIComponents CompType, typename BaseType = void>
        requires(URIComponents<BaseType> || stl::same_as<BaseType, void>)
    struct uri_context {
        using component_type = CompType;
        using base_type      = BaseType;
        using seg_type       = typename component_type::seg_type;
        using char_type      = typename component_type::char_type;
        using iterator       = char_type const*;
        using allocator_type = allocator_type_of<component_type>;

        static constexpr bool is_nothrow    = component_type::is_nothrow;
        static constexpr bool is_modifiable = component_type::is_modifiable;
        static constexpr bool is_segregated = component_type::is_segregated;

        iterator        beg{}; // the beginning of the string, not going to change during parsing
        iterator        pos{}; // current position
        iterator        end{}; // the end of the string
        component_type  out{}; // the output uri components
        base_type       base{};
        uri_status_type status = +uri_status::unparsed;
    };

    /**
     * A class used during parsing a URI
     */
    template <URIComponents CompType>
    struct uri_context<CompType, void> {
        using component_type = CompType;
        using base_type      = void;
        using seg_type       = typename component_type::seg_type;
        using char_type      = typename component_type::char_type;
        using iterator       = char_type const*;
        using allocator_type = allocator_type_of<component_type>;

        static constexpr bool is_nothrow    = component_type::is_nothrow;
        static constexpr bool is_modifiable = component_type::is_modifiable;
        static constexpr bool is_segregated = component_type::is_segregated;

        iterator       beg{}; // the beginning of the string, not going to change during parsing
        iterator       pos{}; // current position
        iterator       end{}; // the end of the string
        component_type out{}; // the output uri components
        [[no_unique_address]] istl::nothing_type base   = istl::nothing;
        uri_status_type                          status = +uri_status::unparsed;
    };

    template <typename CompType, typename BaseType>
    [[nodiscard]] static constexpr decltype(auto) get_allocator(uri_context<CompType, BaseType> const& ctx) noexcept {
        if constexpr (URIModifiableComponents<CompType>) {
            return allocator_from(ctx.out);
        } else if constexpr (URIModifiableComponents<BaseType>) {
            return allocator_from(ctx.base);
        } else {
            static_assert_false(CompType, "No allocator available");
        }
    }

    /// Create a URI Context, and initialize it properly
    template <URIContext CtxT>
    static constexpr CtxT
    create(typename CtxT::iterator beg, typename CtxT::iterator end, typename CtxT::allocator_type alloc = {})
      noexcept(CtxT::is_nothrow) {
        CtxT ctx{
          .beg    = beg,
          .pos    = beg,
          .end    = end,
          .out    = create(stl::type_identity<typename CtxT::component_type>{}, beg, end, alloc),
          .status = +uri_status::unparsed,
        };
        return ctx;
    }

    /// Create a URI Context, and initialize it properly
    template <URIContext CtxT>
    static constexpr CtxT
    create(typename CtxT::iterator            beg,
           typename CtxT::iterator            end,
           typename CtxT::component_type&& out) noexcept(CtxT::is_nothrow) {
        CtxT ctx{
          .beg    = beg,
          .pos    = beg,
          .end    = end,
          .out    = stl::move(out),
          .status = +uri_status::unparsed,
        };
        return ctx;
    }

    /// Create a URI Context, and initialize it properly
    template <URIContext CtxT>
        requires(!stl::is_void_v<typename CtxT::base_type>)
    static constexpr CtxT create(
      typename CtxT::iterator       beg,
      typename CtxT::iterator       end,
      typename CtxT::base_type&&    base_ctx,
      typename CtxT::allocator_type alloc = {}) noexcept(CtxT::is_nothrow) {
        CtxT ctx{
          .beg    = beg,
          .pos    = beg,
          .end    = end,
          .out    = create(stl::type_identity<typename CtxT::component_type>{}, beg, end, alloc),
          .base   = stl::move(base_ctx),
          .status = +uri_status::unparsed,
        };
        return ctx;
    }

    /// Create a new buffer/segment
    template <URIContext CtxT>
        requires(CtxT::is_modifiable)
    [[nodiscard]] static constexpr auto create_buffer(CtxT& ctx) noexcept(CtxT::is_nothrow) {
        using seg_type = typename CtxT::seg_type;
        return seg_type{get_allocator(ctx.out)};
    }

    template <URIContext CtxT>
        requires(CtxT::is_modifiable)
    [[nodiscard]] static constexpr auto
    create_buffer(CtxT& ctx, typename CtxT::iterator beg, typename CtxT::iterator end) noexcept(CtxT::is_nothrow) {
        using seg_type = typename CtxT::seg_type;
        return seg_type{beg, end, get_allocator(ctx.out)};
    }

    /// Create a new buffer (which can be used as a segment)
    template <URIContext CtxT>
    static constexpr segment<typename CtxT::iterator> create_buffer([[maybe_unused]] CtxT& ctx) noexcept {
        return {.beg = ctx.pos, .end = ctx.pos};
    }

    template <URIContext CtxT>
    static constexpr segment<typename CtxT::iterator>
    create_buffer([[maybe_unused]] CtxT& ctx, typename CtxT::iterator beg, typename CtxT::iterator end) noexcept {
        return {.beg = beg, .end = end};
    }

    /// Mark the end of the current segment
    template <URIContext CtxT>
    static constexpr void end_segment([[maybe_unused]] CtxT const&        ctx,
                                      [[maybe_unused]] istl::String auto& seg) noexcept {
        // Do nothing
    }

    /// Mark the end of the current segment
    template <URIContext CtxT>
    static constexpr void end_segment(CtxT const& ctx, segment<typename CtxT::iterator>& seg) noexcept {
        seg.end = ctx.pos;
    }

    /// Empty out the current buffer
    template <URIContext CtxT, typename BufT>
    static constexpr void clear_segment(CtxT& ctx, BufT& buffer) noexcept {
        if constexpr (istl::String<BufT>) {
            buffer.clear();
        } else {
            // Don't set `pos` to be `beg`, set the `beg` to be `pos`
            buffer.beg = buffer.end = ctx.pos;
        }
    }

    /// Push back the buffer to the specified component
    template <VectorOutput CompT>
    static constexpr void push_segment(CompT& component, typename CompT::value_type&& buffer) noexcept(false) {
        component.emplace_back(stl::move(buffer));
    }

    /// For queries
    template <VectorOutput CompT, typename... Args>
    static constexpr void push_segment(CompT& component, Args&&... args) noexcept(false) {
        component.emplace_back(stl::forward<Args>(args)...);
    }

    /// For queries
    template <VectorOutput CompT, typename Iter>
    static constexpr void
    push_segment(CompT& component, segment<Iter> const& key_buffer, segment<Iter> const& value_buffer) noexcept(false) {
        using pair_type   = typename CompT::value_type;
        using first_type  = typename pair_type::first_type;
        using second_type = typename pair_type::second_type;
        component.emplace_back(first_type{key_buffer.beg, key_buffer.end},
                               second_type{value_buffer.beg, value_buffer.end});
    }

    template <VectorOutput CompT, typename Iter>
    static constexpr void push_segment(CompT& component, segment<Iter> const& buffer) noexcept(false) {
        component.emplace_back(buffer.beg, buffer.end);
    }

    // template <istl::String CompT, istl::String BufT>
    //     requires(stl::is_rvalue_reference_v<BufT>)
    // static constexpr void push_segment(CompT& component, BufT&& buffer) noexcept(false) {
    //     component.append(stl::forward<BufT>(buffer));
    // }
    //
    // template <istl::String CompT, typename Iter>
    // static constexpr void push_segment(CompT& component, segment<Iter> const& buffer) noexcept(false) {
    //     component.append(buffer.beg, buffer.end);
    // }
    //
    // /// String View Components are not modifiable
    // template <typename CompT, typename Iter>
    // static constexpr void push_segment([[maybe_unused]] CompT&               component,
    //                                    [[maybe_unused]] segment<Iter> const& buffer) noexcept {
    //     // nothing to do
    // }
} // namespace webpp::uri

namespace webpp::uri::details {

    template <URIContext CtxT>
    using diff_type_of = stl::iter_difference_t<typename CtxT::iterator>;

    template <URIContext CtxT, typename BufT>
    [[nodiscard]] static constexpr bool encode_or_validate(
      [[maybe_unused]] CtxT&   ctx,
      BufT&                    buffer,
      typename CtxT::iterator& pos,
      typename CtxT::iterator  end,
      CharSet auto const&      policy_chars,
      CharSet auto const&      invalid_chars) noexcept(CtxT::is_nothrow) {
        if constexpr (istl::String<BufT>) {
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

    template <URIContext CtxT, typename BufT>
    [[nodiscard]] static constexpr bool encode_or_validate(
      [[maybe_unused]] CtxT&   ctx,
      BufT&                    buffer,
      typename CtxT::iterator& pos,
      typename CtxT::iterator  end,
      CharSet auto const&      policy_chars) noexcept(CtxT::is_nothrow) {
        if constexpr (istl::String<BufT>) {
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
      CtxT&               ctx,
      istl::String auto&  buffer,
      CharSet auto const& policy_chars,
      CharSet auto const& invalid_chars) noexcept(CtxT::is_nothrow) {
        return encode_or_validate(ctx, buffer, ctx.pos, ctx.end, policy_chars, invalid_chars);
    }

    template <URIContext CtxT>
    [[nodiscard]] static constexpr bool
    encode_or_validate(CtxT& ctx, istl::String auto& buffer, CharSet auto const& policy_chars)
      noexcept(CtxT::is_nothrow) {
        return encode_or_validate(ctx, buffer, ctx.pos, ctx.end, policy_chars);
    }

    /**
     * @brief Decode if the context is modifiable, otherwise just validate the invalid characters
     * @param ctx context
     * @param buffer
     * @param policy_chars invalid character or allowed characters depending on the policy
     * @returns successful until the end (== didn't find any invalid chars)
     */
    template <URIContext CtxT, typename BufT>
    [[nodiscard]] static constexpr bool decode_or_validate(CtxT& ctx, BufT& buffer, CharSet auto const& policy_chars)
      noexcept(CtxT::is_nothrow) {
        if constexpr (istl::String<BufT>) {
            return decode_uri_component<uri_encoding_policy::encode_chars>(ctx.pos, ctx.end, buffer, policy_chars);
        } else {
            ctx.pos = policy_chars.find_first_in(ctx.pos, ctx.end);
            return ctx.pos == ctx.end;
        }
    }

    /// Convert to lowercase and also decode
    /// @returns true when we reach the end
    template <URIContext CtxT, typename BufT>
    [[nodiscard]] static constexpr bool decode_or_tolower(CtxT& ctx, BufT& buffer, CharSet auto const& policy_chars)
      noexcept(CtxT::is_nothrow) {
        using char_type = typename CtxT::char_type;
        if constexpr (istl::String<BufT>) {
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

    template <URIContext CtxT, typename OutT>
    static constexpr void skip_separator(CtxT& ctx, OutT& out, diff_type_of<CtxT> count) noexcept {
        if constexpr (istl::String<OutT>) {
            for (; count != 0; --count) {
                out.append(*ctx.pos++);
            }
        } else {
            ctx.pos += count;
        }
    }

    /// Parsing path requires this so we can make sure the modifiable strings' separator is always '/' and
    /// not '\\' if the input contains that separator
    template <URIContext CtxT, typename OutT>
    static constexpr void
    skip_separator(CtxT& ctx, OutT& out, typename CtxT::char_type separator, diff_type_of<CtxT> count = 1)
      noexcept(CtxT::is_nothrow) {
        if constexpr (istl::String<OutT>) {
            out.push_back(separator);
            ctx.pos += count;
        } else {
            return skip_separator(ctx, out, count);
        }
    }

    template <URIContext CtxT, typename OutT>
    static constexpr void skip_separator(CtxT& ctx, OutT& out) noexcept(CtxT::is_nothrow) {
        if constexpr (istl::String<OutT>) {
            out.push_back(*ctx.pos++);
        } else {
            ++ctx.pos;
        }
    }

    template <URIContext CtxT, typename BufT>
    static constexpr void append_n(CtxT& ctx, BufT& buffer, diff_type_of<CtxT> count) noexcept(CtxT::is_nothrow) {
        if constexpr (istl::String<BufT>) {
            for (; count != 0; --count) {
                buffer.push_back(*ctx.pos++);
            }
        } else {
            ctx.pos += count;
        }
    }

    template <typename BufT, typename CharT>
    static constexpr void append(BufT& buffer, CharT const inp_char) noexcept(false) {
        if constexpr (istl::String<BufT>) {
            buffer.push_back(inp_char);
        }
    }

    template <URIContext CtxT, typename BufT>
    constexpr void
    append_inplace_of(CtxT& ctx, BufT& buffer, typename CtxT::char_type inp_char, diff_type_of<CtxT> count = 1)
      noexcept(CtxT::is_nothrow) {
        if constexpr (istl::String<BufT>) {
            buffer.push_back(inp_char);
        }
        ctx.pos += count;
    }

    /// Check if the next 2 characters are valid percent encoded ascii-hex digits.
    template <URIContext CtxT, istl::String OutT>
    [[nodiscard]] constexpr bool validate_percent_encode(CtxT& ctx, OutT& out) noexcept(CtxT::is_nothrow) {
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
