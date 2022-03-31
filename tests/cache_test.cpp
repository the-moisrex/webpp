#include "../core/include/webpp/storage/lru_cache.hpp"
#include "common_pch.hpp"

using namespace webpp;


TEST(Cache, LEUCacheTest) {
    lru_cache c;
    c.set("one", "value");
    EXPECT_EQ("value", c.get("one", ""));
}
