#include "3dscene/commands/Command.h"
#include "scenegraph/Node.h"
#include <memory>

class SetRotationCommand : public Command
{
public:
    SetRotationCommand(std::shared_ptr<Node> node, const glm::quat& newRotation) : node(std::move(node)), newRotation(newRotation), oldRotation(this->node->GetInner()->getOrientationQuat())
    {
        std::stringstream ss;
        ss << "Set rotation of node {" << this->node->GetName() << "}" << " to {" << this->newRotation.x << ", " << this->newRotation.y << ", " << this->newRotation.z << "}";
        this->name = ss.str();
    }

    // this is used with sliders to only set the new rotation when the user releases the slider instead of every frame
    SetRotationCommand(std::shared_ptr<Node> node, const glm::quat& newRotation, const glm::quat& oldRotation) : node(std::move(node)), newRotation(newRotation), oldRotation(oldRotation)
    {
        std::stringstream ss;
        ss << "Set rotation of node {" << this->node->GetName() << "}" << " to {" << this->newRotation.x << ", " << this->newRotation.y << ", " << this->newRotation.z << "}";
        this->name = ss.str();
    }

    bool Execute() override
    {
        if ( auto light = std::dynamic_pointer_cast<Light>(this->node->GetInner()); light )
        {
            light->setOrientation(this->newRotation);
            return true;
        }
        this->node->GetInner()->setOrientation(this->newRotation);
        return true;
    }

    void Undo() override
    {
        if ( auto light = std::dynamic_pointer_cast<Light>(this->node->GetInner()); light )
        {
            light->setOrientation(this->oldRotation);
            return;
        }
        this->node->GetInner()->setOrientation(this->oldRotation);
    }

    [[nodiscard]] std::string DisplayName() const override
    {
        return name;
    }

private:
    std::shared_ptr<Node> node;
    glm::quat newRotation;
    glm::quat oldRotation;
    std::string name;
};
