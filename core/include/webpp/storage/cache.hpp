#ifndef WEBPP_CACHE_CACHE_HPP
#define WEBPP_CACHE_CACHE_HPP

#include "cache_concepts.hpp"


namespace webpp {


    /**
     * This class is mother of all caches.
     *
     * Cache Strategy: The type of cache; e.g. LRU, Expired LRU, ...
     * Storage Gate:   How the cache data is stored (and where); e.g. Memory, File, ...
     */
    template <Traits TraitsType, CacheKey KeyT, CacheValue ValT, CacheStrategy CS, StorageGate SG>
    struct cache : public CS::template strategy<TraitsType, KeyT, ValT, SG> {
        using traits_type   = TraitsType;
        using key_type      = traits::generalify_allocators<traits_type, KeyT>;
        using value_type    = traits::generalify_allocators<traits_type, ValT>;
        using strategy_type = typename CS::template strategy<TraitsType, KeyT, ValT, SG>;

        struct cache_result {};

        // ctor
        using CS::template strategy<TraitsType, KeyT, ValT, SG>::strategy;


        auto operator[](key_type key) {}


        template <CacheKey K, CacheValue V>
            requires(stl::is_convertible_v<K, key_type> && stl::is_convertible_v<V, value_type>)
        value_type get(K&& key, V&& default_value) {
            return strategy_type::get(stl::forward<K>(key)).value_or(stl::forward<V>(default_value));
        }


        template <CacheKey K>
            requires(stl::is_convertible_v<K, key_type>) // it's convertible to key
        stl::optional<value_type> get(K&& key) {
            return strategy_type::get(stl::forward<K>(key));
        }

        auto begin() {
            if constexpr (requires { this->begin(); }) {
                return this->begin();
            } else {
                return this->gate.begin();
            }
        }
        auto begin() const {
            if constexpr (requires { this->begin(); }) {
                return this->begin();
            } else {
                return this->gate.begin();
            }
        }
        auto end() {
            if constexpr (requires { this->end(); }) {
                return this->end();
            } else {
                return this->gate.end();
            }
        }
        auto end() const {
            if constexpr (requires { this->end(); }) {
                return this->end();
            } else {
                return this->gate.end();
            }
        }

        void clear() {
            if constexpr (requires { this->gate.clear(); }) {
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
