#pragma once

#include "middleware/middleware.h"

class ValidationMiddleware final : public IMiddleware {
public:
    std::string handle(ClientSession& session, const std::vector<std::string>& tokens) override;
};
