#pragma once

#include "3dscene/commands/AddShapeChildToNodeCommand.h"
#include "CreateShapeUI.h"
#include "imgui.h"
#include "ofAppRunner.h"
#include "renderer/PrimitiveCreator.h"

constexpr int DEFAULT_PYRAMID_SIDES = 4;
constexpr float DEFAULT_PYRAMID_WIDTH = 100.0f;
constexpr float DEFAULT_PYRAMID_HEIGHT = 100.0f;
constexpr float DEFAULT_PYRAMID_DEPTH = 100.0f;


class CreatePyramidUI : public CreateShapeUI
{
public:
    CreatePyramidUI(std::shared_ptr<SharedShapeCreationParams> sharedParams, CommandHistory& history) : CreateShapeUI(std::move(sharedParams), history) {}

    void Draw() override
    {
        if (sharedParams->selectedNode == nullptr) { return; }

        if (ImGui::TreeNode("Add pyramid"))
        {
            ImGui::SliderInt("Sides", &sides, 3, 10);
            ImGui::SliderFloat("Width", &width, 0.0f, static_cast<float>(ofGetWidth()));
            ImGui::SliderFloat("Height", &height, 0.0f, static_cast<float>(ofGetWidth()));
            ImGui::SliderFloat("Depth", &depth, 0.0f, static_cast<float>(ofGetWidth()));

            if (ImGui::Button("Add"))
            {
                auto pyramid = PrimitiveCreator::createPyramid(sides, width, height, depth);

                auto& mesh = pyramid.getMesh();

                ofFloatColor color(sharedParams->color[0], sharedParams->color[1], sharedParams->color[2], sharedParams->color[3]);
                for (size_t i = 0; i < mesh.getNumVertices(); ++i)
                {
                    mesh.addColor(color);
                }

                auto pyramid_3d = Primitive3D(pyramid);
                auto pyramid_ptr = std::make_shared<Node>("Pyramid", std::make_shared<Primitive3D>(pyramid_3d));

                history.executeCommand(std::make_shared<AddChildToNodeCommand>(*sharedParams->selectedNode, pyramid_ptr));
            }

            ImGui::TreePop();
        }
    }

private:
    int sides = DEFAULT_PYRAMID_SIDES;
    float width = DEFAULT_PYRAMID_WIDTH;
    float height = DEFAULT_PYRAMID_HEIGHT;
    float depth = DEFAULT_PYRAMID_DEPTH;
};
