#include "scenario_helpers.h"
#include "session/client_session.h"
#include "storage/storage.h"

#include <iostream>
#include <memory>

int main() {
    Storage::instance().reset_for_tests();
    ScenarioSocketPair sockets;
    auto session = std::make_shared<ClientSession>(sockets.first());

    session->subscribe_to("name");
    Storage::instance().set("name", "Maria");

    scenario_expect_eq(
        scenario_read_line(sockets.second()),
        "NOTIFY name Maria",
        "subscriber receives notification");

    std::cout << "scenario_subscribe_notify passed" << std::endl;
    return 0;
}
