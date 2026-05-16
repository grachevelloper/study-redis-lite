#include "middleware/validation_middleware.h"
#include "session/client_session.h"
#include "test_helpers.h"

#include <memory>

namespace {

class EchoMiddleware final : public IMiddleware {
public:
    std::string handle(ClientSession&, const std::vector<std::string>& tokens) override {
        forwarded_tokens = tokens;
        return "FORWARDED";
    }

    std::vector<std::string> forwarded_tokens;
};

} // namespace

TEST_F(StorageTestFixture, ValidationRejectsEmptyCommand) {
    ScopedSocketPair sockets;
    auto session = std::make_shared<ClientSession>(sockets.first());
    ValidationMiddleware middleware;

    EXPECT_EQ(middleware.handle(*session, {}), "ERR empty command");
}

TEST_F(StorageTestFixture, ValidationAcceptsPing) {
    ScopedSocketPair sockets;
    auto session = std::make_shared<ClientSession>(sockets.first());
    ValidationMiddleware middleware;
    auto next = std::make_shared<EchoMiddleware>();
    middleware.set_next(next);

    EXPECT_EQ(middleware.handle(*session, {"PING"}), "FORWARDED");
}

TEST_F(StorageTestFixture, ValidationAcceptsSetWithThreeArguments) {
    ScopedSocketPair sockets;
    auto session = std::make_shared<ClientSession>(sockets.first());
    ValidationMiddleware middleware;
    auto next = std::make_shared<EchoMiddleware>();
    middleware.set_next(next);

    EXPECT_EQ(middleware.handle(*session, {"SET", "name", "Ivan"}), "FORWARDED");
}

TEST_F(StorageTestFixture, ValidationRejectsSetWithWrongArgumentsCount) {
    ScopedSocketPair sockets;
    auto session = std::make_shared<ClientSession>(sockets.first());
    ValidationMiddleware middleware;

    EXPECT_EQ(middleware.handle(*session, {"SET", "name"}), "ERR wrong number of arguments");
}

TEST_F(StorageTestFixture, ValidationRejectsGetWithoutKey) {
    ScopedSocketPair sockets;
    auto session = std::make_shared<ClientSession>(sockets.first());
    ValidationMiddleware middleware;

    EXPECT_EQ(middleware.handle(*session, {"GET"}), "ERR wrong number of arguments");
}

TEST_F(StorageTestFixture, ValidationRejectsUnknownCommand) {
    ScopedSocketPair sockets;
    auto session = std::make_shared<ClientSession>(sockets.first());
    ValidationMiddleware middleware;

    EXPECT_EQ(middleware.handle(*session, {"HELLO"}), "ERR unknown command");
}

TEST_F(StorageTestFixture, ValidationAcceptsTransactionCommands) {
    ScopedSocketPair sockets;
    auto session = std::make_shared<ClientSession>(sockets.first());
    ValidationMiddleware middleware;
    auto next = std::make_shared<EchoMiddleware>();
    middleware.set_next(next);

    EXPECT_EQ(middleware.handle(*session, {"BEGIN"}), "FORWARDED");
    EXPECT_EQ(middleware.handle(*session, {"COMMIT"}), "FORWARDED");
    EXPECT_EQ(middleware.handle(*session, {"ROLLBACK"}), "FORWARDED");
}

TEST_F(StorageTestFixture, ValidationAcceptsSubscribeWithKey) {
    ScopedSocketPair sockets;
    auto session = std::make_shared<ClientSession>(sockets.first());
    ValidationMiddleware middleware;
    auto next = std::make_shared<EchoMiddleware>();
    middleware.set_next(next);

    EXPECT_EQ(middleware.handle(*session, {"SUBSCRIBE", "news"}), "FORWARDED");
}
