#pragma once
#include "3dscene/commands/AddShapeChildToNodeCommand.h"
#include "CreateShapeUI.h"
#include "Primitive3D.h"
#include "imgui.h"
#include "of3dPrimitives.h"
#include "renderer/PrimitiveCreator.h"
#include "Cube.h"

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
                //auto cube = ofBoxPrimitive(width, height, depth);
                //for (int i = 0; i < 6; i++) { cube.setSideColor(i, ofFloatColor(sharedParams->color[0], sharedParams->color[1], sharedParams->color[2], sharedParams->color[3])); }

                auto cube = PrimitiveCreator::createCube(width, height, depth);
                ofMesh& mesh = cube.getMesh();

                ofFloatColor color(sharedParams->color[0], sharedParams->color[1], sharedParams->color[2], sharedParams->color[3]);
                for (size_t i = 0; i < mesh.getNumVertices(); ++i)
                {
                    mesh.addColor(color);
                }

                shared_ptr<Material> mat = sharedParams->material->clone();


                auto cube_3d = Primitive3D(cube);
                auto cube_ptr = std::make_shared<Node>("Cube", std::make_shared<Cube>(Vec3(width, height, depth), mat, cube));

                history.executeCommand(std::make_shared<AddChildToNodeCommand>(*sharedParams->selectedNode, cube_ptr));
            }

            ImGui::TreePop();
        }
    }

private:
    float width = DEFAULT_CUBE_WIDTH;
    float height = DEFAULT_CUBE_HEIGHT;
    float depth = DEFAULT_CUBE_DEPTH;
};
