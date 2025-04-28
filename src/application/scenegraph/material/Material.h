#pragma once

#include <string>

class Material
{
public:
    explicit Material(std::string name) : name(std::move(name)) {};

    virtual ~Material() = default;
    virtual void begin() = 0;
    virtual void end() = 0;

    [[nodiscard]] std::string GetName() const { return this->name; }

protected:
    std::string name;
};
