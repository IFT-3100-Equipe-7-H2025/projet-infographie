#pragma once

#include "Command.h"
#include "Node.h"
#include <memory>

class SetMaterialCommand : public Command
{
public:
    SetMaterialCommand(std::shared_ptr<Node> node, std::shared_ptr<Material> newMaterial) : node(std::move(node)), newMaterial(std::move(newMaterial))
    {
        std::stringstream ss;
        ss << "Set material of node {" << this->node->GetName() << "}" << " to {" << this->newMaterial->GetName() << "}";
        this->name = ss.str();
    }

    bool Execute() override
    {
        this->node->SetMaterial(this->newMaterial);
        return true;
    }

    void Undo() override
    {
        this->node->SetMaterial(this->oldMaterial);
    }

    [[nodiscard]] std::string DisplayName() const override
    {
        return this->name;
    }

private:
    std::shared_ptr<Node> node;
    std::shared_ptr<Material> oldMaterial;
    std::shared_ptr<Material> newMaterial;

    std::string name;
};
