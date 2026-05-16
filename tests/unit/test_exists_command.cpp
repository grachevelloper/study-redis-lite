#include "commands/exists_command.h"
#include "storage/storage.h"
#include "test_helpers.h"

TEST_F(StorageTestFixture, ExistsCommandReturnsOneForExistingKey) {
    Storage::instance().set("name", "Ivan");
    ExistsCommand command("name");
    EXPECT_EQ(command.execute(), "1");
}

TEST_F(StorageTestFixture, ExistsCommandReturnsZeroForMissingKey) {
    ExistsCommand command("name");
    EXPECT_EQ(command.execute(), "0");
}

TEST_F(StorageTestFixture, ExistsCommandDoesNotMutateStorage) {
    ExistsCommand command("name");
    EXPECT_FALSE(command.mutates());
}

TEST_F(StorageTestFixture, ExistsCommandUndoDoesNothing) {
    Storage::instance().set("name", "Ivan");
    ExistsCommand command("name");
    command.execute();
    command.undo();
    EXPECT_EQ(Storage::instance().get("name"), "Ivan");
}

TEST_F(StorageTestFixture, ExistsCommandReflectsDeletion) {
    Storage::instance().set("name", "Ivan");
    Storage::instance().del("name");
    ExistsCommand command("name");
    EXPECT_EQ(command.execute(), "0");
}
