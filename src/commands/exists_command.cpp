#include "commands/exists_command.h"

#include "storage/storage.h"

ExistsCommand::ExistsCommand(std::string key)
    : key_(std::move(key)) {
}

std::string ExistsCommand::execute() {
    return Storage::instance().exists(key_) ? "1" : "0";
}

void ExistsCommand::undo() {
}

bool ExistsCommand::mutates() const {
    return false;
}
