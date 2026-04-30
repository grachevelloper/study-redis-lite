#pragma once

#include <cstdint>
#include <deque>
#include <string>
#include <unordered_map>

struct StorageSnapshot {
    const std::unordered_map<std::string, std::string>& data;
    const std::deque<std::string>& insertion_order;
    const std::unordered_map<std::string, std::uint64_t>& last_access;
};

class IEvictionStrategy {
public:
    virtual ~IEvictionStrategy() = default;
    virtual std::string evict(const StorageSnapshot& snapshot) const = 0;
};
