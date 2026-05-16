#include "commands/get_command.h"
#include "commands/set_command.h"
#include "scenario_helpers.h"
#include "storage/lru_eviction.h"
#include "storage/storage.h"

#include <memory>

int main() {
    Storage::instance().reset_for_tests();
    Storage::instance().set_eviction_strategy(std::make_unique<LRUEvictionStrategy>());
    Storage::instance().set_max_keys(2);

    SetCommand set_a("a", "1");
    SetCommand set_b("b", "2");
    SetCommand set_c("c", "3");
    GetCommand get_a("a");
    GetCommand get_b("b");
    GetCommand get_c("c");

    set_a.execute();
    set_b.execute();
    scenario_expect_eq(get_a.execute(), "1", "reading a makes it most recently used");
    set_c.execute();

    scenario_expect_eq(get_a.execute(), "1", "recently used key remains");
    scenario_expect_eq(get_b.execute(), "(nil)", "least recently used key is evicted");
    scenario_expect_eq(get_c.execute(), "3", "new key remains");

    std::cout << "scenario_eviction_lru passed" << std::endl;
    return 0;
}
