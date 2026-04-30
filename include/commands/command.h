#pragma once

#include <memory>
#include <optional>
#include <string>
#include <vector>

class ICommand {
public:
    virtual ~ICommand() = default;
    virtual std::string execute() = 0;
    virtual void undo() = 0;
    virtual bool mutates() const = 0;
};

class CommandFactory {
public:
    static std::unique_ptr<ICommand> create(const std::vector<std::string>& tokens);
};
