#include "commands/del_command.h"
#include "commands/exists_command.h"
#include "commands/get_command.h"
#include "commands/ping_command.h"
#include "commands/set_command.h"
#include "scenario_helpers.h"
#include "storage/storage.h"

#include <iostream>

int main() {
    Storage::instance().reset_for_tests();

    PingCommand ping;
    scenario_expect_eq(ping.execute(), "PONG", "PING returns PONG");

    SetCommand set_name("name", "Ivan");
    scenario_expect_eq(set_name.execute(), "OK", "SET stores value");

    GetCommand get_name("name");
    scenario_expect_eq(get_name.execute(), "Ivan", "GET returns stored value");

    ExistsCommand exists_name("name");
    scenario_expect_eq(exists_name.execute(), "1", "EXISTS confirms key presence");

    DelCommand del_name("name");
    scenario_expect_eq(del_name.execute(), "OK", "DEL removes existing key");
    scenario_expect_eq(get_name.execute(), "(nil)", "GET returns nil after delete");

    std::cout << "scenario_basic_crud passed" << std::endl;
    return 0;
}
