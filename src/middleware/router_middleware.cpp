#include "middleware/router_middleware.h"

#include "commands/command.h"
#include "session/client_session.h"
#include "storage/storage.h"

#include <algorithm>
#include <cctype>
#include <exception>
#include <string>
#include <vector>

namespace {

std::vector<std::string> normalize_command(std::vector<std::string> tokens) {
    std::transform(tokens[0].begin(), tokens[0].end(), tokens[0].begin(), [](unsigned char ch) {
        return static_cast<char>(std::toupper(ch));
    });
    return tokens;
}

} // namespace

std::string RouterMiddleware::handle(ClientSession& session, const std::vector<std::string>& tokens) {
    auto normalized = normalize_command(tokens);
    const auto& command_name = normalized[0];

    if (command_name == "BEGIN") {
        if (session.in_transaction()) {
            return "ERR transaction already started";
        }
        session.begin_transaction();
        return "OK";
    }

    if (command_name == "COMMIT") {
        if (!session.in_transaction()) {
            return "ERR no active transaction";
        }
        session.commit_transaction();
        return "OK";
    }

    if (command_name == "ROLLBACK") {
        if (!session.in_transaction()) {
            return "ERR no active transaction";
        }
        session.rollback_transaction();
        return "OK";
    }

    if (command_name == "SUBSCRIBE") {
        session.subscribe_to(normalized[1]);
        return "OK";
    }

    try {
        auto command = CommandFactory::create(normalized);
        const auto mutates = command->mutates();
        const auto response = command->execute();
        if (mutates && session.in_transaction()) {
            session.record_command(std::move(command));
        }
        return response;
    } catch (const std::exception& error) {
        return std::string("ERR ") + error.what();
    }
}
