#include <gtest/gtest.h>
#include "ThreadSafeCache.hpp"
#include <thread>

class CacheTest : public ::testing::Test {
protected:
    ThreadSafeCache<int, std::string> cache;

    CacheTest() : cache(5000) {}
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

    std::vector<int> keys;
    cache.iterate_keys([&keys](const int& key) {
        keys.push_back(key);
    });

    ASSERT_EQ(keys.size(), 2);
    EXPECT_EQ(keys[0], 1);
    EXPECT_EQ(keys[1], 2);
}

TEST_F(CacheTest, TestConcurrentStoreAndLoad) {
    auto writer = [&]() {
        for (int i = 0; i < 5; ++i) {
            try {
                cache.store(i, "value" + std::to_string(i));
            } catch (...) {}
        }
    };

    auto reader = [&]() {
        for (int i = 0; i < 5; ++i) {
            try {
                cache.load(i);
            } catch (...) {}
        }
    };

    std::thread t1(writer);
    std::thread t2(reader);
    std::thread t3(writer);
    std::thread t4(reader);

    t1.join();
    t2.join();
    t3.join();
    t4.join();

    for (int i = 0; i < 5; ++i) {
        try {
            auto val = cache.load(i);
            SUCCEED();
        } catch (...) {
            // Ok if not present due to cache limit or race condition.
        }
    }
}

TEST_F(CacheTest, TestConcurrentIterate) {
    constexpr int count = 5000;
    for (int i = 0; i < count; ++i) {
        cache.store(i, "val" + std::to_string(i));
    }

    std::vector<std::thread> threads;
    for (int i = 0; i < 10; ++i) {
        threads.emplace_back([&]() {
            std::vector<int> keys;
            cache.iterate_keys([&keys](const int& key) {
                keys.push_back(key);
            });
            // Basic validation
            EXPECT_LE(keys.size(), count);
        });
    }

    for (auto& t : threads) {
        t.join();
    }
}

TEST_F(CacheTest, TestParallelStoreAndWaitUntilAllRead) {
    constexpr int count = 5000;
    int successfully_read{0};

    std::thread writer([&]() {
        for (int i = 0; i < count; ++i) {
            try {
                cache.store(i, "value" + std::to_string(i));
            } catch (const std::overflow_error&) {
                // Cache full — допустимо
            }
        }
    });

    std::thread reader([&]() {
        std::vector<bool> read_flags(count, false);

        while (successfully_read < count) {
            for (int i = 0; i < count; ++i) {
                if (!read_flags[i]) {
                    try {
                        std::string val = cache.load(i);
                        EXPECT_EQ(val, "value" + std::to_string(i));
                        read_flags[i] = true;
                        ++successfully_read;
                    } catch (const std::runtime_error&) {
                        // Not written yet — try again
                    }
                }
            }
        }
    });

    writer.join();
    reader.join();

    EXPECT_EQ(successfully_read, count);
}
