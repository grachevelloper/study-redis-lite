#include "commands/get_command.h"
#include "commands/set_command.h"
#include "scenario_helpers.h"
#include "session/client_session.h"
#include "storage/storage.h"

#include <memory>

int main() {
    Storage::instance().reset_for_tests();
    ScenarioSocketPair sockets;
    auto session = std::make_shared<ClientSession>(sockets.first());

    Storage::instance().set("name", "Ivan");
    session->begin_transaction();
    scenario_expect(session->in_transaction(), "transaction started");

    auto command = std::make_unique<SetCommand>("name", "Maria");
    scenario_expect_eq(command->execute(), "OK", "SET inside transaction works");
    session->record_command(std::move(command));

    GetCommand get_name("name");
    scenario_expect_eq(get_name.execute(), "Maria", "new value visible before rollback");

    session->rollback_transaction();
    scenario_expect(!session->in_transaction(), "transaction closed after rollback");
    scenario_expect_eq(get_name.execute(), "Ivan", "rollback restores previous value");

    std::cout << "scenario_transaction_rollback passed" << std::endl;
    return 0;
}
