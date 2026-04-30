#include "middleware/validation_middleware.h"

#include <algorithm>
#include <cctype>
#include <string>

namespace {

std::string uppercase(std::string value) {
    std::transform(value.begin(), value.end(), value.begin(), [](unsigned char ch) {
        return static_cast<char>(std::toupper(ch));
    });
    return value;
}

} // namespace

std::string ValidationMiddleware::handle(ClientSession& session, const std::vector<std::string>& tokens) {
    if (tokens.empty()) {
        return "ERR empty command";
    }

    const auto command = uppercase(tokens[0]);

    if (command == "PING" && tokens.size() == 1) {
        return IMiddleware::handle(session, tokens);
    }
    if ((command == "GET" || command == "DEL" || command == "EXISTS" || command == "SUBSCRIBE") &&
        tokens.size() == 2) {
        return IMiddleware::handle(session, tokens);
    }
    if (command == "SET" && tokens.size() == 3) {
        return IMiddleware::handle(session, tokens);
    }
    if ((command == "BEGIN" || command == "COMMIT" || command == "ROLLBACK") && tokens.size() == 1) {
        return IMiddleware::handle(session, tokens);
    }

    if (command == "PING" || command == "GET" || command == "DEL" || command == "EXISTS" ||
        command == "SUBSCRIBE" || command == "SET" || command == "BEGIN" || command == "COMMIT" ||
        command == "ROLLBACK") {
        return "ERR wrong number of arguments";
    }

    return "ERR unknown command";
}
