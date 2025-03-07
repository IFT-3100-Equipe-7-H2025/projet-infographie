#pragma once
#include "3dscene/commands/AddShapeChildToNodeCommand.h"
#include "CreateShapeUI.h"
#include "imgui.h"
#include "ofAppRunner.h"
#include "Light.h"

class CreateLightUI : public CreateShapeUI
{
public:
    CreateLightUI(std::shared_ptr<SharedShapeCreationParams> sharedParams, CommandHistory& history) : CreateShapeUI(std::move(sharedParams), history) {}

    void Draw() override
    {
        if (sharedParams->selectedNode == nullptr) { return; }

        if (ImGui::TreeNode("Add light"))
        {
            if (ImGui::Button("Add"))
            {
                auto light = ofLight();
                light.enable();
                light.setDiffuseColor(ofFloatColor(1.0, 1.0, 1.0));
                light.setSpecularColor(ofFloatColor(1.0, 1.0, 1.0));
                light.setAmbientColor(ofFloatColor(0.2, 0.2, 0.2));
                light.lookAt(ofVec3f((float) ofGetWidth() / 2.0f, (float) ofGetHeight() / 2.0f, 0));

                auto light_ptr = std::make_shared<Node>("Light", std::make_shared<ofLight>(light));

                history.executeCommand(std::make_shared<AddChildToNodeCommand>(*sharedParams->selectedNode, light_ptr));
            }

            ImGui::TreePop();
        }
    }

private:
};
