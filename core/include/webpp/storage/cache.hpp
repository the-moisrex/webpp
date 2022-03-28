#ifndef WEBPP_CACHE_CACHE_H
#define WEBPP_CACHE_CACHE_H

#include "cache_concepts.hpp"


namespace webpp {

    template <typename KeyT, typename ValT, CacheStrategy CS>
    struct cache {
        using key_type   = KeyT;
        using value_type = ValT;
    };

    template <typename CacheSystem>
    auto set(CacheSystem&                       cache_system,
             typename CacheSystem::key_type&&   key,
             typename CacheSystem::value_type&& value) {
        return cache_system.set(std::forward<typename CacheSystem::key_type>(key),
                                std::forward<typename CacheSystem::value_type>(value));
    }

} // namespace webpp

#endif
