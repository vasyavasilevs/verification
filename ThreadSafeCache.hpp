#include <iostream>
#include <unordered_map>
#include <mutex>
#include <optional>
#include <stdexcept>
#include <vector>

template <typename K, typename V>
class ThreadSafeCache {
private:
    size_t N; 
    size_t K;
    std::unordered_map<K, V> cache; 
    std::mutex mtx; 

public:
    ThreadSafeCache(size_t N, size_t K) : N(N), K(K) {}

    void store(const K& key, const V& value) {
        std::lock_guard<std::mutex> lock(mtx);
        if (cache.size() < N || cache.find(key) != cache.end()) {
            cache[key] = value;
        } else {
            throw std::overflow_error("Cache size exceeded.");
        }
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
        std::vector<K> keys;
        for (const auto& pair : cache) {
            keys.push_back(pair.first);
        }
        return keys;
    }
};
