#ifndef WEBPP_CACHE_CACHE_HPP
#define WEBPP_CACHE_CACHE_HPP

#include "../std/iterator.hpp"
#include "cache_concepts.hpp"

namespace webpp {

    template <typename CacheType>
    struct cache_result : private CacheType::optional_value_type {
        using cache_type          = CacheType;
        using optional_value_type = typename CacheType::optional_value_type;
        using key_type            = typename cache_type::key_type;
        using value_type          = typename cache_type::value_type;

      private:
        cache_type* cache_ptr;
        key_type    the_key;

      public:
        constexpr cache_result(cache_type& input_cache, key_type key, optional_value_type&& val) noexcept
          : optional_value_type{stl::move(val)},
            cache_ptr{&input_cache},
            the_key{stl::move(key)} {}

#if __cpp_lib_optional >= 202'110L // C++23 Monadic operations in std::optional
        using CacheType::optional_value_type::and_then;
        using CacheType::optional_value_type::or_else;
#endif
        using CacheType::optional_value_type::emplace;
        using CacheType::optional_value_type::reset;
        using CacheType::optional_value_type::value;
        using CacheType::optional_value_type::value_or;

        template <CacheValue V>
            requires(stl::is_convertible_v<V, value_type>)
        constexpr cache_result& operator=(V&& new_val) {
            cache_ptr->set(the_key, stl::forward<V>(new_val));
            return *this;
        }

        constexpr key_type key() const noexcept {
            return the_key;
        }

        constexpr cache_result& save() {
            cache_ptr->set(the_key, this->value());
            return *this;
        }
    };

    /**
     * This class is mother of all caches.
     *
     * Cache Strategy: The type of cache; e.g. LRU, Expired LRU, ...
     * Storage Gate:   How the cache data is stored (and where); e.g. Memory, File, ...
     */
    template <Traits TraitsType, CacheKey KeyT, CacheValue ValT, CacheStrategy CS, StorageGate SG>
    struct cache : public CS::template strategy<TraitsType, KeyT, ValT, SG> {
        using traits_type         = TraitsType;
        using key_type            = KeyT;
        using value_type          = ValT;
        using strategy_type       = typename CS::template strategy<TraitsType, KeyT, ValT, SG>;
        using optional_value_type = stl::optional<value_type>;
        using cache_result_type   = cache_result<cache>;

        // This bool will tell you if this cache supports direct pointer to the values or not
        static constexpr bool supports_direct_pointer = details::CacheStrategyPointerSupport<strategy_type>;

        // ctor
        using CS::template strategy<TraitsType, KeyT, ValT, SG>::strategy;

        template <CacheKey K>
            requires(stl::is_convertible_v<K, key_type>) // it's convertible to key
        constexpr cache_result_type operator[](K&& key) noexcept {
            return cache_result_type{*this, key, get(key)};
        }

        template <CacheKey K, CacheValue V>
            requires(stl::convertible_to<stl::remove_cvref_t<K>, key_type> && // it's a key
                     stl::convertible_to<stl::remove_cvref_t<V>, value_type>) // it's a value
        constexpr cache& set(K&& key, V&& value) {
            strategy_type::set(stl::forward<K>(key), stl::forward<V>(value));
            return *this;
        }

        template <CacheKey K, CacheValue V>
            requires(stl::is_convertible_v<K, key_type> && stl::is_convertible_v<V, value_type>)
        constexpr value_type get(K&& key, V&& default_value) {
            return strategy_type::get(stl::forward<K>(key)).value_or(stl::forward<V>(default_value));
        }

        template <CacheKey K>
            requires(stl::is_convertible_v<K, key_type>) // it's convertible to key
        constexpr optional_value_type get(K&& key) {
            return strategy_type::get(stl::forward<K>(key));
        }

        // Get a reference to the values, the life-time of the returned value becomes the problem in parallel
        // algorithms
        template <CacheKey K>
            requires(
              details::CacheStrategyPointerSupport<strategy_type> && stl::is_convertible_v<K, key_type>)
        constexpr auto* get_ptr(K&& key) {
            return strategy_type::get_ptr(stl::forward<K>(key));
        }

        /**
         * Get the value if exists, if not, construct one, and return the constructed one.
         */
        template <CacheKey K, typename... Args>
            requires(stl::is_convertible_v<K, key_type>) // it's convertible to key
        constexpr cache_result_type emplace_get(K&& key, Args&&... args) {
            if (auto val = strategy_type::get(key); val) {
                return cache_result_type{*this, stl::forward<K>(key), *val};
            }
            if constexpr (sizeof...(Args) == 1 &&
                          stl::same_as<value_type, stl::remove_cvref_t<istl::first_type_t<Args...>>>)
            {
                // if args... is value_type itself, no need to copy/move twice
                set(key, args...);
                return cache_result_type{*this, stl::forward<K>(key), stl::forward<Args>(args)...};
            } else {
                auto val = value_type{stl::forward<Args>(args)...};
                set(key, val);
                return cache_result_type{*this, stl::forward<K>(key), stl::move(val)};
            }
        }

        /**
         * Get the value if exists, if not, construct one, and return the constructed one.
         */
        template <CacheKey K, typename... Args>
            requires(stl::is_convertible_v<K, key_type>) // it's convertible to key
        constexpr auto* emplace_get_ptr(K&& key, Args&&... args) {
            if (auto* val = get_ptr(key); val) {
                return val;
            }
            if constexpr (sizeof...(Args) == 1 &&
                          stl::same_as<value_type, stl::remove_cvref_t<istl::first_type_t<Args...>>>)
            {
                // if args... is value_type itself, no need to copy/move twice
                set(key, args...);
            } else {
                set(key, value_type{stl::forward<Args>(args)...});
            }
            return get_ptr(key);
        }

        constexpr decltype(auto) begin() {
            if constexpr (istl::Iterable<strategy_type>) {
                return this->begin();
            } else {
                return this->get_gate().begin();
            }
        }

        constexpr decltype(auto) begin() const {
            if constexpr (istl::Iterable<strategy_type>) {
                return this->begin();
            } else {
                return this->get_gate().begin();
            }
        }

        constexpr decltype(auto) end() {
            if constexpr (istl::Iterable<strategy_type>) {
                return this->end();
            } else {
                return this->get_gate().end();
            }
        }

        constexpr decltype(auto) end() const {
            if constexpr (istl::Iterable<strategy_type>) {
                return this->end();
            } else {
                return this->get_gate().end();
            }
        }

        constexpr void clear() {
            if constexpr (requires { this->get_gate().clear(); }) {
                this->clear();
            } else {
                for (auto const& [key, _] : *this) {
                    this->erase(key);
                }
            }
        }
    };


} // namespace webpp

#endif // WEBPP_CACHE_CACHE_HPP
