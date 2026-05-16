#pragma once

#include <cstdlib>
#include <iostream>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

class ScenarioSocketPair {
public:
    ScenarioSocketPair() {
        if (socketpair(AF_UNIX, SOCK_STREAM, 0, fds_) != 0) {
            std::cerr << "socketpair failed" << std::endl;
            std::exit(1);
        }
    }

    ~ScenarioSocketPair() {
        close_if_open(fds_[0]);
        close_if_open(fds_[1]);
    }

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

inline std::string scenario_read_line(int fd) {
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

inline void scenario_expect(bool condition, const std::string& message) {
    if (!condition) {
        std::cerr << "[FAIL] " << message << std::endl;
        std::exit(1);
    }
    std::cout << "[OK] " << message << std::endl;
}

inline void scenario_expect_eq(
    const std::string& actual,
    const std::string& expected,
    const std::string& message) {
    if (actual != expected) {
        std::cerr << "[FAIL] " << message << ": expected '" << expected << "', got '" << actual << "'"
                  << std::endl;
        std::exit(1);
    }
    std::cout << "[OK] " << message << std::endl;
}
