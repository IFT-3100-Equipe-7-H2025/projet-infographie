#pragma once

#include "3dscene/commands/Command.h"
#include "scenegraph/Node.h"
#include <memory>

class SetScaleCommand : public Command
{
public:
    SetScaleCommand(std::shared_ptr<Node> node, const glm::vec3& newScale) : node(std::move(node)), newScale(newScale), oldScale(this->node->GetInner()->getScale())
    {
        std::stringstream ss;
        ss << "Set scale of node {" << this->node->GetName() << "}" << " to {" << this->newScale.x << ", " << this->newScale.y << ", " << this->newScale.z << "}";
        this->name = ss.str();
    }

    // this is used with sliders to only set the new scale when the user releases the slider instead of every frame
    SetScaleCommand(std::shared_ptr<Node> node, const glm::vec3& newScale, const glm::vec3& oldScale) : node(std::move(node)), newScale(newScale), oldScale(oldScale)
    {
        std::stringstream ss;
        ss << "Set scale of node {" << this->node->GetName() << "}" << " to {" << this->newScale.x << ", " << this->newScale.y << ", " << this->newScale.z << "}";
        this->name = ss.str();
    }

    bool Execute() override
    {
        this->node->GetInner()->setScale(this->newScale);
        return true;
    }

    void Undo() override
    {
        this->node->GetInner()->setScale(this->oldScale);
    }

    [[nodiscard]] std::string DisplayName() const override
    {
        return this->name;
    }

private:
    std::shared_ptr<Node> node;
    glm::vec3 newScale;
    glm::vec3 oldScale;
    std::string name;
};
