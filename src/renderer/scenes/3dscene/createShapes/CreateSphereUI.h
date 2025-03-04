#pragma once
#include "3dscene/commands/AddShapeChildToNodeCommand.h"
#include "CreateShapeUI.h"
#include "imgui.h"
#include "of3dPrimitives.h"

constexpr float DEFAULT_SPHERE_RADIUS = 100.0f;

class CreateSphereUI : public CreateShapeUI
{
public:
    CreateSphereUI(std::shared_ptr<SharedShapeCreationParams> sharedParams, CommandHistory& history) : CreateShapeUI(std::move(sharedParams), history) {}

    void Draw() override
    {
        if (sharedParams->selectedNode == nullptr) { return; }

        if (ImGui::TreeNode("Add sphere"))
        {
            ImGui::SliderFloat("Radius", &radius, 0.0f, static_cast<float>(ofGetWidth()));

            if (ImGui::Button("Add"))
            {
                auto sphere = ofSpherePrimitive();
                sphere.setRadius(radius);

                auto& mesh = sphere.getMesh();

                ofFloatColor color(sharedParams->color[0], sharedParams->color[1], sharedParams->color[2], sharedParams->color[3]);
                for (size_t i = 0; i < mesh.getNumVertices(); ++i)
                {
                    mesh.addColor(color);
                }

                auto sphere_ptr = std::make_shared<Node>("Sphere", std::make_shared<ofSpherePrimitive>(sphere));
                history.executeCommand(std::make_shared<AddChildToNodeCommand>(*sharedParams->selectedNode, sphere_ptr));

                radius = DEFAULT_SPHERE_RADIUS;
            }

            ImGui::TreePop();
        }
    }

private:
    float radius = DEFAULT_SPHERE_RADIUS;
};
