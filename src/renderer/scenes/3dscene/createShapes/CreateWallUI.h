#pragma once

#include "3dscene/commands/AddShapeChildToNodeCommand.h"
#include "CreateShapeUI.h"
#include "Primitive3D.h"
#include "imgui.h"
#include "of3dPrimitives.h"
#include "renderer/PrimitiveCreator.h"
#include <memory>
#include <string>

class CreateWallUI : public CreateShapeUI
{
public:
    CreateWallUI(std::shared_ptr<SharedShapeCreationParams> sharedParams, CommandHistory& history)
        : CreateShapeUI(std::move(sharedParams), history) {}

    void Draw() override
    {
        if (sharedParams->selectedNode == nullptr) { return; }

        if (ImGui::TreeNode("Add wall from heightmap"))
        {

            // Heightmap path picker using ofSystemLoadDialog
            if (ImGui::Button("Select Heightmap"))
            {
                ofFileDialogResult result = ofSystemLoadDialog("Select heightmap image");
                if (result.bSuccess)
                {
                    strncpy(heightmapPath, result.getPath().c_str(), IM_ARRAYSIZE(heightmapPath));
                }
            }
            ImGui::Text("Heightmap: %s", heightmapPath);

            ImGui::SliderFloat("Width", &wallWidth, 0.1f, 100.0f);
            ImGui::SliderFloat("Height", &wallHeight, 0.1f, 100.0f);
            ImGui::SliderFloat("Depth", &wallDepth, 0.1f, 100.0f);
            ImGui::SliderFloat("Heightmap Scale", &heightmapScale, 0.0f, 10.0f);
            ImGui::SliderInt("Resolution X", &resolutionX, 2, 500);
            ImGui::SliderInt("Resolution Y", &resolutionY, 2, 500);

            if (ImGui::Button("Add"))
            {
                try
                {
                    auto wall = PrimitiveCreator::createWall(
                            std::string(heightmapPath),
                            wallWidth,
                            wallHeight,
                            wallDepth,
                            heightmapScale,
                            resolutionX,
                            resolutionY);

                    auto wall3D = Primitive3D(wall);
                    wall3D.SetColor(ofFloatColor(sharedParams->color[0], sharedParams->color[1], sharedParams->color[2], sharedParams->color[3]));

                    auto wall_ptr = std::make_shared<Node>("Wall", std::make_shared<Primitive3D>(wall3D));
                    history.executeCommand(std::make_shared<AddChildToNodeCommand>(*sharedParams->selectedNode, wall_ptr));
                } catch (std::exception& e)
                {
                    std::cout << "Error creating wall: " << e.what() << std::endl;
                }
            }

            ImGui::TreePop();
        }
    }

private:
    char heightmapPath[256] = "";
    float wallWidth = 10.0f;
    float wallHeight = 10.0f;
    float wallDepth = 1.0f;
    float heightmapScale = 1.0f;
    int resolutionX = 100;
    int resolutionY = 100;
};
