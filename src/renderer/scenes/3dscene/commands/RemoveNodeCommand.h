#pragma once

#include "3dscene/commands/Command.h"
#include "scenegraph/Node.h"


class RemoveNodeCommand : public Command
{
public:
    explicit RemoveNodeCommand(std::shared_ptr<Node> node) : node(std::move(node))
    {
        std::stringstream ss;
        ss << "Remove node {" << this->node->GetName() << "}";
        this->name = ss.str();

        this->parent = this->node->GetParent().lock();
        this->children = this->node->GetChildren();
    }

    void Execute() override
    {
        this->node->Delete();
    }

    void Undo() override
    {
        this->parent->AddChild(this->node);
        for (const auto& child: this->children)
        {
            this->node->AddChild(child);
        }
    }

    [[nodiscard]] std::string DisplayName() const override
    {
        return this->name;
    }

private:
    std::string name;
    std::shared_ptr<Node> node;

    std::shared_ptr<Node> parent;
    std::vector<std::shared_ptr<Node>> children;
};
