#include "commands/del_command.h"

#include "storage/storage.h"

DelCommand::DelCommand(std::string key)
    : key_(std::move(key)) {
}

std::string DelCommand::execute() {
    old_value_ = Storage::instance().del(key_);
    executed_ = true;
    return old_value_ ? "OK" : "(nil)";
}

void DelCommand::undo() {
    if (executed_ && old_value_) {
        Storage::instance().set(key_, *old_value_);
    }
}

bool DelCommand::mutates() const {
    return true;
}
