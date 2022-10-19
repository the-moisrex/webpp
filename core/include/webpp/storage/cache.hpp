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
        using traits_type         = TraitsType;
        using key_type            = traits::generalify_allocators<traits_type, KeyT>;
        using value_type          = traits::generalify_allocators<traits_type, ValT>;
        using strategy_type       = typename CS::template strategy<TraitsType, KeyT, ValT, SG>;
        using optional_value_type = stl::optional<value_type>;

        struct cache_result : optional_value_type {
          private:
            cache&   c;
            key_type the_key;

          public:
            constexpr cache_result(cache& input_cache, key_type key, optional_value_type&& val) noexcept
              : optional_value_type{stl::move(val)},
                c{input_cache},
                the_key{stl::move(key)} {}

            template <CacheValue V>
                requires(stl::is_convertible_v<V, value_type>)
            constexpr cache_result& operator=(V&& new_val) {
                c.set(the_key, stl::forward<V>(new_val));
                return *this;
            }


            constexpr cache_result& operator++() {
                static_assert(
                  requires(value_type v) { ++v; },
                  "You cannot run ++ operator on this value.");
                assert(*this); // make sure we do have a value
                c.set(the_key, ++this->value());
                return *this;
            }

            constexpr cache_result& operator--() {
                static_assert(
                  requires(value_type v) { --v; },
                  "You cannot run ++ operator on this value.");
                assert(*this); // make sure we do have a value
                c.set(the_key, ++this->value());
                return *this;
            }

            constexpr key_type key() const noexcept {
                return the_key;
            }
        };

        // ctor
        using CS::template strategy<TraitsType, KeyT, ValT, SG>::strategy;


        template <CacheKey K>
            requires(stl::is_convertible_v<K, key_type>) // it's convertible to key
        constexpr cache_result operator[](K&& key) noexcept {
            return cache_result{*this, key, get(key)};
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


        /**
         * Get the value if exists, if not, construct one, and return the constructed one.
         */
        template <CacheKey K, typename... Args>
            requires(stl::is_convertible_v<K, key_type>) // it's convertible to key
        constexpr value_type emplace_get(K&& key, Args&&... args) {
            if (auto val = strategy_type::get(key); val) {
                return *val;
            }
            set(stl::forward<K>(key), value_type{stl::forward<Args>(args)...});
            return strategy_type::get(key).value();
        }

        constexpr auto begin() {
            if constexpr (requires { this->begin(); }) {
                return this->begin();
            } else {
                return this->gate.begin();
            }
        }
        constexpr auto begin() const {
            if constexpr (requires { this->begin(); }) {
                return this->begin();
            } else {
                return this->gate.begin();
            }
        }
        constexpr auto end() {
            if constexpr (requires { this->end(); }) {
                return this->end();
            } else {
                return this->gate.end();
            }
        }
        constexpr auto end() const {
            if constexpr (requires { this->end(); }) {
                return this->end();
            } else {
                return this->gate.end();
            }
        }

        constexpr void clear() {
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
