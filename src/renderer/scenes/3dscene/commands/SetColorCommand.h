#pragma once

#include "3dscene/commands/Command.h"
#include "Primitive3D.h"
#include "scenegraph/Node.h"
#include <memory>

class SetColorCommand : public Command
{
public:
    SetColorCommand(std::shared_ptr<Node> node, const ofColor& newColor) : node(std::move(node)), newColor(newColor)
    {
        if (const auto primitive = std::dynamic_pointer_cast<Primitive3D>(this->node->GetInner()); primitive)
        {
            this->oldColor = primitive->GetColor();
        }
        else
        {
            ofLogError() << "Cannot set color of node {" << this->node->GetName() << "} because it is not a Primitive3D";
            return;
        }

        std::stringstream ss;
        ss << "Set color of node {" << this->node->GetName() << "}" << " to {" << this->newColor << "}";
        this->name = ss.str();
    }

    // this is used with sliders to only set the new scale when the user releases the slider instead of every frame
    SetColorCommand(std::shared_ptr<Node> node, const ofColor& newColor, const ofColor& oldColor) : node(std::move(node)), newColor(newColor), oldColor(oldColor)
    {
        std::stringstream ss;
        ss << "Set color of node {" << this->node->GetName() << "}" << " to {" << this->newColor << "}";
        this->name = ss.str();
    }

    bool Execute() override
    {
        if (const auto primitive = std::dynamic_pointer_cast<Primitive3D>(this->node->GetInner()); primitive)
        {
            primitive->SetColor(this->newColor);
        }
        else
        {
            ofLogError() << "Cannot set color of node {" << this->node->GetName() << "} because it is not a Primitive3D";
            return false;
        }

        return true;
    }

    void Undo() override
    {
        if (const auto primitive = std::dynamic_pointer_cast<Primitive3D>(this->node->GetInner()); primitive)
        {
            primitive->SetColor(this->oldColor);
        }
        else
        {
            ofLogError() << "Cannot set color of node {" << this->node->GetName() << "} because it is not a Primitive3D";
        }
    }

    [[nodiscard]] std::string DisplayName() const override
    {
        return this->name;
    }

private:
    std::shared_ptr<Node> node;
    ofColor newColor;
    ofColor oldColor;
    std::string name;
};
