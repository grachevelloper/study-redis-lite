#pragma once

#include "middleware/middleware.h"
#include "storage/observer.h"
#include "transaction/transaction.h"

#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <vector>

class ClientSession final : public IObserver, public std::enable_shared_from_this<ClientSession> {
public:
    explicit ClientSession(int client_fd);
    ~ClientSession() override;

    ClientSession(const ClientSession&) = delete;
    ClientSession& operator=(const ClientSession&) = delete;

    void run();
    void notify(const std::string& key, const std::string& value) override;

    bool in_transaction() const;
    void begin_transaction();
    void commit_transaction();
    void rollback_transaction();
    void record_command(std::unique_ptr<ICommand> command);
    void subscribe_to(const std::string& key);

private:
    static std::vector<std::string> tokenize(const std::string& line);

    std::optional<std::string> read_line();
    bool send_line(const std::string& line);
    void close_socket();
    std::shared_ptr<IMiddleware> build_middleware_chain();

    int client_fd_;
    std::mutex send_mutex_;
    std::unique_ptr<Transaction> transaction_;
};
