cmake_minimum_required(VERSION 3.10)

project(redis_lite LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(redis-lite
    src/main.cpp
    src/commands/command_factory.cpp
    src/commands/del_command.cpp
    src/commands/exists_command.cpp
    src/commands/get_command.cpp
    src/commands/ping_command.cpp
    src/commands/set_command.cpp
    src/middleware/router_middleware.cpp
    src/middleware/validation_middleware.cpp
    src/server/server.cpp
    src/session/client_session.cpp
    src/storage/fifo_eviction.cpp
    src/storage/lru_eviction.cpp
    src/storage/storage.cpp
    src/transaction/transaction.cpp
)

target_include_directories(redis-lite PRIVATE include)
target_link_libraries(redis-lite PRIVATE pthread)

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    target_compile_options(redis-lite PRIVATE -Wall -Wextra -Wpedantic)
endif()
