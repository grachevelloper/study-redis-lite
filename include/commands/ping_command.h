#pragma once

#include "commands/command.h"

class PingCommand final : public ICommand {
public:
    std::string execute() override;
    void undo() override;
    bool mutates() const override;
};
