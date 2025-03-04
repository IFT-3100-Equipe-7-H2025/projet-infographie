#pragma once

#include "3dscene/commands/Command.h"
#include "scenegraph/Node.h"
#include <memory>

class AddChildToNodeCommand : public Command
{
public:
    AddChildToNodeCommand(std::shared_ptr<Node> node, std::shared_ptr<Node> child) : node(std::move(node)), child(std::move(child))
    {
        std::stringstream ss;
        ss << "Add child " << " to node ";
        this->name = ss.str();
    }

    void Execute() override
    {
        node->AddChild(child);
    }

    void Undo() override
    {
        node->RemoveChild(child->GetId());
    }

    [[nodiscard]] std::string DisplayName() const override
    {
        return name;
    }

private:
    std::shared_ptr<Node> node;
    std::shared_ptr<Node> child;
    std::string name;
};
