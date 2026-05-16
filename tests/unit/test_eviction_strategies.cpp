#include "storage/eviction_strategy.h"
#include "storage/fifo_eviction.h"
#include "storage/lru_eviction.h"

#include <gtest/gtest.h>

TEST(EvictionStrategiesTest, FifoReturnsFirstPresentKey) {
    const std::unordered_map<std::string, std::string> data{{"a", "1"}, {"b", "2"}};
    const std::deque<std::string> insertion_order{"a", "b"};
    const std::unordered_map<std::string, std::uint64_t> access{{"a", 1}, {"b", 2}};
    const StorageSnapshot snapshot{data, insertion_order, access};

    FIFOEvictionStrategy strategy;
    EXPECT_EQ(strategy.evict(snapshot), "a");
}

TEST(EvictionStrategiesTest, FifoSkipsMissingKeysFromOrder) {
    const std::unordered_map<std::string, std::string> data{{"b", "2"}};
    const std::deque<std::string> insertion_order{"a", "b"};
    const std::unordered_map<std::string, std::uint64_t> access{{"b", 2}};
    const StorageSnapshot snapshot{data, insertion_order, access};

    FIFOEvictionStrategy strategy;
    EXPECT_EQ(strategy.evict(snapshot), "b");
}

TEST(EvictionStrategiesTest, FifoReturnsEmptyForEmptySnapshot) {
    const std::unordered_map<std::string, std::string> data{};
    const std::deque<std::string> insertion_order{};
    const std::unordered_map<std::string, std::uint64_t> access{};
    const StorageSnapshot snapshot{data, insertion_order, access};

    FIFOEvictionStrategy strategy;
    EXPECT_TRUE(strategy.evict(snapshot).empty());
}

TEST(EvictionStrategiesTest, LruReturnsLeastRecentlyUsedKey) {
    const std::unordered_map<std::string, std::string> data{{"a", "1"}, {"b", "2"}, {"c", "3"}};
    const std::deque<std::string> insertion_order{"a", "b", "c"};
    const std::unordered_map<std::string, std::uint64_t> access{{"a", 3}, {"b", 1}, {"c", 2}};
    const StorageSnapshot snapshot{data, insertion_order, access};

    LRUEvictionStrategy strategy;
    EXPECT_EQ(strategy.evict(snapshot), "b");
}

TEST(EvictionStrategiesTest, LruTreatsMissingAccessAsOldest) {
    const std::unordered_map<std::string, std::string> data{{"a", "1"}, {"b", "2"}};
    const std::deque<std::string> insertion_order{"a", "b"};
    const std::unordered_map<std::string, std::uint64_t> access{{"b", 5}};
    const StorageSnapshot snapshot{data, insertion_order, access};

    LRUEvictionStrategy strategy;
    EXPECT_EQ(strategy.evict(snapshot), "a");
}
