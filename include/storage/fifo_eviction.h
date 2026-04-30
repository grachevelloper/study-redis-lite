#pragma once

#include "storage/eviction_strategy.h"

class FIFOEvictionStrategy final : public IEvictionStrategy {
public:
    std::string evict(const StorageSnapshot& snapshot) const override;
};
