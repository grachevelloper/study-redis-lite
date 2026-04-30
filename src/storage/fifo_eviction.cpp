#include "storage/fifo_eviction.h"

std::string FIFOEvictionStrategy::evict(const StorageSnapshot& snapshot) const {
    for (const auto& key : snapshot.insertion_order) {
        if (snapshot.data.find(key) != snapshot.data.end()) {
            return key;
        }
    }
    return {};
}
