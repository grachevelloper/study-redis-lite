#include "session/client_session.h"

#include "middleware/router_middleware.h"
#include "middleware/validation_middleware.h"
#include "storage/storage.h"

#include <algorithm>
#include <cerrno>
#include <cctype>
#include <cstring>
#include <sstream>
#include <sys/socket.h>
#include <unistd.h>

ClientSession::ClientSession(int client_fd)
    : client_fd_(client_fd) {
}

ClientSession::~ClientSession() {
    Storage::instance().unsubscribe_all(this);
    close_socket();
}

void ClientSession::run() {
    const auto middleware = build_middleware_chain();

    while (true) {
        auto line = read_line();
        if (!line) {
            break;
        }

        auto tokens = tokenize(*line);
        const auto response = middleware->handle(*this, tokens);
        if (!send_line(response)) {
            break;
        }
    }
}

void ClientSession::notify(const std::string& key, const std::string& value) {
    send_line("NOTIFY " + key + " " + value);
}

bool ClientSession::in_transaction() const {
    return transaction_ != nullptr;
}

void ClientSession::begin_transaction() {
    transaction_ = std::make_unique<Transaction>();
}

void ClientSession::commit_transaction() {
    if (transaction_) {
        transaction_->commit();
        transaction_.reset();
    }
}

void ClientSession::rollback_transaction() {
    if (transaction_) {
        transaction_->rollback();
        transaction_.reset();
    }
}

void ClientSession::record_command(std::unique_ptr<ICommand> command) {
    if (transaction_) {
        transaction_->record(std::move(command));
    }
}

void ClientSession::subscribe_to(const std::string& key) {
    Storage::instance().subscribe(key, shared_from_this());
}

std::vector<std::string> ClientSession::tokenize(const std::string& line) {
    std::istringstream stream(line);
    std::vector<std::string> tokens;
    std::string token;

    while (stream >> token) {
        tokens.push_back(token);
    }

    return tokens;
}

std::optional<std::string> ClientSession::read_line() {
    std::string line;
    char ch = '\0';

    while (true) {
        const auto read_count = recv(client_fd_, &ch, 1, 0);
        if (read_count == 0) {
            return std::nullopt;
        }
        if (read_count < 0) {
            if (errno == EINTR) {
                continue;
            }
            return std::nullopt;
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

bool ClientSession::send_line(const std::string& line) {
    const auto payload = line + "\n";
    std::lock_guard<std::mutex> lock(send_mutex_);

    std::size_t sent = 0;
    while (sent < payload.size()) {
        const auto result = send(client_fd_, payload.data() + sent, payload.size() - sent, 0);
        if (result < 0) {
            if (errno == EINTR) {
                continue;
            }
            return false;
        }
        sent += static_cast<std::size_t>(result);
    }

    return true;
}

void ClientSession::close_socket() {
    if (client_fd_ >= 0) {
        close(client_fd_);
        client_fd_ = -1;
    }
}

std::shared_ptr<IMiddleware> ClientSession::build_middleware_chain() {
    auto validation = std::make_shared<ValidationMiddleware>();
    auto router = std::make_shared<RouterMiddleware>();
    validation->set_next(router);
    return validation;
}
