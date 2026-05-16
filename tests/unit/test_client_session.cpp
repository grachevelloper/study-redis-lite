#include "commands/set_command.h"
#include "session/client_session.h"
#include "storage/storage.h"
#include "test_helpers.h"

#include <memory>

TEST_F(StorageTestFixture, ClientSessionStartsTransaction) {
    ScopedSocketPair sockets;
    auto session = std::make_shared<ClientSession>(sockets.first());

    EXPECT_FALSE(session->in_transaction());
    session->begin_transaction();
    EXPECT_TRUE(session->in_transaction());
}

TEST_F(StorageTestFixture, ClientSessionCommitClearsTransactionState) {
    ScopedSocketPair sockets;
    auto session = std::make_shared<ClientSession>(sockets.first());

    session->begin_transaction();
    session->commit_transaction();
    EXPECT_FALSE(session->in_transaction());
}

TEST_F(StorageTestFixture, ClientSessionRollbackUndoesRecordedCommands) {
    ScopedSocketPair sockets;
    auto session = std::make_shared<ClientSession>(sockets.first());

    session->begin_transaction();
    auto command = std::make_unique<SetCommand>("name", "Ivan");
    command->execute();
    session->record_command(std::move(command));
    session->rollback_transaction();

    EXPECT_FALSE(Storage::instance().exists("name"));
    EXPECT_FALSE(session->in_transaction());
}

TEST_F(StorageTestFixture, ClientSessionRecordCommandOutsideTransactionDoesNothing) {
    ScopedSocketPair sockets;
    auto session = std::make_shared<ClientSession>(sockets.first());

    auto command = std::make_unique<SetCommand>("name", "Ivan");
    command->execute();
    session->record_command(std::move(command));

    EXPECT_TRUE(Storage::instance().exists("name"));
}

TEST_F(StorageTestFixture, ClientSessionNotifyWritesExpectedPayload) {
    ScopedSocketPair sockets;
    auto session = std::make_shared<ClientSession>(sockets.first());

    session->notify("name", "Ivan");
    EXPECT_EQ(read_line_from_fd(sockets.second()), "NOTIFY name Ivan");
}

TEST_F(StorageTestFixture, ClientSessionSubscribeToRegistersObserver) {
    ScopedSocketPair sockets;
    auto session = std::make_shared<ClientSession>(sockets.first());

    session->subscribe_to("name");
    Storage::instance().set("name", "Ivan");

    EXPECT_EQ(read_line_from_fd(sockets.second()), "NOTIFY name Ivan");
}
