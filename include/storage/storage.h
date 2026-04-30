#pragma once

#include "storage/eviction_strategy.h"
#include "storage/observer.h"

#include <cstdint>
#include <deque>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

class Storage {
public:
    static Storage& instance();

    Storage(const Storage&) = delete;
    Storage& operator=(const Storage&) = delete;

    void set_eviction_strategy(std::unique_ptr<IEvictionStrategy> strategy);
    void set_max_keys(std::size_t max_keys);

    std::optional<std::string> get(const std::string& key);
    bool exists(const std::string& key);
    std::optional<std::string> set(const std::string& key, const std::string& value);
    std::optional<std::string> del(const std::string& key);

    void subscribe(const std::string& key, const std::shared_ptr<IObserver>& observer);
    void unsubscribe_all(IObserver* observer);

private:
    Storage();

    void touch_locked(const std::string& key);
    void remove_from_order_locked(const std::string& key);
    void evict_if_needed_locked();
    void notify_observers(const std::string& key, const std::string& value);

    std::mutex mutex_;
    std::unordered_map<std::string, std::string> data_;
    std::unordered_map<std::string, std::vector<std::weak_ptr<IObserver>>> observers_;
    std::deque<std::string> insertion_order_;
    std::unordered_map<std::string, std::uint64_t> last_access_;
    std::unique_ptr<IEvictionStrategy> eviction_strategy_;
    std::size_t max_keys_ = 1000;
    std::uint64_t access_clock_ = 0;
};
