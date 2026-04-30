CXX ?= clang++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -Wpedantic -Iinclude
LDFLAGS ?= -pthread

SOURCES := \
	src/main.cpp \
	src/commands/command_factory.cpp \
	src/commands/del_command.cpp \
	src/commands/exists_command.cpp \
	src/commands/get_command.cpp \
	src/commands/ping_command.cpp \
	src/commands/set_command.cpp \
	src/middleware/router_middleware.cpp \
	src/middleware/validation_middleware.cpp \
	src/server/server.cpp \
	src/session/client_session.cpp \
	src/storage/fifo_eviction.cpp \
	src/storage/lru_eviction.cpp \
	src/storage/storage.cpp \
	src/transaction/transaction.cpp

TARGET := build/redis-lite

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(SOURCES)
	mkdir -p build
	$(CXX) $(CXXFLAGS) $(SOURCES) $(LDFLAGS) -o $(TARGET)

run: $(TARGET)
	./$(TARGET) 6379

clean:
	rm -rf build
