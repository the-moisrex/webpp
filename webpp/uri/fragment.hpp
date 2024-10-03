// Created by moisrex on 11/4/20.

#ifndef WEBPP_URI_FRAGMENT_HPP
#define WEBPP_URI_FRAGMENT_HPP

#include "../std/string_like.hpp"
#include "./details/constants.hpp"
#include "./details/iiequals.hpp"
#include "./details/uri_components_encoding.hpp"
#include "encoding.hpp"

namespace webpp::uri {

    template <uri_parsing_options Options = uri_parsing_options{}, ParsingURIContext CtxT>
    static constexpr void parse_fragment(CtxT& ctx) noexcept(CtxT::is_nothrow) {
        // https://url.spec.whatwg.org/#fragment-state
        using ctx_type  = CtxT;
        using char_type = typename ctx_type::char_type;

        if (ctx.pos == ctx.end) {
            set_valid(ctx.status, uri_status::valid);
            return;
        }

        if constexpr (Options.parse_fragment) {
            details::component_encoder<components::fragment, ctx_type> encoder{ctx};
            while (!encoder.template encode_or_validate<uri_encoding_policy::encode_chars>(
              details::FRAGMENT_ENCODE_SET,
              charset<char_type, 1>('%')))
            {
                switch (*ctx.pos) {
                    case '%':
                        if (encoder.template validate_percent_encode<Options.ignore_tabs_or_newlines>()) {
                            continue;
                        }
                        break;
                    default: break;
                }
                set_warning(ctx.status, uri_status::invalid_character);
            }
            encoder.set_value();
            set_valid(ctx.status, uri_status::valid);
        } else {
            if (ctx.pos == ctx.end) {
                set_valid(ctx.status, uri_status::valid);
            } else {
                set_warning(ctx.status, uri_status::invalid_character);
            }
        }
    }

    /// Serialize fragment
    template <istl::StringLike StorageStrT, istl::StringLike StrT>
    static constexpr void
    render_fragment(StorageStrT const& storage, StrT& out, bool const add_separators = false)
      noexcept(!istl::ModifiableString<StrT>) {
        // https://url.spec.whatwg.org/#url-serializing
        using string_type = StrT;
        using char_type   = typename string_type::value_type;

        if (storage.empty()) {
            return;
        }
        if constexpr (istl::ModifiableString<StrT>) {
            if (add_separators) {
                out.push_back(static_cast<char_type>('#'));
            }
        }
        istl::append(out, storage);
    }

    /**
     * @brief Basic Fragment (or sometimes called Hash, like in WHATWG)
     * @tparam StringType String or String View type to be used as a storage
     */
    template <istl::StringLike StringType = stl::string_view>
    struct basic_fragment {
        using string_type = StringType;
        using char_type   = typename string_type::value_type;
        using iterator    = typename string_type::iterator;
        using size_type   = typename string_type::size_type;

        static constexpr bool is_modifiable   = istl::ModifiableString<string_type>;
        static constexpr bool is_nothrow      = !is_modifiable;
        static constexpr bool needs_allocator = requires { typename string_type::allocator_type; };


      private:
        string_type storage;

      public:
        template <uri_parsing_options Options = uri_parsing_options{}, typename Iter = iterator>
        constexpr uri_status_type parse(Iter beg, Iter end) noexcept(is_nothrow) {
            parsing_uri_component_context<components::fragment, string_type*, stl::remove_cvref_t<Iter>>
              ctx{};
            ctx.beg = beg;
            ctx.pos = beg;
            ctx.end = end;
            ctx.out = stl::addressof(storage);
            parse_fragment<Options>(ctx);
            return ctx.status;
        }

        template <Allocator AllocT = allocator_type_from_t<string_type>>
            requires needs_allocator
        explicit constexpr basic_fragment(AllocT const& alloc = {}) noexcept : storage{alloc} {}

        template <Allocator AllocT = allocator_type_from_t<string_type>>
            requires(!needs_allocator)
        explicit constexpr basic_fragment([[maybe_unused]] AllocT const& alloc = {}) noexcept {}

        template <istl::StringLike InpStr = stl::basic_string_view<char_type>>
        explicit constexpr basic_fragment(InpStr const& inp_str) noexcept(is_nothrow) {
            parse(inp_str.begin(), inp_str.end());
        }

        template <istl::StringLike InpStr = stl::basic_string_view<char_type>>
        constexpr basic_fragment& operator=(InpStr const& inp_str) noexcept(is_nothrow) {
            parse(inp_str.begin(), inp_str.end());
            return *this;
        }

        [[nodiscard]] constexpr size_type size() const noexcept {
            return storage.size();
        }

        constexpr void clear() {
            istl::clear(storage);
        }

        template <istl::StringView StrVT = stl::basic_string_view<char_type>>
        [[nodiscard]] constexpr StrVT view() const noexcept {
            return StrVT{storage.data(), storage.size()};
        }

        template <istl::StringLike NStrT = stl::basic_string_view<char_type>>
        constexpr void to_string(NStrT& out, bool const append_separators = false) const
          noexcept(!istl::ModifiableString<NStrT>) {
            render_fragment(storage, out, append_separators);
        }

        template <istl::StringLike NStrT = stl::basic_string_view<char_type>, typename... Args>
        [[nodiscard]] constexpr NStrT as_string(Args&&... args) const
          noexcept(!istl::ModifiableString<NStrT>) {
            NStrT out{stl::forward<Args>(args)...};
            to_string(out);
            return out;
        }

        /**
         * @brief Replace the value with the specified raw data, without parsing
         * @param beg start of the value
         * @param end the end of the value
         */
        constexpr void assign(iterator beg, iterator end) noexcept(!is_modifiable) {
            istl::assign(storage, beg, end);
        }

        /**
         * @brief check if we have value
         * @return false if we don't have anything
         */
        [[nodiscard]] constexpr bool has_value() const noexcept {
            return !storage.empty();
        }

        [[nodiscard]] constexpr auto const& get_allocator() const noexcept {
            return storage.get_allocator();
        }

        [[nodiscard]] constexpr auto& storage_ref() noexcept {
            return storage;
        }

        [[nodiscard]] constexpr auto const& storage_ref() const noexcept {
            return storage;
        }

        template <istl::StringViewifiable NStrT = stl::basic_string_view<char_type>>
        [[nodiscard]] constexpr bool operator==(NStrT&& inp_str) const noexcept {
            if constexpr (is_modifiable) {
                return iiequals_fl<uri::details::TABS_OR_NEWLINES<char_type>>(
                  storage,
                  stl::forward<NStrT>(inp_str));
            } else {
                return iiequals<uri::details::TABS_OR_NEWLINES<char_type>>(storage,
                                                                           stl::forward<NStrT>(inp_str));
            }
        }

        [[nodiscard]] constexpr bool operator==(basic_fragment const& other) const noexcept {
            return storage == other.storage_ref();
        }
    };


} // namespace webpp::uri

#endif // WEBPP_URI_FRAGMENT_HPP
