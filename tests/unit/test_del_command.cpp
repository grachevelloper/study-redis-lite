#include "commands/del_command.h"
#include "storage/storage.h"
#include "test_helpers.h"

TEST_F(StorageTestFixture, DelCommandRemovesExistingValue) {
    Storage::instance().set("name", "Ivan");
    DelCommand command("name");
    EXPECT_EQ(command.execute(), "OK");
    EXPECT_FALSE(Storage::instance().exists("name"));
}

TEST_F(StorageTestFixture, DelCommandReturnsNilForMissingValue) {
    DelCommand command("name");
    EXPECT_EQ(command.execute(), "(nil)");
}

TEST_F(StorageTestFixture, DelCommandUndoRestoresRemovedValue) {
    Storage::instance().set("name", "Ivan");
    DelCommand command("name");
    command.execute();
    command.undo();
    EXPECT_EQ(Storage::instance().get("name"), "Ivan");
}

TEST_F(StorageTestFixture, DelCommandUndoAfterMissingDeleteDoesNothing) {
    DelCommand command("name");
    command.execute();
    command.undo();
    EXPECT_FALSE(Storage::instance().exists("name"));
}

TEST_F(StorageTestFixture, DelCommandMutatesStorage) {
    DelCommand command("name");
    EXPECT_TRUE(command.mutates());
}
