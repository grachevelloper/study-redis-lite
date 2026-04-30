#include "server/server.h"

#include "session/client_session.h"

#include <cerrno>
#include <cstring>
#include <iostream>
#include <memory>
#include <netinet/in.h>
#include <pthread.h>
#include <stdexcept>
#include <sys/socket.h>
#include <unistd.h>

namespace {

void* run_client_session(void* arg) {
    std::unique_ptr<std::shared_ptr<ClientSession>> session(
        static_cast<std::shared_ptr<ClientSession>*>(arg));
    (*session)->run();
    return nullptr;
}

int create_listening_socket(int port) {
    const int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        throw std::runtime_error(std::string("socket failed: ") + std::strerror(errno));
    }

    int yes = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0) {
        close(server_fd);
        throw std::runtime_error(std::string("setsockopt failed: ") + std::strerror(errno));
    }

    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(static_cast<uint16_t>(port));

    if (bind(server_fd, reinterpret_cast<sockaddr*>(&address), sizeof(address)) < 0) {
        close(server_fd);
        throw std::runtime_error(std::string("bind failed: ") + std::strerror(errno));
    }

    if (listen(server_fd, SOMAXCONN) < 0) {
        close(server_fd);
        throw std::runtime_error(std::string("listen failed: ") + std::strerror(errno));
    }

    return server_fd;
}

} // namespace

void RedisLiteServer::start(int port) {
    const int server_fd = create_listening_socket(port);
    std::cout << "redis-lite listens on port " << port << std::endl;

    while (true) {
        const int client_fd = accept(server_fd, nullptr, nullptr);
        if (client_fd < 0) {
            if (errno == EINTR) {
                continue;
            }
            std::cerr << "accept failed: " << std::strerror(errno) << std::endl;
            continue;
        }

        auto* session = new std::shared_ptr<ClientSession>(std::make_shared<ClientSession>(client_fd));
        pthread_t thread{};
        const int result = pthread_create(&thread, nullptr, run_client_session, session);
        if (result != 0) {
            delete session;
            close(client_fd);
            std::cerr << "pthread_create failed: " << std::strerror(result) << std::endl;
            continue;
        }
        pthread_detach(thread);
    }
}
