// Created by moisrex on 1/23/24.

#ifndef WEBPP_URI_CREDENTIALS_HPP
#define WEBPP_URI_CREDENTIALS_HPP

#include "../std/string.hpp"
#include "./details/constants.hpp"
#include "./details/uri_components_encoding.hpp"
#include "./details/uri_status.hpp"

namespace webpp::uri {

    namespace details {

        template <ParsingURIContext CtxT, typename Iter = typename CtxT::iterator>
        static constexpr void parse_credentials(CtxT& ctx, Iter beg, Iter password_token_pos) noexcept(
          CtxT::is_nothrow) {
            // todo: add "needs_encoding"
            // todo: See if there's a way to find the last atsign position instead of running this function for every atsign
            // todo: use already parsed host

            using details::ascii_bitmap;
            using details::component_encoder;
            using details::USER_INFO_ENCODE_SET;

            using ctx_type = CtxT;
            using iterator = typename ctx_type::iterator;

            webpp_assume(ctx.pos < ctx.end);

            set_warning(ctx.status, uri_status::has_credentials);
            auto const atsign_pos = ctx.pos;

            // append to the username and password
            if (atsign_pos != ctx.end) {
                // parse username
                iterator const username_beg = beg;
                iterator const username_end = stl::min(password_token_pos, atsign_pos);

                clear<components::username>(ctx); // todo: it's optimizable
                component_encoder<components::username, ctx_type> user_encoder{ctx};
                user_encoder.template encode_or_set<uri_encoding_policy::encode_chars>(
                  username_beg,
                  username_end,
                  USER_INFO_ENCODE_SET);

                // parse password
                if (password_token_pos != ctx.end) {
                    iterator const password_beg = password_token_pos + 1;
                    iterator const password_end = atsign_pos;

                    clear<components::password>(ctx); // todo: it's optimizable
                    component_encoder<components::password, ctx_type> pass_encoder{ctx};
                    pass_encoder.template encode_or_set<uri_encoding_policy::encode_chars>(
                      password_beg,
                      password_end,
                      USER_INFO_ENCODE_SET);
                }
            }
        }

    } // namespace details

    /// parse username
    /// This function doesn't care about boundaries, encodes and validates
    /// This function is not being used inside the URI parsing at all
    template <uri_parsing_options Options = uri_parsing_options{}, ParsingURIContext CtxT>
    static constexpr void parse_username(CtxT& ctx) noexcept(CtxT::is_nothrow) {
        using details::ascii_bitmap;
        using details::component_encoder;
        using details::USER_INFO_ENCODE_SET;

        using ctx_type = CtxT;

        if constexpr (Options.parse_credentails) {
            if (ctx.pos == ctx.end) {
                return;
            }

            webpp_assume(ctx.pos < ctx.end);
            set_warning(ctx.status, uri_status::has_credentials);

            clear<components::username>(ctx);
            component_encoder<components::username, ctx_type> user_encoder{ctx};
            user_encoder.template encode_or_set<uri_encoding_policy::encode_chars>(
              ctx.pos,
              ctx.end,
              USER_INFO_ENCODE_SET);
        }
    }

    /// parse password
    /// This function doesn't care about boundaries, encodes and validates
    /// This function is not being used inside the URI parsing at all
    template <uri_parsing_options Options = uri_parsing_options{}, ParsingURIContext CtxT>
    static constexpr void parse_password(CtxT& ctx) noexcept(CtxT::is_nothrow) {
        using details::ascii_bitmap;
        using details::component_encoder;
        using details::USER_INFO_ENCODE_SET;

        using ctx_type = CtxT;

        if constexpr (Options.parse_credentails) {
            if (ctx.pos == ctx.end) {
                return;
            }

            webpp_assume(ctx.pos < ctx.end);
            set_warning(ctx.status, uri_status::has_credentials);

            clear<components::password>(ctx);
            component_encoder<components::password, ctx_type> pass_encoder{ctx};
            pass_encoder.template encode_or_set<uri_encoding_policy::encode_chars>(
              ctx.pos,
              ctx.end,
              USER_INFO_ENCODE_SET);
        }
    }

    /// Serialize username
    template <istl::StringLike StorageStrT, istl::StringLike StrT>
    static constexpr void render_username(StorageStrT const& storage, StrT& out) noexcept(
      !istl::ModifiableString<StrT>) {
        // https://url.spec.whatwg.org/#url-serializing
        if (storage.empty()) {
            return;
        }
        istl::append(out, storage);
    }

    /// Serialize password
    template <istl::StringLike StorageStrT, istl::StringLike StrT>
    static constexpr void render_password(StorageStrT const& storage, StrT& out) noexcept(
      !istl::ModifiableString<StrT>) {
        // https://url.spec.whatwg.org/#url-serializing
        if (storage.empty()) {
            return;
        }
        istl::append(out, storage);
    }

    /**
     * @brief Basic structured username
     * @tparam StringType Storage Type
     */
    template <istl::StringLike StringType = stl::string_view>
    struct basic_username {
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
            parsing_uri_component_context<components::username, string_type*, stl::remove_cvref_t<Iter>>
              ctx{};
            ctx.beg = beg;
            ctx.pos = beg;
            ctx.end = end;
            ctx.out = stl::addressof(storage);
            parse_username<Options>(ctx);
            return ctx.status;
        }

        template <Allocator AllocT = allocator_type_from_t<string_type>>
            requires needs_allocator
        explicit constexpr basic_username(AllocT const& alloc = {}) noexcept : storage{alloc} {}

        template <Allocator AllocT = allocator_type_from_t<string_type>>
            requires(!needs_allocator)
        explicit constexpr basic_username([[maybe_unused]] AllocT const& alloc = {}) noexcept {}

        template <istl::StringLike InpStr = stl::basic_string_view<char_type>>
        explicit constexpr basic_username(InpStr const& inp_str) noexcept(is_nothrow) {
            parse(inp_str.begin(), inp_str.end());
        }

        template <istl::StringLike InpStr = stl::basic_string_view<char_type>>
        constexpr basic_username& operator=(InpStr const& inp_str) noexcept(is_nothrow) {
            parse(inp_str.begin(), inp_str.end());
            return *this;
        }

        /**
         * @brief check if we have value
         * @return false if we don't have anything
         */
        [[nodiscard]] constexpr bool has_value() const noexcept {
            return !storage.empty();
        }

        [[nodiscard]] constexpr size_type size() const noexcept {
            return storage.size();
        }

        constexpr void clear() {
            istl::clear(storage);
        }

        constexpr void assign(iterator beg, iterator end) noexcept(!is_modifiable) {
            istl::assign(storage, beg, end);
        }

        template <istl::StringView StrVT = stl::basic_string_view<char_type>>
        [[nodiscard]] constexpr StrVT view() const noexcept {
            return StrVT{storage.data(), storage.size()};
        }

        template <istl::StringLike NStrT = stl::basic_string_view<char_type>>
        constexpr void to_string(NStrT& out) const noexcept(!istl::ModifiableString<NStrT>) {
            render_username(storage, out);
        }

        template <istl::StringLike NStrT = stl::basic_string_view<char_type>, typename... Args>
        [[nodiscard]] constexpr NStrT as_string(Args&&... args) const
          noexcept(!istl::ModifiableString<NStrT>) {
            NStrT out{stl::forward<Args>(args)...};
            to_string(out);
            return out;
        }

        [[nodiscard]] constexpr decltype(auto) get_allocator() const noexcept {
            return storage.get_allocator();
        }

        [[nodiscard]] constexpr auto& storage_ref() noexcept {
            return storage;
        }

        [[nodiscard]] constexpr auto const& storage_ref() const noexcept {
            return storage;
        }

        /// Equality check
        /// Attention: this function doesn't parse your input
        template <istl::StringViewifiable NStrT = stl::basic_string_view<char_type>>
        [[nodiscard]] constexpr bool operator==(NStrT&& inp_str) const noexcept {
            return storage == stl::forward<NStrT>(inp_str);
        }

        [[nodiscard]] constexpr bool operator==(basic_username const& other) const noexcept {
            return storage == other.storage_ref();
        }
    };

    /**
     * @brief Basic structured password
     * @tparam StringType Storage type
     */
    template <istl::StringLike StringType = stl::string_view>
    struct basic_password {
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
            parsing_uri_component_context<components::password, string_type*, stl::remove_cvref_t<Iter>>
              ctx{};
            ctx.beg = beg;
            ctx.pos = beg;
            ctx.end = end;
            ctx.out = stl::addressof(storage);
            parse_password<Options>(ctx);
            return ctx.status;
        }

        template <Allocator AllocT = allocator_type_from_t<string_type>>
            requires needs_allocator
        explicit constexpr basic_password(AllocT const& alloc = {}) noexcept : storage{alloc} {}

        template <Allocator AllocT = allocator_type_from_t<string_type>>
            requires(!needs_allocator)
        explicit constexpr basic_password([[maybe_unused]] AllocT const& alloc = {}) noexcept {}

        template <istl::StringLike InpStr = stl::basic_string_view<char_type>>
        explicit constexpr basic_password(InpStr const& inp_str) noexcept(is_nothrow) {
            parse(inp_str.begin(), inp_str.end());
        }

        template <istl::StringLike InpStr = stl::basic_string_view<char_type>>
        constexpr basic_password& operator=(InpStr const& inp_str) noexcept(is_nothrow) {
            parse(inp_str.begin(), inp_str.end());
            return *this;
        }

        [[nodiscard]] constexpr size_type size() const noexcept {
            return storage.size();
        }

        constexpr void clear() {
            istl::clear(storage);
        }

        constexpr void assign(iterator beg, iterator end) noexcept(!is_modifiable) {
            istl::assign(storage, beg, end);
        }

        template <istl::StringView StrVT = stl::basic_string_view<char_type>>
        [[nodiscard]] constexpr StrVT view() const noexcept {
            return StrVT{storage.data(), storage.size()};
        }

        template <istl::StringLike NStrT = stl::basic_string_view<char_type>>
        constexpr void to_string(NStrT& out) const noexcept(!istl::ModifiableString<NStrT>) {
            render_password(storage, out);
        }

        template <istl::StringLike NStrT = stl::basic_string_view<char_type>, typename... Args>
        [[nodiscard]] constexpr NStrT as_string(Args&&... args) const
          noexcept(!istl::ModifiableString<NStrT>) {
            NStrT out{stl::forward<Args>(args)...};
            to_string(out);
            return out;
        }

        /**
         * @brief check if we have value
         * @return false if we don't have anything
         */
        [[nodiscard]] constexpr bool has_value() const noexcept {
            return !storage.empty();
        }

        [[nodiscard]] constexpr decltype(auto) get_allocator() const noexcept {
            return storage.get_allocator();
        }

        [[nodiscard]] constexpr auto& storage_ref() noexcept {
            return storage;
        }

        [[nodiscard]] constexpr auto const& storage_ref() const noexcept {
            return storage;
        }

        /// Equality check
        /// Attention: this function doesn't parse your input
        template <istl::StringViewifiable NStrT = stl::basic_string_view<char_type>>
        [[nodiscard]] constexpr bool operator==(NStrT&& inp_str) const noexcept {
            return storage == stl::forward<NStrT>(inp_str);
        }

        [[nodiscard]] constexpr bool operator==(basic_password const& other) const noexcept {
            return storage == other.storage_ref();
        }
    };

} // namespace webpp::uri

#endif // WEBPP_URI_CREDENTIALS_HPP
