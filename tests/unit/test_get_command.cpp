#include "commands/get_command.h"
#include "storage/storage.h"
#include "test_helpers.h"

TEST_F(StorageTestFixture, GetCommandReturnsStoredValue) {
    Storage::instance().set("name", "Ivan");
    GetCommand command("name");
    EXPECT_EQ(command.execute(), "Ivan");
}

TEST_F(StorageTestFixture, GetCommandReturnsNilForMissingKey) {
    GetCommand command("name");
    EXPECT_EQ(command.execute(), "(nil)");
}

TEST_F(StorageTestFixture, GetCommandDoesNotMutateStorage) {
    GetCommand command("name");
    EXPECT_FALSE(command.mutates());
}

TEST_F(StorageTestFixture, GetCommandUndoDoesNothing) {
    Storage::instance().set("name", "Ivan");
    GetCommand command("name");
    command.execute();
    command.undo();
    EXPECT_EQ(Storage::instance().get("name"), "Ivan");
}

TEST_F(StorageTestFixture, GetCommandReflectsDeletion) {
    Storage::instance().set("name", "Ivan");
    Storage::instance().del("name");
    GetCommand command("name");
    EXPECT_EQ(command.execute(), "(nil)");
}
