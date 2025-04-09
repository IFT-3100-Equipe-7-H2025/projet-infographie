#pragma once

#include "3dscene/commands/AddShapeChildToNodeCommand.h"
#include "CreateShapeUI.h"
#include "Primitive3D.h"
#include "imgui.h"
#include "of3dPrimitives.h"
#include "renderer/PrimitiveCreator.h"

constexpr float DEFAULT_LASAGNA_L_W_RATIO = 0.5;
constexpr int DEFAULT_LASAGNA_PERIODS = 4;
constexpr int DEFAULT_LASAGNA_RESOLUTION_L = 40;
constexpr int DEFAULT_LASAGNA_RESOLUTION_W = 2;
constexpr float DEFAULT_LASAGNA_WIDTH = 100.0f;
constexpr float DEFAULT_LASAGNA_HEIGHT = 40.0f;
constexpr float DEFAULT_LASAGNA_DEPTH = 20.0f;

class CreateLasagnaUI : public CreateShapeUI
{
public:
    CreateLasagnaUI(std::shared_ptr<SharedShapeCreationParams> sharedParams, CommandHistory& history) : CreateShapeUI(std::move(sharedParams), history) {}

    void Draw() override
    {
        if (sharedParams->selectedNode == nullptr) { return; }

        if (ImGui::TreeNode("Add lasagna"))
        {
            ImGui::SliderFloat("L/W ratio", &l_w_ratio, 0.0f, 1.0f);
            ImGui::SliderInt("Periods", &periods, 1.0f, 10.0f);
            ImGui::SliderInt("Resolution L", &resolution_l, 2, 100);
            ImGui::SliderInt("Resolution W", &resolution_w, 2, 100);
            ImGui::SliderFloat("Width", &width, 0.0f, static_cast<float>(ofGetWidth()));
            ImGui::SliderFloat("Height", &height, 0.0f, static_cast<float>(ofGetWidth()));
            ImGui::SliderFloat("Depth", &depth, 0.0f, static_cast<float>(ofGetWidth()));

            if (ImGui::Button("Add"))
            {
                auto lasagna = PrimitiveCreator::createLasagna(l_w_ratio, periods, resolution_l, resolution_w, width, height, depth);
                auto lasagna_3d = Primitive3D(lasagna);
                lasagna_3d.SetColor(ofFloatColor(sharedParams->color[0], sharedParams->color[1], sharedParams->color[2], sharedParams->color[3]));

                auto lasagna_ptr = std::make_shared<Node>("Lasagna", std::make_shared<Primitive3D>(lasagna_3d));
                history.executeCommand(std::make_shared<AddChildToNodeCommand>(*sharedParams->selectedNode, lasagna_ptr));
            }

            ImGui::TreePop();
        }
    }

private:
    float l_w_ratio = DEFAULT_LASAGNA_L_W_RATIO;
    int periods = DEFAULT_LASAGNA_PERIODS;
    int resolution_l = DEFAULT_LASAGNA_RESOLUTION_L;
    int resolution_w = DEFAULT_LASAGNA_RESOLUTION_W;

    float width = DEFAULT_LASAGNA_WIDTH;
    float height = DEFAULT_LASAGNA_HEIGHT;
    float depth = DEFAULT_LASAGNA_DEPTH;
};
