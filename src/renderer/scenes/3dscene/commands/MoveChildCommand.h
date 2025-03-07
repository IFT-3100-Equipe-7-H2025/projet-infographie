#pragma once
#include "Command.h"
#include "Node.h"

class MoveChildCommand : public Command
{
public:
    MoveChildCommand(std::shared_ptr<Node> node, std::shared_ptr<Node> newParent) : node(std::move(node))
                                                                                  , newParent(std::move(newParent))
    {
        this->oldParent = this->node->GetParent().lock();

        std::stringstream ss;
        ss << "Move child {" << this->node->GetName() << "} from node {" << this->oldParent->GetName() << "} to node {" << this->newParent->GetName() << "}";
        this->name = ss.str();
    }

    bool Execute() override
    {
        this->oldParent->RemoveChild(this->node->GetId());
        return this->newParent->AddChild(this->node);
    }

    void Undo() override
    {
        this->newParent->RemoveChild(this->node->GetId());
        this->oldParent->AddChild(this->node);
    }

    [[nodiscard]] std::string DisplayName() const override { return this->name; }

private:
    std::string           name;

    std::shared_ptr<Node> node;
    std::shared_ptr<Node> newParent;
    std::shared_ptr<Node> oldParent;
};
