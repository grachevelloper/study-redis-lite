#include "commands/command.h"

#include "commands/del_command.h"
#include "commands/exists_command.h"
#include "commands/get_command.h"
#include "commands/ping_command.h"
#include "commands/set_command.h"

#include <stdexcept>

std::unique_ptr<ICommand> CommandFactory::create(const std::vector<std::string>& tokens) {
    if (tokens.empty()) {
        throw std::invalid_argument("empty command");
    }

    const auto& name = tokens[0];
    if (name == "PING") {
        return std::make_unique<PingCommand>();
    }
    if (name == "SET") {
        return std::make_unique<SetCommand>(tokens[1], tokens[2]);
    }
    if (name == "GET") {
        return std::make_unique<GetCommand>(tokens[1]);
    }
    if (name == "DEL") {
        return std::make_unique<DelCommand>(tokens[1]);
    }
    if (name == "EXISTS") {
        return std::make_unique<ExistsCommand>(tokens[1]);
    }

    throw std::invalid_argument("unknown command");
}
