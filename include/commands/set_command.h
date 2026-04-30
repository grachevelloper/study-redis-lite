#pragma once

#include "commands/command.h"

#include <optional>
#include <string>

class SetCommand final : public ICommand {
public:
    SetCommand(std::string key, std::string value);

    std::string execute() override;
    void undo() override;
    bool mutates() const override;

private:
    std::string key_;
    std::string value_;
    std::optional<std::string> old_value_;
    bool executed_ = false;
};
