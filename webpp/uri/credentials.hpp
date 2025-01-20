// Created by moisrex on 1/23/24.

#ifndef WEBPP_URI_CREDENTIALS_HPP
#define WEBPP_URI_CREDENTIALS_HPP

#include "../std/string_like.hpp"
#include "../std/string_view.hpp"
#include "./parser/parse_credentials.hpp"
#include "./parser/uri_components.hpp"
#include "uri_status.hpp"

namespace webpp::uri {

    /// Serialize username
    template <istl::StringLike StorageStrT, istl::StringLike StrT>
    static constexpr void render_username(StorageStrT const& storage, StrT& out)
      noexcept(!istl::ModifiableString<StrT>) {
        // https://url.spec.whatwg.org/#url-serializing
        if (storage.empty()) {
            return;
        }
        istl::append(out, storage);
    }

    /// Serialize password
    template <istl::StringLike StorageStrT, istl::StringLike StrT>
    static constexpr void render_password(StorageStrT const& storage, StrT& out)
      noexcept(!istl::ModifiableString<StrT>) {
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
