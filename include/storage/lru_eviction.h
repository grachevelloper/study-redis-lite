#pragma once

#include "storage/eviction_strategy.h"

class LRUEvictionStrategy final : public IEvictionStrategy {
public:
    std::string evict(const StorageSnapshot& snapshot) const override;
};
