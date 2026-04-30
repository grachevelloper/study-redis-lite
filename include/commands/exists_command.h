#pragma once

#include "commands/command.h"

#include <string>

class ExistsCommand final : public ICommand {
public:
    explicit ExistsCommand(std::string key);

    std::string execute() override;
    void undo() override;
    bool mutates() const override;

private:
    std::string key_;
};
