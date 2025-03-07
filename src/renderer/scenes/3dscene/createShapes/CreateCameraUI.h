#pragma once
#pragma once
#include "3dscene/commands/AddShapeChildToNodeCommand.h"
#include "CreateShapeUI.h"
#include "Light.h"
#include "imgui.h"
#include "ofAppRunner.h"

class CreateCameraUI : public CreateShapeUI
{
public:
    CreateCameraUI(std::shared_ptr<SharedShapeCreationParams> sharedParams, CommandHistory& history) : CreateShapeUI(std::move(sharedParams), history) {}

    void Draw() override
    {
        if (sharedParams->selectedNode == nullptr) { return; }

        if ( ImGui::TreeNode("Add camera") )
        {
            if (ImGui::Button("Add"))
            {
                auto camera = std::make_shared<ofCamera>();
                camera->setPosition(0, 0, -200);
                camera->lookAt(ofVec3f(0, 0, 0));
                auto cam_ptr = std::make_shared<Node>("Camera", camera);


                history.executeCommand(std::make_shared<AddChildToNodeCommand>(*sharedParams->selectedNode, cam_ptr));
            }

            ImGui::TreePop();
        }
    }

private:
};
