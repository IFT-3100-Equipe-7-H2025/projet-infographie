#pragma once

#include <string>

class Command
{
public:
    virtual bool                      Execute() = 0;
    virtual void                      Undo() = 0;
    [[nodiscard]] virtual std::string DisplayName() const = 0;

    virtual ~Command() = default;
};
