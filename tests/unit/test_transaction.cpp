#include "storage/storage.h"
#include "test_helpers.h"
#include "transaction/transaction.h"

#include <memory>

TEST_F(StorageTestFixture, TransactionRollbackCallsUndoForEachRecordedCommand) {
    Transaction transaction;
    int undo_counter = 0;

    transaction.record(std::make_unique<FakeCommand>("OK", true, &undo_counter));
    transaction.record(std::make_unique<FakeCommand>("OK", true, &undo_counter));

    transaction.rollback();
    EXPECT_EQ(undo_counter, 2);
}

TEST_F(StorageTestFixture, TransactionRollbackUndoesCommandsInReverseOrder) {
    Transaction transaction;
    std::string order;

    class OrderedUndoCommand final : public ICommand {
    public:
        OrderedUndoCommand(std::string marker, std::string* order)
            : marker_(std::move(marker)),
              order_(order) {
        }

        std::string execute() override {
            return "OK";
        }

        void undo() override {
            *order_ += marker_;
        }

        bool mutates() const override {
            return true;
        }

    private:
        std::string marker_;
        std::string* order_;
    };

    transaction.record(std::make_unique<OrderedUndoCommand>("A", &order));
    transaction.record(std::make_unique<OrderedUndoCommand>("B", &order));
    transaction.record(std::make_unique<OrderedUndoCommand>("C", &order));

    transaction.rollback();
    EXPECT_EQ(order, "CBA");
}

TEST_F(StorageTestFixture, TransactionCommitDoesNotCallUndo) {
    Transaction transaction;
    int undo_counter = 0;

    transaction.record(std::make_unique<FakeCommand>("OK", true, &undo_counter));
    transaction.commit();

    EXPECT_EQ(undo_counter, 0);
}

TEST_F(StorageTestFixture, TransactionRollbackOnEmptyTransactionIsSafe) {
    Transaction transaction;
    transaction.rollback();
    SUCCEED();
}

TEST_F(StorageTestFixture, TransactionIgnoresNullCommands) {
    Transaction transaction;
    transaction.record(nullptr);
    transaction.commit();
    SUCCEED();
}
