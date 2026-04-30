#include "commands/get_command.h"

#include "storage/storage.h"

GetCommand::GetCommand(std::string key)
    : key_(std::move(key)) {
}

std::string GetCommand::execute() {
    const auto value = Storage::instance().get(key_);
    return value.value_or("(nil)");
}

void GetCommand::undo() {
}

bool GetCommand::mutates() const {
    return false;
}
