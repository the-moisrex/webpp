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
    template <typename T>
    concept URIContext = requires(stl::remove_cvref_t<T> ctx) {
        typename stl::remove_cvref_t<T>::iterator;
        typename stl::remove_cvref_t<T>::char_type;

        // Component type
        requires URIComponents<typename stl::remove_cvref_t<T>::component_type>;

        // Base type (that component type would inherit from)
        typename stl::remove_cvref_t<T>::base_type;
        requires URIComponents<typename stl::remove_cvref_t<T>::base_type> ||
                   stl::is_void_v<typename stl::remove_cvref_t<T>::base_type>;

        stl::remove_cvref_t<T>::is_nothrow;
        stl::remove_cvref_t<T>::is_modifiable;
        stl::remove_cvref_t<T>::is_segregated;

        { ctx.beg } -> stl::convertible_to<typename stl::remove_cvref_t<T>::iterator>;
        { ctx.pos } -> stl::convertible_to<typename stl::remove_cvref_t<T>::iterator>;
        { ctx.end } -> stl::convertible_to<typename stl::remove_cvref_t<T>::iterator>;
        { ctx.out } -> stl::convertible_to<typename stl::remove_cvref_t<T>::component_type>;
        ctx.status;
        requires requires {
            { ctx.base } -> stl::convertible_to<typename stl::remove_cvref_t<T>::base_type>;
        } || requires {
            { ctx.base } -> stl::convertible_to<istl::nothing_type>;
        };

        // Compatibility Check: If base type is modifiable, then component type must be modifiable as well.
        requires(URIModifiableComponents<typename stl::remove_cvref_t<T>::component_type> &&
                 URIModifiableComponents<typename stl::remove_cvref_t<T>::base_type>) ||
                  (!URIModifiableComponents<typename stl::remove_cvref_t<T>::base_type>);
    };

    template <typename T>
    concept URIModifiableContext =
      URIContext<T> && URIModifiableComponents<typename T::component_type> && T::is_modifiable;

    template <typename T>
    concept URIStructuredContext =
      URIContext<T> && URIStructuredComponents<typename T::component_type> && T::is_segregated;

    template <typename T>
    concept URIHrefContext = URIContext<T> && URIHrefComponents<typename T::component_type>;

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
            // return istl::nothing;
        }
    }

    /// Create a URI Context, and initialize it properly
    template <URIModifiableContext CtxT>
    static constexpr CtxT
    create(typename CtxT::iterator beg, typename CtxT::iterator end, allocator_type_of<CtxT> const& inp_alloc = alloc)
      noexcept(CtxT::is_nothrow) {
        CtxT ctx{
          .beg    = beg,
          .pos    = beg,
          .end    = end,
          .out    = create(stl::type_identity<typename CtxT::component_type>{}, beg, end, inp_alloc),
          .status = +uri_status::unparsed,
        };
        return ctx;
    }

    /// Create a URI Context, and initialize it properly
    template <URIContext CtxT>
    static constexpr CtxT
    create(typename CtxT::iterator beg, typename CtxT::iterator end, typename CtxT::component_type&& out)
      noexcept(CtxT::is_nothrow) {
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
    template <URIModifiableContext CtxT>
        requires(!stl::is_void_v<typename CtxT::base_type>)
    static constexpr CtxT create(
      typename CtxT::iterator        beg,
      typename CtxT::iterator        end,
      typename CtxT::base_type&&     base_ctx,
      allocator_type_of<CtxT> const& inp_alloc = alloc) noexcept(CtxT::is_nothrow) {
        CtxT ctx{
          .beg    = beg,
          .pos    = beg,
          .end    = end,
          .out    = create(stl::type_identity<typename CtxT::component_type>{}, beg, end, inp_alloc),
          .base   = stl::move(base_ctx),
          .status = +uri_status::unparsed,
        };
        return ctx;
    }

    /// Create a new buffer/segment
    template <URIModifiableContext CtxT>
    [[nodiscard]] static constexpr auto create_buffer(CtxT& ctx) noexcept(CtxT::is_nothrow) {
        using seg_type = typename CtxT::seg_type;
        return seg_type{get_allocator(ctx.out)};
    }

    template <URIModifiableContext CtxT>
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

    template <URIStructuredContext CtxT>
    static constexpr segment<typename CtxT::component_type::vec_type::const_iterator> create_buffer(
      [[maybe_unused]] CtxT&                                  ctx,
      typename CtxT::component_type::vec_type::const_iterator beg,
      typename CtxT::component_type::vec_type::const_iterator end) noexcept {
        return {.beg = beg, .end = end};
    }

    template <URIStructuredContext CtxT>
    static constexpr segment<typename CtxT::component_type::map_type::const_iterator> create_buffer(
      [[maybe_unused]] CtxT&                                  ctx,
      typename CtxT::component_type::map_type::const_iterator beg,
      typename CtxT::component_type::map_type::const_iterator end) noexcept {
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

    template <istl::String CompT, istl::String BufT>
        requires(stl::is_rvalue_reference_v<BufT>)
    static constexpr void push_segment(CompT& component, BufT&& buffer) noexcept(false) {
        component.append(stl::forward<BufT>(buffer));
    }

    template <istl::String CompT, istl::StringView BufT>
    static constexpr void push_segment(CompT& component, BufT buffer) noexcept(false) {
        component.append(buffer.data(), buffer.size());
    }

    template <istl::String CompT, typename Iter>
    static constexpr void push_segment(CompT& component, segment<Iter> const& buffer) noexcept(false) {
        component.append(buffer.beg, buffer.end);
    }

    template <typename IterT, typename Iter>
    static constexpr void push_segment(segment<IterT>& component, segment<Iter> const& buffer) noexcept(false) {
        assert(component.end == buffer.beg);
        component.end = buffer.end;
    }

    // /// String View Components are not modifiable
    // template <typename CompT, typename Iter>
    // static constexpr void push_segment([[maybe_unused]] CompT&               component,
    //                                    [[maybe_unused]] segment<Iter> const& buffer) noexcept {
    //     // nothing to do
    // }



    template <typename Iter>
    [[nodiscard]] static constexpr bool is_empty(segment<Iter> const& buffer) noexcept {
        return buffer.beg == buffer.end;
    }

    template <typename StrT>
        requires requires(StrT str) { str.empty(); }
    [[nodiscard]] static constexpr bool is_empty(StrT const& buffer) noexcept {
        return buffer.empty();
    }

    // template <typename Iter>
    // [[nodiscard]] static constexpr stl::size_t length(segment<Iter> const& buffer) noexcept {
    //     return buffer.end - buffer.beg;
    // }

    // template <typename StrT>
    //     requires requires(StrT str) { str.size(); }
    // [[nodiscard]] static constexpr stl::size_t length(StrT const& buffer) noexcept {
    //     return buffer.size();
    // }

    template <URIContext CtxT>
    [[nodiscard]] static constexpr char peek(CtxT const& ctx) noexcept {
        return ctx.pos == ctx.end ? '\0' : *ctx.pos;
    }

    [[nodiscard]] static constexpr uri_status nonspecial_state(
      char const code_point,
      uri_status default_state = uri_status::unparsed) noexcept {
        using enum uri_status;
        switch (code_point) {
            case '\0': return valid;
            case '/': return valid_path;
            case ':': return valid_port;
            case '#': return valid_fragment;
            case '?': return valid_queries;
            default: break;
        }
        return default_state;
    }

    [[nodiscard]] static constexpr uri_status special_state(char const code_point,
                                                            uri_status default_state = uri_status::unparsed) noexcept {
        using enum uri_status;
        switch (code_point) {
            case '\0': return valid;
            case '\\':
            case '/': return valid_path;
            case ':': return valid_port;
            case '#': return valid_fragment;
            case '?': return valid_queries;
            default: break;
        }
        return default_state;
    }

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
            using char_type = typename BufT::value_type;
            buffer.push_back(static_cast<char_type>(inp_char));
        }
    }

    /// Check if the next 2 characters are valid percent encoded ascii-hex digits.
    template <URIContext CtxT, typename OutT>
    [[nodiscard]] static constexpr bool next_percent_encode(CtxT& ctx, OutT& out) noexcept(CtxT::is_nothrow) {
        using ascii::is_hex_digit;

        // NOLINTBEGIN(*-inc-dec-in-conditions)
        auto       cur      = ctx.pos;
        bool const is_valid = cur++ + 2 <= ctx.end && is_hex_digit(*cur) && is_hex_digit(*++cur);
        append_n(ctx, out, cur - ctx.pos);
        return is_valid;
        // NOLINTEND(*-inc-dec-in-conditions)
    }

    template <typename Iter, typename OutT>
    [[nodiscard]] static constexpr bool decode_percent_encoded(Iter& spos, Iter const send, OutT& out) {
        // static_assert(CtxT::is_modifiable, "The output must be modifiable");

        Iter cur = spos;
        if (cur++ + 2 > send) [[unlikely]] {
            return false;
        }
        auto const ch0 = ascii::hex_digit<stl::int8_t>(*cur);
        auto const ch1 = ascii::hex_digit<stl::int8_t>(*++cur);
        if (ch0 < 0 || ch1 < 0) [[unlikely]] {
            return false;
        }
        auto const code_point = static_cast<stl::uint8_t>(ch0 * 10) + ch1;
        append(out, static_cast<char>(code_point));
        spos = cur;
        return true;
    }


} // namespace webpp::uri::details

#endif // URI_COMPONENTS_ENCODING_HPP
