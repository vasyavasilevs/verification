#include <iostream>
#include <unordered_map>
#include <mutex>
#include <stdexcept>
#include <vector>
#include <algorithm>

template <typename K, typename V>
class ThreadSafeCache {
private:
    size_t N; 
    size_t cacheSize;
    std::unordered_map<K, V> cache; 
    std::vector<K> insertion_order; 
    std::mutex mtx; 

public:
    ThreadSafeCache(size_t N, size_t cacheSize) : N(N), cacheSize(cacheSize) {}

    void store(const K& key, const V& value) {
        std::lock_guard<std::mutex> lock(mtx);
        auto it = cache.find(key);
        if (it == cache.end()) {
            if (cache.size() >= N) {
                throw std::overflow_error("Cache size exceeded.");
            }
            insertion_order.push_back(key);
        }
        cache[key] = value;
    }

    V load(const K& key) {
        std::lock_guard<std::mutex> lock(mtx);
        auto it = cache.find(key);
        if (it == cache.end()) {
            throw std::runtime_error("Key not found.");
        }
        return it->second;
    }

    bool is_empty() {
        std::lock_guard<std::mutex> lock(mtx);
        return cache.empty();
    }

    std::vector<K> iterate_keys() {
        std::lock_guard<std::mutex> lock(mtx);
        return insertion_order;
    }
};
