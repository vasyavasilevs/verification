#include <gtest/gtest.h>
#include "ThreadSafeCache.h"

class CacheTest : public ::testing::Test {
protected:
    ThreadSafeCache<int, std::string> cache;

    CacheTest() : cache(5, 3) {}
};

TEST_F(CacheTest, TestStoreNewKey) {
    cache.store(1, "value1");
    EXPECT_EQ(cache.load(1), "value1");
}

TEST_F(CacheTest, TestStoreUpdateKey) {
    cache.store(1, "value1");
    cache.store(1, "updated_value1");
    EXPECT_EQ(cache.load(1), "updated_value1");
}

TEST_F(CacheTest, TestLoadNonExistentKey) {
    EXPECT_THROW(cache.load(2), std::runtime_error);
}

TEST_F(CacheTest, TestIsEmpty) {
    EXPECT_TRUE(cache.is_empty());
    cache.store(1, "value1");
    EXPECT_FALSE(cache.is_empty());
}

TEST_F(CacheTest, TestIterateKeys) {
    cache.store(1, "value1");
    cache.store(2, "value2");
    std::vector<int> keys = cache.iterate_keys();
    EXPECT_EQ(keys.size(), 2);
    EXPECT_EQ(keys[0], 1);
    EXPECT_EQ(keys[1], 2);
}
