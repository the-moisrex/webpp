#ifndef WEBPP_UNIFIED_CACHE_CACHE_H
#define WEBPP_UNIFIED_CACHE_CACHE_H

#include "../std/type_traits.hpp"

#include <utility>

namespace webpp {

    /**
     * This class is designed to contain other caching systems inside it.
     * Its purpose is to have a unified caching system for different types.
     * You can use different caching system for different key and values
     * You even can use different caching methods available in the wild with
     * this class.
     *
     * You probably want to use this class instead of individual classes out
     * there.
     *
     */
    template <typename... CacheSystem>
    class unified_caches : public CacheSystem... {
      private:
      public:
        unified_caches() noexcept = default;

        //
        // template <typename KeyType, typename ValueType>
        // auto set(KeyType&& key, ValueType&& value) noexcept {
        // static_assert(
        // can_use_as_key<KeyType>(),
        // "You cannot use this key type in this caching system. You can "
        // "add a new template parameter to the caching system store in "
        // "order to be able to use this key type here.");
        // static_assert(
        // can_use_as_value<ValueType>(),
        // "You can't use this value type in this caching system. Add a "
        // "new template parameter to the caching system store in order "
        // "to be able to store this value here.");
        // }
        //
        // template <typename KeyType, typename ValueType>
        // auto get(KeyType&& key, ValueType default_value) noexcept {
        // static_assert(
        // can_use_as_key<KeyType>(),
        // "You cannot use this key type in this caching system. You can "
        // "add a new template parameter to the caching system store in "
        // "order to be able to use this key type here.");
        // }
        //


        // ------------------------- static methods -------------------------

        /**
         * This method check if you can use the specified key_type as a key in
         * this unified caching system or you have to add a new caching system
         * to this class before you can use this value.
         */
        template <typename KeyType>
        static constexpr bool can_use_as_key() noexcept {
            return (std::is_convertible_v<KeyType, typename CacheSystem::key_type> || ...);
        }

        /**
         * This method checks if you can use the specified value type as a value
         * in this unified caching system, or you have to add a new caching
         * system in this caching system's template to be able to use this value
         * as a value type in the caching system.
         */
        template <typename ValueType>
        static constexpr bool can_use_as_value() noexcept {
            return (std::is_convertible_v<ValueType, typename CacheSystem::value_type> || ...);
        }
    };


} // namespace webpp

#endif // WEBPP_UNIFIED_CACHE_CACHE_H
