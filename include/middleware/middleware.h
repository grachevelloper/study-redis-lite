#pragma once

#include <memory>
#include <string>
#include <vector>

class ClientSession;

class IMiddleware {
public:
    virtual ~IMiddleware() = default;

    void set_next(std::shared_ptr<IMiddleware> next) {
        next_ = std::move(next);
    }

    virtual std::string handle(ClientSession& session, const std::vector<std::string>& tokens) {
        if (next_) {
            return next_->handle(session, tokens);
        }
        return "ERR no route";
    }

private:
    std::shared_ptr<IMiddleware> next_;
};
