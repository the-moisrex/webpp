#include "../core/include/webpp/storage/cache.hpp"

#include "../core/include/webpp/storage/lru_cache.hpp"
#include "common_pch.hpp"

using namespace webpp;

static_assert(CacheKey<int>);
static_assert(!CacheKey<void>);
static_assert(CacheValue<double>);
static_assert(StorageGate<null_gate>);
static_assert(StorageGate<file_gate>);
static_assert(StorageGate<memory_gate<null_gate>>);

TEST(Cache, LEUCacheTest) {
    enable_owner_traits<default_traits> t;
    lru_cache<>                         c(t);
    c.set("one", "value");
    EXPECT_EQ("value", c.get("one", ""));
}
