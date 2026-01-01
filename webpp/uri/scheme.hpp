// Created by moisrex on 10/30/20.

#ifndef WEBPP_URI_SCHEME_HPP
#define WEBPP_URI_SCHEME_HPP

#include "../std/string.hpp"
#include "../std/string_like.hpp"
#include "./parser/parse_scheme.hpp"

namespace webpp::uri {

    /// Serialize scheme
    template <istl::StringLike StorageStrT, istl::StringLike StrT>
    static constexpr void
    render_scheme(StorageStrT const& storage, StrT& out, bool const add_separators = istl::ModifiableString<StrT>)
      noexcept(!istl::ModifiableString<StrT>) {
        // https://url.spec.whatwg.org/#url-serializing

        if (storage.empty()) {
            return;
        }
        istl::append(out, storage);
        if constexpr (istl::ModifiableString<StrT>) {
            if (add_separators) {
                out.push_back(':');
            }
        }
    }

    /**
     * Scheme or Protocol
     * @tparam StringType
     */
    template <istl::StringLike StringType = stl::string_view>
    struct basic_scheme : StringType {
        using string_type      = StringType;
        using char_type        = istl::char_type_of_t<string_type>;
        using iterator         = typename string_type::iterator;
        using size_type        = typename string_type::size_type;
        using allocator_type   = allocator_type_from_t<string_type>;
        using string_view_type = istl::string_view_type_of<StringType>;

        static constexpr bool is_modifiable   = istl::ModifiableString<string_type>;
        static constexpr bool is_nothrow      = !is_modifiable;
        static constexpr bool needs_allocator = requires { typename string_type::allocator_type; };

        template <uri_options Options = uri_options{}, typename Iter = iterator>
        constexpr uri_status_type
          parse(Iter beg, Iter end, uri_status_type const initial_status = +uri_status::unparsed) noexcept(is_nothrow) {
            parsing_uri_component_context<components::scheme, basic_scheme*, stl::remove_cvref_t<Iter>> ctx{};
            ctx.beg    = beg;
            ctx.pos    = beg;
            ctx.end    = end;
            ctx.out    = this;
            ctx.status = initial_status;
            parse_scheme<Options>(ctx);
            return ctx.status;
        }

        template <typename... Args>
        explicit constexpr basic_scheme(Args&&... args) noexcept(is_nothrow) : StringType{std::forward<Args>(args)...} {
            parse(this->begin(), this->end());
        }

        constexpr basic_scheme& operator=(string_view_type const& inp_str) noexcept(is_nothrow) {
            parse(inp_str.begin(), inp_str.end());
            return *this;
        }

        constexpr void clear() noexcept {
            istl::clear(storage_ref());
        }

        /**
         * Return the default for the current scheme.
         *  - http, ws:   80
         *  - https, wss: 443
         *  - ftp:        21
         *  - others:     0
         */
        [[nodiscard]] constexpr stl::uint16_t known_port() const noexcept {
            return uri::known_port(view(*this));
        }

        /**
         * @brief checks if the URI is a relative reference
         */
        [[nodiscard]] constexpr bool is_relative_reference() const noexcept {
            return this->empty();
        }

        /**
         * @brief Replace the value with the specified raw data, without parsing
         * @param beg start of the value
         * @param end the end of the value
         */
        template <typename Iter = iterator>
        constexpr void assign(Iter beg, Iter end) noexcept(!is_modifiable) {
            istl::assign(storage_ref(), beg, end);
        }

        template <istl::StringView StrVT = string_view_type>
        [[nodiscard]] constexpr StrVT view() const noexcept {
            return StrVT{this->data(), this->size()};
        }

        [[nodiscard]] constexpr bool is_special() const noexcept {
            return is_special_scheme(this->view());
        }

        /**
         * @brief check if we have value
         * @return false if we don't have anything
         */
        [[nodiscard]] constexpr bool has_value() const noexcept {
            return !this->empty();
        }

        template <istl::StringLike NStrT = string_view_type>
        constexpr void to_string(NStrT& out, bool const append_separators = false) const
          noexcept(!istl::ModifiableString<NStrT>) {
            render_scheme(storage_ref(), out, append_separators);
        }

        template <istl::StringLike NStrT = string_view_type, typename... Args>
        [[nodiscard]] constexpr NStrT as_string(Args&&... args) const noexcept(!istl::ModifiableString<NStrT>) {
            NStrT out{stl::forward<Args>(args)...};
            to_string(out);
            return out;
        }

        [[nodiscard]] constexpr auto& storage_ref() noexcept {
            return static_cast<StringType&>(*this);
        }

        [[nodiscard]] constexpr auto const& storage_ref() const noexcept {
            return static_cast<StringType const&>(*this);
        }

        [[nodiscard]] constexpr bool operator==(basic_scheme const& other) const noexcept {
            return storage_ref() == other.storage_ref();
        }
    };

} // namespace webpp::uri

#endif // WEBPP_URI_SCHEME_HPP
