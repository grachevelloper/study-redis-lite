#include "storage/lru_eviction.h"

#include <limits>

std::string LRUEvictionStrategy::evict(const StorageSnapshot& snapshot) const {
    std::string candidate;
    std::uint64_t oldest_access = std::numeric_limits<std::uint64_t>::max();

    for (const auto& item : snapshot.data) {
        const auto access_it = snapshot.last_access.find(item.first);
        const auto access = access_it == snapshot.last_access.end() ? 0 : access_it->second;
        if (access < oldest_access) {
            oldest_access = access;
            candidate = item.first;
        }
    }

    return candidate;
}
