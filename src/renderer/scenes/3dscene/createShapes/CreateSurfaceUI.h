#pragma once

#include "3dscene/commands/AddShapeChildToNodeCommand.h"
#include "CreateShapeUI.h"
#include "Primitive3D.h"
#include "imgui.h"
#include "of3dPrimitives.h"
#include "renderer/PrimitiveCreator.h"
#include <numbers>


constexpr float DEFAULT_STARTING_U = 0;
constexpr float DEFAULT_END_U = 2 * numbers::pi;
constexpr float DEFAULT_STARTING_V = 0;
constexpr float DEFAULT_END_V = 2 * numbers::pi;
constexpr int DEFAULT_RESOLUTION = 100;

class CreateSurfaceUI : public CreateShapeUI
{
public:
    CreateSurfaceUI(std::shared_ptr<SharedShapeCreationParams> sharedParams, CommandHistory& history) : CreateShapeUI(std::move(sharedParams), history) {}

    void Draw() override
    {
        if (sharedParams->selectedNode == nullptr) { return; }

        if (ImGui::TreeNode("Add surface"))
        {
            ImGui::SliderInt("Resolution", &resolution, 2, 200);
            ImGui::SliderFloat("Starting U", &starting_u, -200.0f, 200.0f);
            ImGui::SliderFloat("Ending U", &end_u, -200.0f, 200.0f);
            ImGui::SliderFloat("Starting V", &starting_v, -200.0f, 200.0f);
            ImGui::SliderFloat("Ending V", &end_v, -200.0f, 200.0f);
            ImGui::InputText("X", x, 256);
            ImGui::InputText("Y", y, 256);
            ImGui::InputText("Z", z, 256);

            if (ImGui::Button("Add"))
            {
                try
                {
                    auto surface = PrimitiveCreator::createSurface(starting_u, end_u, starting_v, end_v, resolution, std::string(x), std::string(y), std::string(z));
                    auto surface_3d = Primitive3D(surface);
                    surface_3d.SetColor(ofFloatColor(sharedParams->color[0], sharedParams->color[1], sharedParams->color[2], sharedParams->color[3]));

                    auto surface_ptr = std::make_shared<Node>("Surface", std::make_shared<Primitive3D>(surface_3d));
                    history.executeCommand(std::make_shared<AddChildToNodeCommand>(*sharedParams->selectedNode, surface_ptr));
                } catch (exception& e)
                {
                    std::cout << "Invalid expression" << std::endl;
                }
            }

            ImGui::TreePop();
        }
    }

private:
    int resolution = DEFAULT_RESOLUTION;
    float starting_u = DEFAULT_STARTING_U;
    float starting_v = DEFAULT_STARTING_V;
    float end_u = DEFAULT_END_U;
    float end_v = DEFAULT_END_V;
    char x[256] = "";
    char y[256] = "";
    char z[256] = "";
};
