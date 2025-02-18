// Created by moisrex on 11/4/20.

#ifndef WEBPP_URI_QUERIES_HPP
#define WEBPP_URI_QUERIES_HPP

#include "../std/map.hpp"
#include "./parser/parse_queries.hpp"

namespace webpp::uri {


    /**
     * Serialize queries
     * Storage type can be structured (not string-like), and also
     * Storage type can be string-like (not-structured)
     */
    template <typename StorageType, istl::StringLike StrT>
    static constexpr void
    render_queries(StorageType const& storage, StrT& out, bool const add_separators = false)
      noexcept(!istl::ModifiableString<StrT>) {
        // https://url.spec.whatwg.org/#url-serializing
        if (storage.empty()) {
            return;
        }
        if (add_separators) {
            out += '?';
        }
        if constexpr (istl::StringLike<StorageType>) {
            istl::append(out, storage);
        } else {
            for (auto pos = storage.begin();;) {
                auto const [name, value]  = *pos;
                out                      += name;
                if (!value.empty()) {
                    out += '=';
                    out += value;
                }
                if (++pos == storage.end()) {
                    break;
                }
                out += '&';
            }
        }
    }

    /**
     * @brief Basic Queries (or sometimes called Searches, like in WHATWG) stored as key-values
     * @tparam StringType String or String View type used as the storage type
     * @tparam AllocType Allocator type
     */
    template <istl::StringLike StringType = stl::string,
              Allocator        AllocType  = allocator_type_from_t<StringType>>
    struct basic_queries
      : stl::map<StringType,
                 StringType,
                 stl::less<>,
                 rebind_allocator<AllocType, stl::pair<stl::add_const_t<StringType>, StringType>>> {
        using map_type =
          stl::map<StringType,
                   StringType,
                   stl::less<>,
                   rebind_allocator<AllocType, stl::pair<stl::add_const_t<StringType>, StringType>>>;
        using string_type    = StringType;
        using key_type       = typename map_type::key_type;
        using mapped_type    = typename map_type::mapped_type;
        using char_type      = typename string_type::value_type;
        using value_type     = stl::pair<key_type, mapped_type>;
        using pack_type      = typename map_type::value_type;
        using allocator_type = typename map_type::allocator_type;

        using iterator       = typename map_type::iterator;
        using const_iterator = typename map_type::const_iterator;

        static constexpr bool is_modifiable = istl::ModifiableString<string_type>;
        static constexpr bool is_segregated = true;
        static constexpr bool is_nothrow    = false;
        static constexpr auto allowed_chars = details::QUERY_OR_FRAGMENT_NOT_PCT_ENCODED<char_type>;

        template <uri_parsing_options Options = uri_parsing_options{}, typename Iter = iterator>
        constexpr uri_status_type parse(Iter beg, Iter end) noexcept(is_nothrow) {
            parsing_uri_component_context<components::queries, map_type*, stl::remove_cvref_t<Iter>> ctx{};
            ctx.beg = beg;
            ctx.pos = beg;
            ctx.end = end;
            ctx.out = static_cast<map_type*>(this);
            parse_fragment<Options>(ctx);
            return ctx.status;
        }

        template <typename... Args>
            requires requires(Args... args) { map_type{stl::forward<Args>(args)...}; }
        explicit constexpr basic_queries(Args&&... args)
          noexcept(noexcept(map_type(stl::forward<Args>(args)...)))
          : map_type{stl::forward<Args>(args)...} {}

        /**
         * Get the raw string non-decoded size
         */
        [[nodiscard]] stl::size_t raw_string_size() const noexcept {
            // todo: we could remove lambda; or we even can use an iterator_wrapper and use "std::reduce"
            return [this]() noexcept -> stl::size_t {
                stl::size_t sum = 0;
                for (auto const& [key, value] : *this) {
                    sum += key.size();
                    sum += value.size();
                }
                return sum;
            }() + (this->size() * 2) - 2;
        }

        /**
         * @brief Replace the values with the specified raw data, without parsing
         * @param beg start of the value
         * @param end the end of the value
         */
        constexpr void assign(iterator beg, iterator end) {
            // first "name" is chosen for the whole value, because of the algorithm that gets the queries will
            // be correct that way

            this->clear();
            if constexpr (is_modifiable) {
                istl::emplace_one(static_cast<basic_queries&>(*this),
                                  pack_type{
                                    key_type{beg, end, this->get_allocator()},
                                    mapped_type{this->get_allocator()}
                });
            } else {
                istl::emplace_one(static_cast<basic_queries&>(*this),
                                  pack_type{
                                    key_type{beg, end},
                                    mapped_type{}
                });
            }
        }

        [[nodiscard]] constexpr auto& storage_ref() noexcept {
            return static_cast<map_type&>(*this);
        }

        [[nodiscard]] constexpr auto const& storage_ref() const noexcept {
            return static_cast<map_type const&>(*this);
        }

        /**
         * @brief check if we have value
         * @return false if we don't have anything
         */
        [[nodiscard]] constexpr bool has_value() const noexcept {
            return !this->empty();
        }

        template <istl::String NStrT = stl::string>
        constexpr void to_string(NStrT& out, bool const add_separator = false) const {
            render_queries(storage_ref(), out, add_separator);
        }

        template <istl::String NStrT = stl::string, typename... Args>
        [[nodiscard]] constexpr NStrT as_string(Args&&... args) const {
            NStrT out{stl::forward<Args>(args)...};
            to_string(out);
            return out;
        }

        /// Equality check
        /// Attention: this function doesn't parse your input
        template <istl::StringViewifiable NStrT = stl::basic_string_view<char_type>>
        [[nodiscard]] constexpr bool operator==(NStrT&& inp_str) const noexcept {
            // NOLINTNEXTLINE(*-avoid-c-arrays)
            webpp_static_constexpr char_type queries_separators[3]{'=', '&', '\0'};

            auto str = istl::string_viewify(stl::forward<NStrT>(inp_str));
            while (!str.empty()) {
                auto const name = str.substr(0, str.find_first_of(queries_separators));
                str.remove_prefix(name.size());
                if (str.starts_with('=')) {
                    str.remove_prefix(1);
                }
                auto const value = str.substr(0, str.find_first_of('&'));
                if (auto const res = storage_ref().find(name);
                    name.empty() || res == this->end() || res->second != value)
                {
                    return false;
                }
                str.remove_prefix(value.size());
                if (str.starts_with('&')) {
                    str.remove_prefix(1);
                }
            }
            return true;
        }

        [[nodiscard]] constexpr bool operator==(basic_queries const& other) const noexcept {
            return storage_ref() == other.storage_ref();
        }

        friend constexpr string_type get_buffer(basic_queries const& queries) noexcept
            requires(is_modifiable)
        {
            return string_type{queries.get_allocator()};
        }
    };

} // namespace webpp::uri

#endif // WEBPP_URI_QUERIES_HPP
