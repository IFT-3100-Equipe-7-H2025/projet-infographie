#pragma once
#include "3dscene/commands/AddShapeChildToNodeCommand.h"
#include "CreateShapeUI.h"
#include "imgui.h"
#include "of3dPrimitives.h"

constexpr float DEFAULT_CUBE_WIDTH = 100.0f;
constexpr float DEFAULT_CUBE_HEIGHT = 100.0f;
constexpr float DEFAULT_CUBE_DEPTH = 100.0f;

class CreateCubeUI : public CreateShapeUI
{
public:
    CreateCubeUI(std::shared_ptr<SharedShapeCreationParams> sharedParams, CommandHistory& history) : CreateShapeUI(std::move(sharedParams), history) {}

    void Draw() override
    {
        if (sharedParams->selectedNode == nullptr) { return; }

        if (ImGui::TreeNode("Add cube"))
        {
            ImGui::SliderFloat("Width", &width, 0.0f, static_cast<float>(ofGetWidth()));
            ImGui::SliderFloat("Height", &height, 0.0f, static_cast<float>(ofGetWidth()));
            ImGui::SliderFloat("Depth", &depth, 0.0f, static_cast<float>(ofGetWidth()));

            if (ImGui::Button("Add"))
            {
                auto cube = ofBoxPrimitive(width, height, depth);
                for (int i = 0; i < 6; i++) { cube.setSideColor(i, ofColor(sharedParams->color[0] * 255, sharedParams->color[1] * 255, sharedParams->color[2] * 255, sharedParams->color[3] * 255)); }
                auto cube_ptr = std::make_shared<Node>("Cube", std::make_shared<ofBoxPrimitive>(cube));

                history.executeCommand(std::make_shared<AddChildToNodeCommand>(*sharedParams->selectedNode, cube_ptr));

                width = DEFAULT_CUBE_WIDTH;
                height = DEFAULT_CUBE_HEIGHT;
                depth = DEFAULT_CUBE_DEPTH;
            }

            ImGui::TreePop();
        }
    }

private:
    float width = DEFAULT_CUBE_WIDTH;
    float height = DEFAULT_CUBE_HEIGHT;
    float depth = DEFAULT_CUBE_DEPTH;
};
