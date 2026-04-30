#include "server/server.h"

#include <cstdlib>
#include <exception>
#include <iostream>

int main(int argc, char* argv[]) {
    int port = 6379;
    if (argc > 1) {
        port = std::atoi(argv[1]);
    }

    try {
        RedisLiteServer server;
        server.start(port);
    } catch (const std::exception& error) {
        std::cerr << "fatal: " << error.what() << std::endl;
        return 1;
    }

    return 0;
}
