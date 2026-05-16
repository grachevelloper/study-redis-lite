#include "storage/fifo_eviction.h"
#include "storage/lru_eviction.h"
#include "storage/storage.h"
#include "test_helpers.h"

#include <memory>

TEST_F(StorageTestFixture, SetReturnsNoOldValueForNewKey) {
    const auto old_value = Storage::instance().set("name", "Ivan");
    EXPECT_FALSE(old_value.has_value());
    EXPECT_EQ(Storage::instance().get("name"), "Ivan");
}

TEST_F(StorageTestFixture, SetReturnsOldValueForExistingKey) {
    Storage::instance().set("name", "Ivan");
    const auto old_value = Storage::instance().set("name", "Maria");
    ASSERT_TRUE(old_value.has_value());
    EXPECT_EQ(*old_value, "Ivan");
    EXPECT_EQ(Storage::instance().get("name"), "Maria");
}

TEST_F(StorageTestFixture, GetReturnsNulloptForMissingKey) {
    EXPECT_FALSE(Storage::instance().get("missing").has_value());
}

TEST_F(StorageTestFixture, ExistsReflectsStorageState) {
    EXPECT_FALSE(Storage::instance().exists("name"));
    Storage::instance().set("name", "Ivan");
    EXPECT_TRUE(Storage::instance().exists("name"));
    Storage::instance().del("name");
    EXPECT_FALSE(Storage::instance().exists("name"));
}

TEST_F(StorageTestFixture, DelReturnsOldValueAndRemovesKey) {
    Storage::instance().set("name", "Ivan");
    const auto removed = Storage::instance().del("name");
    ASSERT_TRUE(removed.has_value());
    EXPECT_EQ(*removed, "Ivan");
    EXPECT_FALSE(Storage::instance().exists("name"));
}

TEST_F(StorageTestFixture, DelReturnsNulloptForMissingKey) {
    EXPECT_FALSE(Storage::instance().del("missing").has_value());
}

TEST_F(StorageTestFixture, SubscribeReceivesNotificationsForSetAndDelete) {
    auto observer = std::make_shared<RecordingObserver>();
    Storage::instance().subscribe("name", observer);

    Storage::instance().set("name", "Ivan");
    Storage::instance().del("name");

    ASSERT_EQ(observer->notifications.size(), 2U);
    EXPECT_EQ(observer->notifications[0], "name=Ivan");
    EXPECT_EQ(observer->notifications[1], "name=(nil)");
}

TEST_F(StorageTestFixture, UnsubscribeAllStopsNotifications) {
    auto observer = std::make_shared<RecordingObserver>();
    Storage::instance().subscribe("name", observer);
    Storage::instance().unsubscribe_all(observer.get());

    Storage::instance().set("name", "Ivan");
    EXPECT_TRUE(observer->notifications.empty());
}

TEST_F(StorageTestFixture, FifoEvictionRemovesOldestInsertedKey) {
    Storage::instance().set_eviction_strategy(std::make_unique<FIFOEvictionStrategy>());
    Storage::instance().set_max_keys(2);

    Storage::instance().set("a", "1");
    Storage::instance().set("b", "2");
    Storage::instance().set("c", "3");

    EXPECT_FALSE(Storage::instance().exists("a"));
    EXPECT_TRUE(Storage::instance().exists("b"));
    EXPECT_TRUE(Storage::instance().exists("c"));
}

TEST_F(StorageTestFixture, LruEvictionRespectsRecentReads) {
    Storage::instance().set_eviction_strategy(std::make_unique<LRUEvictionStrategy>());
    Storage::instance().set_max_keys(2);

    Storage::instance().set("a", "1");
    Storage::instance().set("b", "2");
    EXPECT_EQ(Storage::instance().get("a"), "1");
    Storage::instance().set("c", "3");

    EXPECT_TRUE(Storage::instance().exists("a"));
    EXPECT_FALSE(Storage::instance().exists("b"));
    EXPECT_TRUE(Storage::instance().exists("c"));
}
