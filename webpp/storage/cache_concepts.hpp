#ifndef WEBPP_STORAGE_CACHE_CONCEPTS_HPP
#define WEBPP_STORAGE_CACHE_CONCEPTS_HPP

#include "../convert/lexical_cast.hpp"
#include "../std/type_traits.hpp"
#include "../traits/default_traits.hpp"
#include "null_gate.hpp"

namespace webpp {


    template <typename K>
    concept CacheKey = !stl::is_void_v<K>;

    template <typename V>
    concept CacheValue = !stl::is_void_v<V>;

    template <typename T>
    concept CacheOptions = !stl::is_void_v<T> && stl::is_copy_assignable_v<T>;

    template <typename K>
    concept CacheFileKey = CacheKey<K> && lexical::CastableTo<K, stl::string> &&
                           lexical::CastableTo<stl::string, K> && requires(K key) { stl::hash<K>{}(key); };

    template <typename V>
    concept CacheFileValue =
      CacheValue<V> && lexical::CastableTo<V, stl::string> && lexical::CastableTo<stl::string, V>;

    template <typename T>
    concept CacheFileOptions =
      CacheOptions<T> && lexical::CastableTo<T, stl::string> && lexical::CastableTo<stl::string, T>;

    namespace details {

        // I extracted this function from StorageGateType because of GCC segfault crash.
        // Seems like we can't use lambdas in concepts in GCC 12.1.0, it's been resolved in 12.2.1.
        struct if_condition {
            [[nodiscard]] constexpr bool operator()(auto const&) const noexcept {
                return true;
            }
        };

        template <typename S>
        concept StorageGateType = requires(S gate) {
            typename S::key_type;
            typename S::value_type;
            typename S::options_type;
            typename S::bundle_type;
            requires CacheKey<typename S::key_type>;
            requires CacheValue<typename S::value_type>;
            requires CacheOptions<typename S::options_type>;
            typename S::traits_type;
            requires Traits<typename S::traits_type>;

            requires requires(typename S::key_type     key,
                              typename S::value_type   value,
                              typename S::options_type opts,
                              if_condition             condition) {
                gate.erase(key);
                gate.set(key, value, opts);
                gate.set_options(key, opts);
                {
                    gate.get(key)
                } -> stl::same_as<stl::optional<typename S::bundle_type>>;

                // I added the erase_if here and not in the "cache" because
                // it might be faster (I think)
                // todo: check if we really need erase_if here
                gate.erase_if(condition);
            };
        };

        /**
         * This concept accepts the StorageGate types that support references as well;
         * for example the in-memory caches can give you a reference of what they're holding.
         * The life-time management of this is a little tricky and should be avoided unless you know what
         * you're doing.
         */
        template <typename T>
        concept StorageGatePointerSupport = requires {
            typename T::key_ptr_type;
            typename T::value_ptr_type;
            typename T::options_ptr_type;
            typename T::bundle_ptr_type;

            requires requires(T                        gate,
                              typename T::key_type     key,
                              typename T::value_type   value,
                              typename T::options_type opts) {
                {
                    gate.get_ptr(key)
                } -> stl::same_as<stl::optional<typename T::bundle_ptr_type>>;
            };
        };


        template <typename T>
        concept CacheStrategy = requires {
            typename T::key_type;
            typename T::value_type;
            requires CacheKey<typename T::key_type>;
            requires CacheValue<typename T::value_type>;
            typename T::traits_type;
            typename T::storage_gate_type;
            requires requires(T st, typename T::key_type key, typename T::value_type value) {
                st.set(key, value);
                {
                    st.get(key)
                } -> stl::same_as<stl::optional<typename T::value_type>>;
            };
        };


        template <typename T>
        concept CacheStrategyPointerSupport = requires {
            requires StorageGatePointerSupport<typename T::storage_gate_type>;
            requires requires(T st, typename T::key_type key) { st.get_ptr(key); };
        };

    } // namespace details

    template <typename T>
    concept StorageGate = requires {
        typename T::template storage_gate<default_traits, int, int, int>;
        requires details::StorageGateType<typename T::template storage_gate<default_traits, int, int, int>>;
    };


    template <typename T>
    concept CacheStrategy = requires {
        typename T::template strategy<default_traits, int, int, null_gate>;
        requires details::CacheStrategy<typename T::template strategy<default_traits, int, int, null_gate>>;
    };

    template <typename KeyT, typename ValueT, typename OptionsT>
    struct cache_tuple {
        KeyT     key;
        ValueT   value;
        OptionsT options;
    };

} // namespace webpp

#endif // WEBPP_STORAGE_CACHE_CONCEPTS_HPP
