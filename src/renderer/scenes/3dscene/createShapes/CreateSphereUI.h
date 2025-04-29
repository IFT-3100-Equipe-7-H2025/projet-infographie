#pragma once

#include "3dscene/commands/AddShapeChildToNodeCommand.h"
#include "CreateShapeUI.h"
#include "Primitive3D.h"
#include "imgui.h"
#include "of3dPrimitives.h"
#include "Primitive3D.h"
#include "../Sphere.h"
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
                auto sphere = PrimitiveCreator::createSphere(30, 30, radius);
                auto sphere_3d = Primitive3D(sphere);
                sphere_3d.SetColor(ofFloatColor(sharedParams->color[0], sharedParams->color[1], sharedParams->color[2], sharedParams->color[3]));

                auto sphere_ptr = std::make_shared<Node>("Sphere", std::make_shared<Primitive3D>(sphere_3d));
                ofFloatColor color(sharedParams->color[0], sharedParams->color[1], sharedParams->color[2], sharedParams->color[3]);
                ofLog() << "Material Color" << color;
                shared_ptr<Material> mat = sharedParams->material->clone();

                ofLog() << "Material Color" << mat->getColor();



                of3dPrimitive prim = PrimitiveCreator::createSphere(resolution_lat, resolution_lon, radius);
                //RayMesh sphere = PrimitiveCreator::createSphere(30, 30, radius, mat);
                //of3dPrimitive sphere = PrimitiveCreator::createSphere(30, 30, radius);
                //ofMesh mesh = sphere.getMesh();
                auto& mesh = prim.getMesh();
                
                RayMesh sphere(mat, prim);
                auto sphere_ptr = std::make_shared<Node>("Sphere", std::make_shared<RayMesh>(sphere));

                if (sharedParams->useBVH) {
                    ComposedShape shape = ComposedShape(make_shared<BvhNode>(sphere), mat);
                    sphere_ptr = std::make_shared<Node>("Sphere", std::make_shared<ComposedShape>(shape));
                    ofLog() << "Using BVH";

                }
                else {
                    ofLog() << "Not uing BVH";

                }
                
                
                //Sphere sphere_scene();
                sphere_ptr = std::make_shared<Node>("Sphere", std::make_shared<Sphere>(prim, radius, mat));
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
