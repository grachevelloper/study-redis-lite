include(FetchContent)

find_package(GTest CONFIG QUIET)

if(NOT GTest_FOUND)
    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
    )
    FetchContent_MakeAvailable(googletest)
endif()

add_executable(redis-lite-unit-tests
    unit/test_storage.cpp
    unit/test_transaction.cpp
    unit/test_command_factory.cpp
    unit/test_set_command.cpp
    unit/test_get_command.cpp
    unit/test_del_command.cpp
    unit/test_exists_command.cpp
    unit/test_ping_command.cpp
    unit/test_validation_middleware.cpp
    unit/test_router_middleware.cpp
    unit/test_eviction_strategies.cpp
    unit/test_client_session.cpp
)

target_include_directories(redis-lite-unit-tests PRIVATE
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/tests
    ${PROJECT_SOURCE_DIR}/tests/helpers
)
target_link_libraries(redis-lite-unit-tests PRIVATE
    redis_lite_core
    GTest::gtest
    GTest::gtest_main
    pthread
)

include(GoogleTest)
gtest_discover_tests(redis-lite-unit-tests)

function(add_scenario name)
    add_executable(${name} scenarios/${name}.cpp)
    target_include_directories(${name} PRIVATE
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/tests
        ${PROJECT_SOURCE_DIR}/tests/helpers
    )
    target_link_libraries(${name} PRIVATE redis_lite_core pthread)
endfunction()

add_scenario(scenario_basic_crud)
add_scenario(scenario_transaction_commit)
add_scenario(scenario_transaction_rollback)
add_scenario(scenario_eviction_fifo)
add_scenario(scenario_eviction_lru)
add_scenario(scenario_subscribe_notify)
