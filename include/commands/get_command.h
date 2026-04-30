#pragma once

#include "commands/command.h"

#include <string>

class GetCommand final : public ICommand {
public:
    explicit GetCommand(std::string key);

    std::string execute() override;
    void undo() override;
    bool mutates() const override;

private:
    std::string key_;
};
