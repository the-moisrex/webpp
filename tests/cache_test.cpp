#include "../webpp/storage/cache.hpp"

#include "../webpp/storage/directory_gate.hpp"
#include "../webpp/storage/file_gate.hpp"
#include "../webpp/storage/lru_cache.hpp"
#include "common/tests_common_pch.hpp"

using namespace webpp;

static_assert(CacheKey<int>);
static_assert(!CacheKey<void>);
static_assert(CacheValue<double>);
static_assert(StorageGate<null_gate>);
static_assert(StorageGate<directory_gate>);
static_assert(StorageGate<file_gate>);
static_assert(StorageGate<memory_gate<null_gate>>);
static_assert(!Allocator<int>);
using replacer = alloc::details::allocator_replacer<std::allocator>::template replacer<int>;
static_assert(!replacer::value);
static_assert(stl::is_same_v<int, typename replacer::type>);
static_assert(stl::is_same_v<traits::generalify_allocators<default_traits, int>, int>);
static_assert(stl::is_same_v<traits::generalify_allocators<default_traits, double>, double>);
static_assert(stl::is_same_v<traits::generalify_allocators<std_traits, std::string_view>, std::string_view>);

// NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers)

TEST(Cache, LRUCacheTest) {
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

    for (auto const& [key, value] : c2) {
        EXPECT_TRUE(key < 10);
    }
}

TEST(Cache, CacheResultTest) {
    enable_owner_traits<default_traits>         t;
    lru_cache<default_traits, stl::string, int> c(t);
    c["one"] = 1;
    c["two"] = 2;

    EXPECT_EQ(c["one"].value(), 1);
    EXPECT_EQ(c["two"].value(), 2);
    EXPECT_EQ(c["one"].key(), "one");
}


TEST(Cache, DirectoryGateTest) {
    enable_owner_traits<default_traits> t;
    auto                                dir = stl::filesystem::temp_directory_path();
    dir /= "webpp-directory-gate-test";
    stl::filesystem::create_directory(dir);
    lru_cache<default_traits, std::string, std::string, directory_gate> c(t, 1024, dir, "one");
    c.set("one", "value");
    EXPECT_EQ("value", c.get("one", "default"));
    c.set("one", "new value");
    EXPECT_EQ("new value", c.get("one", "default"));
    c.set("one", "old value");
    EXPECT_EQ("old value", c.get("one", "default"));

    lru_cache<default_traits, int, std::string, directory_gate> c2{t, 3, dir, "two"};
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

    for (auto const& [key, value] : c2) {
        try {
            EXPECT_TRUE(key < 10) << key << istl::to_std_string(value);
        } catch (stl::bad_alloc const& b) {
            // I'm doing this to get the key when it fails. (the issue is fixed now)
            EXPECT_NO_THROW(throw b) << key;
        }
    }

    stl::filesystem::remove_all(dir);
}


TEST(Cache, ReferenceTest) {
    enable_owner_traits<default_traits> t;
    lru_cache<>                         c(t);
    c.set("one", "value");
    EXPECT_EQ("value", *c.get_ptr("one"));
    c.set("one", "new value");
    EXPECT_EQ("new value", *c.get_ptr("one"));
    auto& val_ref = *c.get_ptr("one");
    val_ref       = "new new value";
    EXPECT_EQ("new new value", c.get("one").value());
}

// NOLINTEND(cppcoreguidelines-avoid-magic-numbers)
