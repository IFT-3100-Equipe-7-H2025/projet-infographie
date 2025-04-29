#pragma once

#include "Command.h"
#include "scenegraph/Node.h"
#include <memory>

class SetPositionCommand : public Command
{
public:
    SetPositionCommand(std::shared_ptr<Node> node, const glm::vec3& newPosition) : node(std::move(node)), newPosition(newPosition), oldPosition(this->node->GetInner()->getPosition())
    {
        std::stringstream ss;
        ss << "Set position of node {" << this->node->GetName() << "}" << " to {" << this->newPosition.x << ", " << this->newPosition.y << ", " << this->newPosition.z << "}";
        this->name = ss.str();
    }

    // this is used with sliders to only set the new position when the user releases the slider instead of every frame
    SetPositionCommand(std::shared_ptr<Node> node, const glm::vec3& newPosition, const glm::vec3& oldPosition) : node(std::move(node)), newPosition(newPosition), oldPosition(oldPosition)
    {
        std::stringstream ss;
        ss << "Set position of node {" << this->node->GetName() << "}" << " to {" << this->newPosition.x << ", " << this->newPosition.y << ", " << this->newPosition.z << "}";
        this->name = ss.str();
    }

    bool Execute() override
    {
        if (auto light = std::dynamic_pointer_cast<Light>(this->node->GetInner()); light)
        {
            light->setPosition(glm::vec3(this->newPosition[0], this->newPosition[1], this->newPosition[2]));
        }
        else
        {
            this->node->GetInner()->setPosition(this->newPosition);
        }
        return true;
    }

    void Undo() override
    {
        if (auto light = std::dynamic_pointer_cast<Light>(this->node->GetInner()); light)
        {
            light->setPosition(glm::vec3(this->oldPosition[0], this->oldPosition[1], this->oldPosition[2]));
        }
        else
        {
            this->node->GetInner()->setPosition(this->oldPosition);
        }
    }

    [[nodiscard]] std::string DisplayName() const override
    {
        return name;
    }

private:
    std::shared_ptr<Node> node;
    glm::vec3 newPosition;
    glm::vec3 oldPosition;
    std::string name;
};
