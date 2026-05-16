#include "storage/storage.h"

#include "storage/lru_eviction.h"

#include <algorithm>

Storage& Storage::instance() {
    static Storage storage;
    return storage;
}

Storage::Storage()
    : eviction_strategy_(std::make_unique<LRUEvictionStrategy>()) {
}

void Storage::set_eviction_strategy(std::unique_ptr<IEvictionStrategy> strategy) {
    std::lock_guard<std::mutex> lock(mutex_);
    eviction_strategy_ = std::move(strategy);
}

void Storage::set_max_keys(std::size_t max_keys) {
    std::lock_guard<std::mutex> lock(mutex_);
    max_keys_ = max_keys;
    evict_if_needed_locked();
}

std::optional<std::string> Storage::get(const std::string& key) {
    std::lock_guard<std::mutex> lock(mutex_);
    const auto it = data_.find(key);
    if (it == data_.end()) {
        return std::nullopt;
    }

    touch_locked(key);
    return it->second;
}

bool Storage::exists(const std::string& key) {
    std::lock_guard<std::mutex> lock(mutex_);
    const auto found = data_.find(key) != data_.end();
    if (found) {
        touch_locked(key);
    }
    return found;
}

std::optional<std::string> Storage::set(const std::string& key, const std::string& value) {
    std::optional<std::string> old_value;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        const auto it = data_.find(key);
        if (it == data_.end()) {
            insertion_order_.push_back(key);
        } else {
            old_value = it->second;
        }

        data_[key] = value;
        touch_locked(key);
        evict_if_needed_locked();
    }

    notify_observers(key, value);
    return old_value;
}

std::optional<std::string> Storage::del(const std::string& key) {
    std::optional<std::string> old_value;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        const auto it = data_.find(key);
        if (it == data_.end()) {
            return std::nullopt;
        }

        old_value = it->second;
        data_.erase(it);
        last_access_.erase(key);
        remove_from_order_locked(key);
    }

    notify_observers(key, "(nil)");
    return old_value;
}

void Storage::subscribe(const std::string& key, const std::shared_ptr<IObserver>& observer) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto& observers = observers_[key];
    const auto already_subscribed = std::any_of(
        observers.begin(),
        observers.end(),
        [&](const std::weak_ptr<IObserver>& current) {
            const auto locked = current.lock();
            return locked && locked.get() == observer.get();
        });

    if (!already_subscribed) {
        observers.push_back(observer);
    }
}

void Storage::unsubscribe_all(IObserver* observer) {
    std::lock_guard<std::mutex> lock(mutex_);
    for (auto& item : observers_) {
        auto& observers = item.second;
        observers.erase(
            std::remove_if(
                observers.begin(),
                observers.end(),
                [&](const std::weak_ptr<IObserver>& current) {
                    const auto locked = current.lock();
                    return !locked || locked.get() == observer;
                }),
            observers.end());
    }
}

void Storage::reset_for_tests() {
    std::lock_guard<std::mutex> lock(mutex_);
    data_.clear();
    observers_.clear();
    insertion_order_.clear();
    last_access_.clear();
    eviction_strategy_ = std::make_unique<LRUEvictionStrategy>();
    max_keys_ = 1000;
    access_clock_ = 0;
}

void Storage::touch_locked(const std::string& key) {
    last_access_[key] = ++access_clock_;
}

void Storage::remove_from_order_locked(const std::string& key) {
    insertion_order_.erase(
        std::remove(insertion_order_.begin(), insertion_order_.end(), key),
        insertion_order_.end());
}

void Storage::evict_if_needed_locked() {
    while (data_.size() > max_keys_ && eviction_strategy_) {
        const StorageSnapshot snapshot{data_, insertion_order_, last_access_};
        const auto key_to_remove = eviction_strategy_->evict(snapshot);
        if (key_to_remove.empty()) {
            return;
        }

        data_.erase(key_to_remove);
        last_access_.erase(key_to_remove);
        remove_from_order_locked(key_to_remove);
    }
}

void Storage::notify_observers(const std::string& key, const std::string& value) {
    std::vector<std::shared_ptr<IObserver>> observers;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        const auto it = observers_.find(key);
        if (it == observers_.end()) {
            return;
        }
        auto& stored = it->second;
        stored.erase(
            std::remove_if(
                stored.begin(),
                stored.end(),
                [&](const std::weak_ptr<IObserver>& current) {
                    auto locked = current.lock();
                    if (!locked) {
                        return true;
                    }
                    observers.push_back(std::move(locked));
                    return false;
                }),
            stored.end());
    }

    for (const auto& observer : observers) {
        if (observer) {
            observer->notify(key, value);
        }
    }
}
