#include "commands/get_command.h"
#include "commands/set_command.h"
#include "scenario_helpers.h"
#include "storage/fifo_eviction.h"
#include "storage/storage.h"

#include <memory>

int main() {
    Storage::instance().reset_for_tests();
    Storage::instance().set_eviction_strategy(std::make_unique<FIFOEvictionStrategy>());
    Storage::instance().set_max_keys(2);

    SetCommand set_a("a", "1");
    SetCommand set_b("b", "2");
    SetCommand set_c("c", "3");

    set_a.execute();
    set_b.execute();
    set_c.execute();

    GetCommand get_a("a");
    GetCommand get_b("b");
    GetCommand get_c("c");

    scenario_expect_eq(get_a.execute(), "(nil)", "FIFO evicts oldest inserted key");
    scenario_expect_eq(get_b.execute(), "2", "second key remains");
    scenario_expect_eq(get_c.execute(), "3", "last inserted key remains");

    std::cout << "scenario_eviction_fifo passed" << std::endl;
    return 0;
}
