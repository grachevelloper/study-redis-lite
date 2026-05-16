#include "commands/command.h"
#include "commands/del_command.h"
#include "commands/exists_command.h"
#include "commands/get_command.h"
#include "commands/ping_command.h"
#include "commands/set_command.h"

#include <gtest/gtest.h>
#include <memory>
#include <stdexcept>
#include <vector>

TEST(CommandFactoryTest, CreatesPingCommand) {
    auto command = CommandFactory::create({"PING"});
    EXPECT_NE(dynamic_cast<PingCommand*>(command.get()), nullptr);
}

TEST(CommandFactoryTest, CreatesSetCommand) {
    auto command = CommandFactory::create({"SET", "name", "Ivan"});
    EXPECT_NE(dynamic_cast<SetCommand*>(command.get()), nullptr);
}

TEST(CommandFactoryTest, CreatesGetCommand) {
    auto command = CommandFactory::create({"GET", "name"});
    EXPECT_NE(dynamic_cast<GetCommand*>(command.get()), nullptr);
}

TEST(CommandFactoryTest, CreatesDelCommand) {
    auto command = CommandFactory::create({"DEL", "name"});
    EXPECT_NE(dynamic_cast<DelCommand*>(command.get()), nullptr);
}

TEST(CommandFactoryTest, CreatesExistsCommand) {
    auto command = CommandFactory::create({"EXISTS", "name"});
    EXPECT_NE(dynamic_cast<ExistsCommand*>(command.get()), nullptr);
}

TEST(CommandFactoryTest, ThrowsForEmptyCommand) {
    EXPECT_THROW(CommandFactory::create({}), std::invalid_argument);
}

TEST(CommandFactoryTest, ThrowsForUnknownCommand) {
    EXPECT_THROW(CommandFactory::create({"UNKNOWN"}), std::invalid_argument);
}
