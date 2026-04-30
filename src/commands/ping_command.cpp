#include "commands/ping_command.h"

std::string PingCommand::execute() {
    return "PONG";
}

void PingCommand::undo() {
}

bool PingCommand::mutates() const {
    return false;
}
