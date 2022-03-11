#ifndef WEBPP_STORAGE_LRU_CACHE_HPP
#define WEBPP_STORAGE_LRU_CACHE_HPP

#include "../traits/traits.hpp"

namespace webpp {

    template <Traits TraitsType, typename KeyT, typename ValueT>
    struct basic_lru_cache {
        using key_type   = KeyT;
        using value_type = ValueT;
    };

} // namespace webpp

#endif // WEBPP_STORAGE_LRU_CACHE_HPP
