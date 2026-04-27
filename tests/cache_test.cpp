#include "../webpp/storage/cache.hpp"

#include "../webpp/storage/directory_gate.hpp"
#include "../webpp/storage/file_gate.hpp"
#include "../webpp/storage/lru_cache.hpp"
#include "common/test.hpp"

using namespace webpp;

static_assert(CacheKey<int>);
static_assert(!CacheKey<void>);
static_assert(CacheValue<double>);
static_assert(StorageGate<null_gate>);
static_assert(StorageGate<directory_gate>);
static_assert(StorageGate<file_gate>);
static_assert(StorageGate<memory_gate<null_gate>>);
static_assert(!Allocator<int>);

// NOLINTBEGIN(*-magic-numbers)

TEST(Cache, LRUCacheTest) {
    lru_cache<> cache;
    cache.set("one", "value");
    EXPECT_EQ("value", cache.get("one", ""));
    cache.set("one", "new value");
    EXPECT_EQ("new value", cache.get("one", ""));
    cache.set("one", "old value");
    EXPECT_EQ("old value", cache.get("one", ""));

    lru_cache<int> cache2{trs, 3};
    cache2.set(1, "hello");
    cache2.set(1, "hello 2");
    EXPECT_EQ("hello 2", cache2.get(1).value());
    cache2.set(2, "two");
    cache2.set(3, "three");
    cache2.set(4, "four");
    cache2.set(5, "five");
    EXPECT_TRUE(!cache2.get(1));
    EXPECT_TRUE(!cache2.get(2));
    EXPECT_TRUE(!!cache2.get(3));
    EXPECT_TRUE(!!cache2.get(4));
    EXPECT_TRUE(!!cache2.get(5));

    for (auto const& [key, value] : cache2) {
        EXPECT_TRUE(key < 10);
    }
}

TEST(Cache, CacheResultTest) {
    lru_cache<stl::string, int> cache;
    cache["one"] = 1;
    cache["two"] = 2;

    EXPECT_EQ(cache["one"].value(), 1);
    EXPECT_EQ(cache["two"].value(), 2);
    EXPECT_EQ(cache["one"].key(), "one");
}

TEST(Cache, DirectoryGateTest) {
    auto dir  = stl::filesystem::temp_directory_path();
    dir      /= "webpp-directory-gate-test";
    stl::filesystem::create_directory(dir);
    lru_cache<std::string, std::string, directory_gate> c(1024, dir, "one");
    c.set("one", "value");
    EXPECT_EQ("value", c.get("one", "default"));
    c.set("one", "new value");
    EXPECT_EQ("new value", c.get("one", "default"));
    c.set("one", "old value");
    EXPECT_EQ("old value", c.get("one", "default"));

    lru_cache<int, std::string, directory_gate> cache2{trs, 3, dir, "two"};
    cache2.set(1, "hello");
    cache2.set(1, "hello 2");
    EXPECT_EQ("hello 2", cache2.get(1).value());
    cache2.set(2, "two");
    cache2.set(3, "three");
    cache2.set(4, "four");
    cache2.set(5, "five");
    EXPECT_TRUE(!cache2.get(1));
    EXPECT_TRUE(!cache2.get(2));
    EXPECT_TRUE(!!cache2.get(3));
    EXPECT_TRUE(!!cache2.get(4));
    EXPECT_TRUE(!!cache2.get(5));

    for (auto const& [key, value] : cache2) {
        try {
            EXPECT_TRUE(key < 10) << key << value;
        } catch (stl::bad_alloc const& err) {
            // I'm doing this to get the key when it fails. (the issue is fixed now)
#ifdef EXPECT_NO_THROW
            EXPECT_NO_THROW(throw err) << key;
#endif
        }
    }

    stl::filesystem::remove_all(dir);
}

TEST(Cache, ReferenceTest) {
    lru_cache<> cache;
    cache.set("one", "value");
    EXPECT_EQ("value", *cache.get_ptr("one"));
    cache.set("one", "new value");
    EXPECT_EQ("new value", *cache.get_ptr("one"));
    auto& val_ref = *cache.get_ptr("one");
    val_ref       = "new new value";
    EXPECT_EQ("new new value", cache.get("one").value());
}




namespace fs = std::filesystem;

// Mocking required web++ concepts/types for the test to compile
namespace webpp {
    struct dummy_options {};

    template <typename K, typename V, typename O>
    struct cache_tuple {
        K key;
        V value;
        O options;
    };

    namespace lexical {
        template <typename T, typename U>
        T cast(U const& u) {
            return T(u);
        }
    } // namespace lexical
} // namespace webpp

class FileGateTest : public ::testing::Test {
  protected:
    fs::path temp_dir;

    void SetUp() override {
        temp_dir = fs::temp_directory_path() / "webpp_test_cache";
        fs::create_directories(temp_dir);
    }

    void TearDown() override {
        fs::remove_all(temp_dir);
    }
};

// Using a concrete instantiation for testing
using TestGate = webpp::file_gate::storage_gate<std::string, std::string, webpp::dummy_options>;

TEST_F(FileGateTest, SetAndGetSuccessfully) {
    TestGate gate(temp_dir);

    gate.set("my_key", "my_value");
    auto result = gate.get("my_key");

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->key, "my_key");
    EXPECT_EQ(result->value, "my_value");
}

TEST_F(FileGateTest, GetNonExistentKeyReturnsNullopt) {
    TestGate gate(temp_dir);
    auto     result = gate.get("does_not_exist");
    EXPECT_FALSE(result.has_value());
}

TEST_F(FileGateTest, EraseRemovesKey) {
    TestGate gate(temp_dir);
    gate.set("delete_me", "data");
    EXPECT_TRUE(gate.get("delete_me").has_value());

    gate.erase("delete_me");
    EXPECT_FALSE(gate.get("delete_me").has_value());
}

TEST_F(FileGateTest, EraseIfRemovesMatchingKeys) {
    TestGate gate(temp_dir);
    gate.set("keep_me", "data1");
    gate.set("drop_me", "data2");

    gate.erase_if([](auto const& bundle) {
        return bundle.key == "drop_me";
    });

    EXPECT_TRUE(gate.get("keep_me").has_value());
    EXPECT_FALSE(gate.get("drop_me").has_value());
}

TEST_F(FileGateTest, HandlesEmptyFileGracefully) {
    TestGate gate(temp_dir);
    // Simulate corrupted empty file
    auto hash = std::hash<std::string>{}("corrupt_key");
    std::ofstream(temp_dir / std::to_string(hash));

    auto result = gate.get("corrupt_key");
    EXPECT_FALSE(result.has_value()); // Should fail cleanly, not crash
}

// NOLINTEND(*-magic-numbers)
