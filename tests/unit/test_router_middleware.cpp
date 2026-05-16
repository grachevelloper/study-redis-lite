#include "middleware/router_middleware.h"
#include "session/client_session.h"
#include "storage/storage.h"
#include "test_helpers.h"

#include <memory>

TEST_F(StorageTestFixture, RouterExecutesPingCommand) {
    ScopedSocketPair sockets;
    auto session = std::make_shared<ClientSession>(sockets.first());
    RouterMiddleware router;

    EXPECT_EQ(router.handle(*session, {"PING"}), "PONG");
}

TEST_F(StorageTestFixture, RouterBeginsTransaction) {
    ScopedSocketPair sockets;
    auto session = std::make_shared<ClientSession>(sockets.first());
    RouterMiddleware router;

    EXPECT_EQ(router.handle(*session, {"BEGIN"}), "OK");
    EXPECT_TRUE(session->in_transaction());
}

TEST_F(StorageTestFixture, RouterRejectsNestedTransaction) {
    ScopedSocketPair sockets;
    auto session = std::make_shared<ClientSession>(sockets.first());
    RouterMiddleware router;

    EXPECT_EQ(router.handle(*session, {"BEGIN"}), "OK");
    EXPECT_EQ(router.handle(*session, {"BEGIN"}), "ERR transaction already started");
}

TEST_F(StorageTestFixture, RouterCommitsTransaction) {
    ScopedSocketPair sockets;
    auto session = std::make_shared<ClientSession>(sockets.first());
    RouterMiddleware router;

    router.handle(*session, {"BEGIN"});
    EXPECT_EQ(router.handle(*session, {"COMMIT"}), "OK");
    EXPECT_FALSE(session->in_transaction());
}

TEST_F(StorageTestFixture, RouterRollsBackTransaction) {
    ScopedSocketPair sockets;
    auto session = std::make_shared<ClientSession>(sockets.first());
    RouterMiddleware router;

    router.handle(*session, {"BEGIN"});
    EXPECT_EQ(router.handle(*session, {"SET", "name", "Ivan"}), "OK");
    EXPECT_EQ(router.handle(*session, {"ROLLBACK"}), "OK");
    EXPECT_FALSE(Storage::instance().exists("name"));
}

TEST_F(StorageTestFixture, RouterRejectsCommitWithoutTransaction) {
    ScopedSocketPair sockets;
    auto session = std::make_shared<ClientSession>(sockets.first());
    RouterMiddleware router;

    EXPECT_EQ(router.handle(*session, {"COMMIT"}), "ERR no active transaction");
}

TEST_F(StorageTestFixture, RouterSubscribesSessionToKey) {
    ScopedSocketPair sockets;
    auto session = std::make_shared<ClientSession>(sockets.first());
    RouterMiddleware router;

    EXPECT_EQ(router.handle(*session, {"SUBSCRIBE", "name"}), "OK");
    Storage::instance().set("name", "Ivan");
    EXPECT_EQ(read_line_from_fd(sockets.second()), "NOTIFY name Ivan");
}

TEST_F(StorageTestFixture, RouterReportsUnknownCommandError) {
    ScopedSocketPair sockets;
    auto session = std::make_shared<ClientSession>(sockets.first());
    RouterMiddleware router;

    EXPECT_EQ(router.handle(*session, {"UNKNOWN"}), "ERR unknown command");
}
