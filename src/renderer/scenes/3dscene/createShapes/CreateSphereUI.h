#pragma once

#include "3dscene/commands/AddShapeChildToNodeCommand.h"
#include "CreateShapeUI.h"
#include "Primitive3D.h"
#include "imgui.h"
#include "of3dPrimitives.h"
#include "Primitive3D.h"
#include "rayTracer/RayObjects/Sphere.h"
#include "renderer/PrimitiveCreator.h"
#include "RayMesh.h"
#include "BvhNode.h"


constexpr float DEFAULT_SPHERE_RADIUS = 100.0f;
constexpr int DEFAULT_SPHERE_RESOLUTION_LAT = 20;
constexpr int DEFAULT_SPHERE_RESOLUTION_LON = 20;

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
            ImGui::SliderInt("Resolution Lat", &resolution_lat, 3, 100);
            ImGui::SliderInt("Resolution Lon", &resolution_lon, 3, 100);

            if (ImGui::Button("Add"))
            {
                shared_ptr<MaterialContainer> mat = sharedParams->material->clone();

                auto sphere = PrimitiveCreator::createSphere(resolution_lat, resolution_lon, radius);
                Sphere sphere_3d(sphere, radius, mat);
                sphere_3d.SetColor(ofFloatColor(sharedParams->color[0], sharedParams->color[1], sharedParams->color[2], sharedParams->color[3]));

                //RayMesh rayMesh(mat, sphere);
                //auto sphere_ptr = std::make_shared<Node>("Sphere", std::make_shared<RayMesh>(rayMesh));

                //if (sharedParams->useBVH)
                //{
                //    ComposedShape shape = ComposedShape(make_shared<BvhNode>(rayMesh), mat);
                //    sphere_ptr = std::make_shared<Node>("Sphere", std::make_shared<ComposedShape>(shape));
                //}
                auto sphere_ptr = std::make_shared<Node>("Sphere", std::make_shared<Sphere>(sphere_3d));

                history.executeCommand(std::make_shared<AddChildToNodeCommand>(*sharedParams->selectedNode, sphere_ptr));




 /*               of3dPrimitive prim = PrimitiveCreator::createSphere(resolution_lat, resolution_lon, radius);
                RayMesh sphere_3d(mat, sphere);
                
                auto sphere_ptr = std::make_shared<Node>("Sphere", std::make_shared<RayMesh>(sphere));

                if (sharedParams->useBVH) {
                    ComposedShape shape = ComposedShape(make_shared<BvhNode>(sphere), mat);
                    sphere_ptr = std::make_shared<Node>("Sphere", std::make_shared<ComposedShape>(shape));
                }*/
                
               
            }

            ImGui::TreePop();
        }
    }

private:
    float radius = DEFAULT_SPHERE_RADIUS;
    int resolution_lat = DEFAULT_SPHERE_RESOLUTION_LAT;
    int resolution_lon = DEFAULT_SPHERE_RESOLUTION_LON;
};
