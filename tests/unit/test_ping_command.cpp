#include "commands/ping_command.h"

#include <gtest/gtest.h>

TEST(PingCommandTest, ExecuteReturnsPong) {
    PingCommand command;
    EXPECT_EQ(command.execute(), "PONG");
}

TEST(PingCommandTest, MutatesReturnsFalse) {
    PingCommand command;
    EXPECT_FALSE(command.mutates());
}

TEST(PingCommandTest, UndoDoesNothing) {
    PingCommand command;
    command.undo();
    SUCCEED();
}
