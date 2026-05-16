#include "commands/set_command.h"
#include "storage/storage.h"
#include "test_helpers.h"

TEST_F(StorageTestFixture, SetCommandExecuteStoresValue) {
    SetCommand command("name", "Ivan");
    EXPECT_EQ(command.execute(), "OK");
    EXPECT_EQ(Storage::instance().get("name"), "Ivan");
}

TEST_F(StorageTestFixture, SetCommandUndoRemovesNewKey) {
    SetCommand command("name", "Ivan");
    command.execute();
    command.undo();
    EXPECT_FALSE(Storage::instance().exists("name"));
}

TEST_F(StorageTestFixture, SetCommandUndoRestoresPreviousValue) {
    Storage::instance().set("name", "Old");
    SetCommand command("name", "New");
    command.execute();
    command.undo();
    EXPECT_EQ(Storage::instance().get("name"), "Old");
}

TEST_F(StorageTestFixture, SetCommandMutatesStorage) {
    SetCommand command("name", "Ivan");
    EXPECT_TRUE(command.mutates());
}

TEST_F(StorageTestFixture, SetCommandUndoBeforeExecuteIsSafe) {
    SetCommand command("name", "Ivan");
    command.undo();
    EXPECT_FALSE(Storage::instance().exists("name"));
}
