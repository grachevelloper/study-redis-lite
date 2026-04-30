#pragma once

#include "commands/command.h"

#include <memory>
#include <vector>

class Transaction {
public:
    void record(std::unique_ptr<ICommand> command);
    void commit();
    void rollback();

private:
    std::vector<std::unique_ptr<ICommand>> executed_;
};
