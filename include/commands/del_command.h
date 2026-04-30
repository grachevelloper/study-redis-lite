#pragma once

#include "commands/command.h"

#include <optional>
#include <string>

class DelCommand final : public ICommand {
public:
    explicit DelCommand(std::string key);

    std::string execute() override;
    void undo() override;
    bool mutates() const override;

private:
    std::string key_;
    std::optional<std::string> old_value_;
    bool executed_ = false;
};
