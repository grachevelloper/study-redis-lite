#include "transaction/transaction.h"

void Transaction::record(std::unique_ptr<ICommand> command) {
    if (command) {
        executed_.push_back(std::move(command));
    }
}

void Transaction::commit() {
    executed_.clear();
}

void Transaction::rollback() {
    for (auto it = executed_.rbegin(); it != executed_.rend(); ++it) {
        (*it)->undo();
    }
    executed_.clear();
}
