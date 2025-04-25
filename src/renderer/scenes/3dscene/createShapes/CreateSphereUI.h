#pragma once

#include "3dscene/commands/AddShapeChildToNodeCommand.h"
#include "CreateShapeUI.h"
#include "imgui.h"
#include "of3dPrimitives.h"
#include "Primitive3D.h"
#include "../Sphere.h"
#include "renderer/PrimitiveCreator.h"
#include "RayMesh.h"
#include "BvhNode.h"


constexpr float DEFAULT_SPHERE_RADIUS = 100.0f;
constexpr int DEFAULT_SPHERE_RESOLUTION_LAT = 40;
constexpr int DEFAULT_SPHERE_RESOLUTION_LON = 2;

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
            ImGui::SliderInt("Resolution Lat", &resolution_lat, 2, 100);
            ImGui::SliderInt("Resolution Lon", &resolution_lon, 2, 100);

            if (ImGui::Button("Add"))
            {
                ofFloatColor color(sharedParams->color[0], sharedParams->color[1], sharedParams->color[2], sharedParams->color[3]);

                shared_ptr<Material> mat = sharedParams->material;
                ofLog() << "Material Color" << mat->getColor();



                RayMesh sphere = PrimitiveCreator::createSphere(resolution_lat, resolution_lon, radius, mat);
                //RayMesh sphere = PrimitiveCreator::createSphere(30, 30, radius, mat);
                //of3dPrimitive sphere = PrimitiveCreator::createSphere(30, 30, radius);
                ofMesh mesh = sphere.getMesh();
                //auto& mesh = sphere.getMesh();

                for (size_t i = 0; i < mesh.getNumVertices(); ++i)
                {
                    mesh.addColor(color);
                }
                
                //auto sphere_scene = RayMesh(mat, sphere);
                //auto sphere_scene = Sphere(sphere, radius, mat);
                ComposedShape shape = ComposedShape(make_shared<BvhNode>(sphere, 11), mat);
                auto sphere_ptr = std::make_shared<Node>("Sphere", std::make_shared<ComposedShape>(shape));
                //auto sphere_ptr = std::make_shared<Node>("Sphere", std::make_shared<Sphere>(sphere_scene));
                history.executeCommand(std::make_shared<AddChildToNodeCommand>(*sharedParams->selectedNode, sphere_ptr));
            }

            ImGui::TreePop();
        }
    }

private:
    float radius = DEFAULT_SPHERE_RADIUS;
    int resolution_lat = DEFAULT_SPHERE_RESOLUTION_LAT;
    int resolution_lon = DEFAULT_SPHERE_RESOLUTION_LON;
};
