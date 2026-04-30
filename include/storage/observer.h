#pragma once

#include <string>

class IObserver {
public:
    virtual ~IObserver() = default;
    virtual void notify(const std::string& key, const std::string& value) = 0;
};
