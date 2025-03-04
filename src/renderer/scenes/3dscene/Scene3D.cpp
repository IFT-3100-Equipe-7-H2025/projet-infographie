#include "Scene3D.h"

#include "3dscene/commands/SetPositionCommand.h"
#include "3dscene/commands/SetRotationCommand.h"
#include "3dscene/commands/SetScaleCommand.h"
#include "3dscene/createShapes/CreateCubeUI.h"
#include "3dscene/createShapes/CreateLightUI.h"
#include "3dscene/createShapes/CreateSphereUI.h"
#include "imgui.h"
#include "ofAppRunner.h"
#include "ofGraphics.h"
#include <ranges>

Scene3D::Scene3D() : history(CommandHistory()),
                     graph(SceneGraph()),
                     selectedNode(std::make_shared<std::shared_ptr<Node>>(nullptr)),
                     sharedParams(std::make_shared<SharedShapeCreationParams>())
{
    this->sharedParams->selectedNode = this->selectedNode;
}


void Scene3D::setup()
{
    this->SelectNode(this->graph.GetRoot());

    this->createShapeUIs.push_back(std::make_unique<CreateCubeUI>(CreateCubeUI(this->sharedParams, this->history)));
    this->createShapeUIs.emplace_back(std::make_unique<CreateSphereUI>(CreateSphereUI(this->sharedParams, this->history)));
    this->createShapeUIs.emplace_back(std::make_unique<CreateLightUI>(CreateLightUI(this->sharedParams, this->history)));

    material.setDiffuseColor(ofFloatColor(1.0, 0.5, 0.5));
    material.setSpecularColor(ofFloatColor(1.0, 1.0, 1.0));
    material.setShininess(64);

    // Add base light
    auto light = ofLight();
    light.enable();
    light.setDiffuseColor(ofFloatColor(1.0, 1.0, 1.0));
    light.setSpecularColor(ofFloatColor(1.0, 1.0, 1.0));
    light.setAmbientColor(ofFloatColor(0.2, 0.2, 0.2));
    light.lookAt(ofVec3f((float) ofGetWidth() / 2.0f, (float) ofGetHeight() / 2.0f, 0));
    auto light_ptr = std::make_shared<Node>("Light", std::make_shared<ofLight>(light));
    this->graph.AddNode(light_ptr);
}

void Scene3D::draw()
{
    this->DrawSceneGraphWindow();
    this->DrawSelectedNodeWindow();
    this->DrawCommandHistoryWindow();

    material.begin();
    graph.Draw();
    material.end();
}

void Scene3D::DrawSceneGraphWindow()
{
    ImGui::SetNextWindowPos(ImVec2(10, 30), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(300, 400), ImGuiCond_FirstUseEver);
    ImGui::Begin("3D scene graph");
    this->ShowChildren(this->graph.GetRoot());
    ImGui::End();
}

void Scene3D::DrawSelectedNodeWindow()
{
    if ((*this->selectedNode) != nullptr)
    {
        ImGui::SetNextWindowPos(ImVec2(320, 30), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(300, 400), ImGuiCond_FirstUseEver);

        if (ImGui::Begin((*this->selectedNode)->GetName().c_str()))
        {
            this->DrawModifyNodeSliders(*this->selectedNode);

            if (ImGui::CollapsingHeader("Add child", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::ColorEdit4("Color", sharedParams->color);

                for (auto& createShapeUI: this->createShapeUIs)
                {
                    createShapeUI->Draw();
                }
            }

            ImGui::End();
        }
    }
}

void Scene3D::DrawModifyNodeSliders(const std::shared_ptr<Node>& node)
{
    const std::shared_ptr<ofNode>& inner = node->GetInner();

    // Positions
    const glm::vec3 currentPosition = inner->getPosition();
    if (ImGui::SliderFloat3("Translate", this->translate, -static_cast<float>(ofGetWidth()), static_cast<float>(ofGetWidth())))
    {
        inner->setPosition(this->translate[0], this->translate[1], this->translate[2]);
    }
    if (ImGui::IsItemActivated())
    {
        this->initialPosition = currentPosition;
    }
    if (ImGui::IsItemDeactivatedAfterEdit())
    {
        this->history.executeCommand(std::make_shared<SetPositionCommand>(node, glm::vec3(this->translate[0], this->translate[1], this->translate[2]), this->initialPosition));
    }

    // Rotations
    const glm::quat currentRotation = inner->getOrientationQuat();
    if (ImGui::SliderFloat3("Rotate", this->rotate, -360.0f, 360.0f))
    {
        glm::quat newRotation = glm::quat(glm::radians(glm::vec3(this->rotate[0], this->rotate[1], this->rotate[2])));
        inner->setOrientation(newRotation);
    }
    if (ImGui::IsItemActivated())
    {
        this->initialRotation = currentRotation;
    }
    if (ImGui::IsItemDeactivatedAfterEdit())
    {
        glm::quat newRotation = glm::quat(glm::radians(glm::vec3(this->rotate[0], this->rotate[1], this->rotate[2])));
        this->history.executeCommand(std::make_shared<SetRotationCommand>(node, newRotation, this->initialRotation));
    }

    // Scales
    const glm::vec3 currentScale = inner->getScale();
    if (ImGui::SliderFloat3("Scale", this->scale, 0.001f, 100.0f))
    {
        inner->setScale(this->scale[0], this->scale[1], this->scale[2]);
    }
    if (ImGui::IsItemActivated())
    {
        this->initialScale = currentScale;
    }
    if (ImGui::IsItemDeactivatedAfterEdit())
    {
        this->history.executeCommand(std::make_shared<SetScaleCommand>(node, glm::vec3(this->scale[0], this->scale[1], this->scale[2]), this->initialScale));
    }
}

void Scene3D::DrawCommandHistoryWindow()
{
    ImGui::SetNextWindowPos(ImVec2(10, 440), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(300, 400), ImGuiCond_FirstUseEver);
    ImGui::Begin("Command history");

    // Buttons for undo and redo
    auto& undoStack = this->history.GetUndoStack();
    auto& redoStack = this->history.GetRedoStack();

    ImGui::BeginDisabled(undoStack.size() == 0);
    if (ImGui::Button("Undo"))
    {
        this->history.undo();
        this->ResetParams(*this->selectedNode);
    }
    ImGui::EndDisabled();

    ImGui::SameLine();
    ImGui::BeginDisabled(redoStack.size() == 0);
    if (ImGui::Button("Redo"))
    {
        this->history.redo();
        this->ResetParams(*this->selectedNode);
    }
    ImGui::EndDisabled();

    // Undo and redo history
    if (ImGui::BeginTable("UndoRedoTable", 2, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_Resizable))
    {
        if (ImGui::TableNextColumn())
        {
            ImGui::SeparatorText("Undo history");

            for (auto& it: std::ranges::reverse_view(undoStack))
            {
                ImGui::TextUnformatted(it->DisplayName().c_str());
            }
        }

        if (ImGui::TableNextColumn())
        {
            ImGui::SeparatorText("Redo history");

            for (auto& it: std::ranges::reverse_view(redoStack))
            {
                ImGui::TextUnformatted(it->DisplayName().c_str());
            }
        }
        ImGui::EndTable();
    }

    ImGui::End();
}

void Scene3D::SelectNode(const std::shared_ptr<Node>& node)
{
    *selectedNode = node;

    this->ResetParams(*this->selectedNode);

    node->SetOpen(!node->IsOpen());
}

void Scene3D::ShowChildren(const std::shared_ptr<Node>& node)
{
    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_DefaultOpen;
    if (*selectedNode == node)
    {
        flags |= ImGuiTreeNodeFlags_Selected;
    }
    if (node->GetChildren().empty())
    {
        flags |= ImGuiTreeNodeFlags_Leaf;
    }

    bool nodeOpen = ImGui::TreeNodeEx(node->GetName().c_str(), flags);

    if (ImGui::IsItemClicked())
    {
        this->SelectNode(node);
    }

    if (nodeOpen)
    {
        for (const auto& child: node->GetChildren())
        {
            ShowChildren(child);
        }
        ImGui::TreePop();
    }
}

void Scene3D::ResetParams(const std::shared_ptr<Node>& node)
{
    const std::shared_ptr<ofNode>& inner = node->GetInner();

    const glm::vec3 currentPosition = inner->getPosition();
    translate[0] = currentPosition.x;
    translate[1] = currentPosition.y;
    translate[2] = currentPosition.z;

    const glm::vec3 currentScale = inner->getScale();
    scale[0] = currentScale.x;
    scale[1] = currentScale.y;
    scale[2] = currentScale.z;

    const glm::quat currentRotation = inner->getOrientationQuat();
    glm::vec3 eulerRotation = glm::degrees(glm::eulerAngles(currentRotation));
    rotate[0] = eulerRotation.x;
    rotate[1] = eulerRotation.y;
    rotate[2] = eulerRotation.z;
}
