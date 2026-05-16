#pragma once

#include "commands/command.h"
#include "storage/observer.h"
#include "storage/storage.h"

#include <gtest/gtest.h>

#include <stdexcept>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include <utility>
#include <vector>

class ScopedSocketPair {
public:
    ScopedSocketPair() {
        if (socketpair(AF_UNIX, SOCK_STREAM, 0, fds_) != 0) {
            throw std::runtime_error("socketpair failed");
        }
    }

    ~ScopedSocketPair() {
        close_if_open(fds_[0]);
        close_if_open(fds_[1]);
    }

    ScopedSocketPair(const ScopedSocketPair&) = delete;
    ScopedSocketPair& operator=(const ScopedSocketPair&) = delete;

    int first() const {
        return fds_[0];
    }

    int second() const {
        return fds_[1];
    }

private:
    static void close_if_open(int& fd) {
        if (fd >= 0) {
            close(fd);
            fd = -1;
        }
    }

    int fds_[2] = {-1, -1};
};

inline std::string read_line_from_fd(int fd) {
    std::string line;
    char ch = '\0';

    while (true) {
        const auto count = recv(fd, &ch, 1, 0);
        if (count <= 0) {
            break;
        }
        if (ch == '\n') {
            break;
        }
        if (ch != '\r') {
            line.push_back(ch);
        }
    }

    return line;
}

class RecordingObserver final : public IObserver {
public:
    void notify(const std::string& key, const std::string& value) override {
        notifications.emplace_back(key + "=" + value);
    }

    std::vector<std::string> notifications;
};

class FakeCommand final : public ICommand {
public:
    FakeCommand(std::string response, bool mutates, int* undo_counter = nullptr)
        : response_(std::move(response)),
          mutates_(mutates),
          undo_counter_(undo_counter) {
    }

    std::string execute() override {
        ++execute_count_;
        return response_;
    }

    void undo() override {
        ++undo_count_;
        if (undo_counter_) {
            ++(*undo_counter_);
        }
    }

    bool mutates() const override {
        return mutates_;
    }

    int execute_count() const {
        return execute_count_;
    }

    int undo_count() const {
        return undo_count_;
    }

private:
    std::string response_;
    bool mutates_;
    int* undo_counter_ = nullptr;
    int execute_count_ = 0;
    int undo_count_ = 0;
};

class StorageTestFixture : public ::testing::Test {
protected:
    void SetUp() override {
        Storage::instance().reset_for_tests();
    }

    void TearDown() override {
        Storage::instance().reset_for_tests();
    }
};
