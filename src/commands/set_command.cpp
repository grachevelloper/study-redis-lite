#include "commands/set_command.h"

#include "storage/storage.h"

SetCommand::SetCommand(std::string key, std::string value)
    : key_(std::move(key)),
      value_(std::move(value)) {
}

std::string SetCommand::execute() {
    old_value_ = Storage::instance().set(key_, value_);
    executed_ = true;
    return "OK";
}

void SetCommand::undo() {
    if (!executed_) {
        return;
    }

    if (old_value_) {
        Storage::instance().set(key_, *old_value_);
    } else {
        Storage::instance().del(key_);
    }
}

bool SetCommand::mutates() const {
    return true;
}
