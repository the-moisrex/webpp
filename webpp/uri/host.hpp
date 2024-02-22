// Created by moisrex on 11/4/20.

#ifndef WEBPP_URI_HOST_HPP
#define WEBPP_URI_HOST_HPP

#include "../memory/allocators.hpp"
#include "../std/string.hpp"
#include "../std/string_view.hpp"
#include "../std/vector.hpp"
#include "./details/uri_components_encoding.hpp"

namespace webpp::uri {


    /**
     * @brief Basic Host
     * @tparam StringType
     */
    template <istl::StringLike StringType = stl::string_view,
              Allocator        AllocT     = allocator_type_from_t<StringType>>
    struct basic_host {
        using string_type    = StringType;
        using allocator_type = rebind_allocator<AllocT, string_type>;
        using container_type = stl::vector<string_type, allocator_type>;
        using vector_type    = container_type;
        using char_type      = typename string_type::value_type;

        using iterator       = typename container_type::const_iterator;
        using const_iterator = typename container_type::const_iterator;
        using size_type      = typename container_type::size_type;

        static constexpr bool is_modifiable = istl::ModifiableString<string_type>;
        static constexpr bool is_nothrow    = !is_modifiable;
        static constexpr bool is_segregated = true;

        /// string_type if it's modifiable, otherwise, std::string
        using modifiable_string_type = istl::defaulted_string<string_type, allocator_type>;

      private:
        container_type storage;

      public:
        template <uri_parsing_options Options = uri_parsing_options{}, typename Iter = iterator>
        constexpr uri_status_type parse(Iter beg, Iter end) noexcept(is_nothrow) {
            parsing_uri_component_context<components::host, basic_host*, stl::remove_cvref_t<Iter>> ctx{};
            ctx.beg = beg;
            ctx.pos = beg;
            ctx.end = end;
            ctx.out = this;
            // todo: parse hose not authority (even though they will be ignored)
            parse_authority<Options>(ctx);
            return ctx.status;
        }

        template <Allocator NAllocT = allocator_type>
        explicit constexpr basic_host(NAllocT const& alloc = {}) noexcept : storage{alloc} {}

        template <istl::StringViewifiable InpStr = stl::basic_string_view<char_type>>
        explicit constexpr basic_host(InpStr&& inp_str) noexcept(is_nothrow) {
            auto const str = istl::string_viewify(stl::forward<InpStr>(inp_str));
            parse(str.begin(), str.end());
        }

        template <istl::StringLike InpStr = stl::basic_string_view<char_type>>
        constexpr basic_host& operator=(InpStr const& inp_str) noexcept(is_nothrow) {
            parse(inp_str.begin(), inp_str.end());
            return *this;
        }

        template <istl::StringViewifiable StrT>
        constexpr basic_host& operator+=(StrT&& inp_str) {
            auto const str = istl::string_viewify(stl::forward<StrT>(inp_str));
            parse(str.begin(), str.end());
            return *this;
        }

        // todo: add prepend

        /**
         * @brief Replace the values with the specified raw data, without parsing
         * @param beg start of the value
         * @param end the end of the value
         */
        template <typename Iter = iterator>
        constexpr void assign(Iter beg, Iter end) {
            storage.clear();
            if constexpr (is_modifiable) {
                istl::emplace_one(storage, beg, end, storage.get_allocator());
            } else {
                istl::emplace_one(storage, beg, end);
            }
        }

        template <istl::String NStrT = modifiable_string_type>
        constexpr void to_string(NStrT& out) const {
            if (storage.empty()) {
                return;
            }
            for (auto pos = storage.begin();;) {
                out += *pos;
                if (++pos == storage.end()) {
                    break;
                }
                out += '.';
            }
        }

        template <istl::String NStrT = modifiable_string_type, typename... Args>
        [[nodiscard]] constexpr NStrT as_string(Args&&... args) const {
            NStrT out{stl::forward<Args>(args)...};
            to_string(out);
            return out;
        }

        /**
         * @brief check if we have value
         * @return false if we don't have anything
         */
        [[nodiscard]] constexpr bool has_value() const noexcept {
            return !storage.empty() && !(storage.size() == 1 && storage.front().empty());
        }

        /**
         * Top Level Domain; sometimes called the extension
         */
        [[nodiscard]] constexpr auto tld() const {
            return storage.back();
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

        [[nodiscard]] constexpr decltype(auto) front() const noexcept {
            return storage.front();
        }

        [[nodiscard]] constexpr decltype(auto) back() const noexcept {
            return storage.back();
        }

        [[nodiscard]] constexpr decltype(auto) begin() const noexcept {
            return storage.begin();
        }

        [[nodiscard]] constexpr decltype(auto) end() const noexcept {
            return storage.end();
        }

        [[nodiscard]] constexpr decltype(auto) begin() noexcept {
            return storage.begin();
        }

        [[nodiscard]] constexpr decltype(auto) end() noexcept {
            return storage.end();
        }

        [[nodiscard]] constexpr size_type size() const noexcept {
            return storage.size();
        }

        constexpr void clear() noexcept {
            return storage.clear();
        }

        constexpr void pop_back() noexcept {
            return storage.pop_back();
        }

        /// Equality check
        /// Attention: this function doesn't parse your input
        template <istl::StringViewifiable NStrT = stl::basic_string_view<char_type>>
        [[nodiscard]] constexpr bool operator==(NStrT&& inp_str) const noexcept {
            auto str      = istl::string_viewify(stl::forward<NStrT>(inp_str));
            auto piece_it = storage.begin();
            for (; piece_it != storage.end(); ++piece_it) {
                bool should_continue = false;
                if constexpr (is_modifiable) {
                    should_continue = ascii::iequals_fl(*piece_it, str.substr(0, piece_it->size()));
                } else {
                    should_continue = ascii::iequals(*piece_it, str.substr(0, piece_it->size()));
                }
                if (!should_continue) {
                    return false;
                }
                str.remove_prefix(piece_it->size());
                if (!str.starts_with('.')) {
                    ++piece_it;
                    break;
                }
                str.remove_prefix(1);
            }
            return str.empty() && piece_it == storage.end();
        }

        [[nodiscard]] constexpr bool operator==(basic_host const& other) const noexcept {
            return storage == other.storage_ref();
        }
    };

} // namespace webpp::uri

#endif // WEBPP_URI_HOST_HPP
