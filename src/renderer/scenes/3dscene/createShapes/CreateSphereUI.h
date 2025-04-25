#pragma once

#include "3dscene/commands/AddShapeChildToNodeCommand.h"
#include "CreateShapeUI.h"
#include "imgui.h"
#include "of3dPrimitives.h"
#include "Primitive3D.h"
#include "../Sphere.h"
#include "renderer/PrimitiveCreator.h"
#include "RayMesh.h"


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
                auto sphere = PrimitiveCreator::createSphere(30, 30, radius);

                auto& mesh = sphere.getMesh();

                ofFloatColor color(sharedParams->color[0], sharedParams->color[1], sharedParams->color[2], sharedParams->color[3]);
                for (size_t i = 0; i < mesh.getNumVertices(); ++i)
                {
                    mesh.addColor(color);
                }
                ofLog() << "Creating sphere" << endl;
                bool isGlass = sharedParams->isGlass;
                shared_ptr<Material> mat = std::make_shared<Dielectric>(1.50);
                if (!isGlass)
                {
                    mat = std::make_shared<Metal>(color, 0.1);
                    ofLog() << "Metal" << endl;
                }
                //else if (randint == 2) {
                //    mat = std::make_shared<Lambert>(color);
                //    ofLog() << "Lambert" << endl;
                //}
                else {
                    ofLog() << "Glass" << endl;
                }
                auto sphere_scene = RayMesh(mat, sphere);
                //auto sphere_scene = Sphere(sphere, radius, mat);
                auto sphere_ptr = std::make_shared<Node>("Sphere", std::make_shared<RayMesh>(sphere_scene));
                history.executeCommand(std::make_shared<AddChildToNodeCommand>(*sharedParams->selectedNode, sphere_ptr));
            }

            ImGui::TreePop();
        }
    }

private:
    float radius = DEFAULT_SPHERE_RADIUS;
};
