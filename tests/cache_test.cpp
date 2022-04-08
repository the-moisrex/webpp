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
    c.set("one", "new value");
    EXPECT_EQ("new value", c.get("one", ""));
    c.set("one", "old value");
    EXPECT_EQ("old value", c.get("one", ""));

    lru_cache<default_traits, int> c2{t, 3};
    c2.set(1, "hello");
    c2.set(1, "hello 2");
    EXPECT_EQ("hello 2", c2.get(1).value());
    c2.set(2, "two");
    c2.set(3, "three");
    c2.set(4, "four");
    c2.set(5, "five");
    EXPECT_TRUE(!c2.get(1));
    EXPECT_TRUE(!c2.get(2));
    EXPECT_TRUE(!!c2.get(3));
    EXPECT_TRUE(!!c2.get(4));
    EXPECT_TRUE(!!c2.get(5));
}
