#include "Scene3D.h"

#include "3dscene/commands/SetPositionCommand.h"
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
    this->DrawSceneGraphUI();
    this->DrawSelectedNodeUI();
    this->DrawCommandHistoryUI();

    material.begin();
    graph.Draw();
    material.end();
}

void Scene3D::DrawSceneGraphUI()
{
    ImGui::SetNextWindowPos(ImVec2(10, 30), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(300, 400), ImGuiCond_FirstUseEver);
    ImGui::Begin("3D scene graph");
    this->ShowChildren(this->graph.GetRoot());
    ImGui::End();
}

void Scene3D::DrawSelectedNodeUI()
{
    if ((*this->selectedNode) != nullptr)
    {
        ImGui::SetNextWindowPos(ImVec2(320, 30), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(300, 400), ImGuiCond_FirstUseEver);

        if (ImGui::Begin((*this->selectedNode)->GetName().c_str()))
        {
            const std::shared_ptr<ofNode>& inner = (*this->selectedNode)->GetInner();
            const glm::vec3 currentPosition = inner->getPosition();

            if (ImGui::SliderFloat("TranslateX", &this->translateX, -static_cast<float>(ofGetWidth()), static_cast<float>(ofGetWidth())))
            {
                inner->setPosition(this->translateX, currentPosition.y, currentPosition.z);
            }
            if (ImGui::IsItemActivated())
            {
                this->initialPosition = currentPosition;
            }
            if (ImGui::IsItemDeactivatedAfterEdit())
            {
                this->history.executeCommand(std::make_unique<SetPositionCommand>(*this->selectedNode, glm::vec3(this->translateX, currentPosition.y, currentPosition.z), this->initialPosition));
            }

            if (ImGui::SliderFloat("TranslateY", &this->translateY, -static_cast<float>(ofGetHeight()), static_cast<float>(ofGetHeight())))
            {
                inner->setPosition(currentPosition.x, this->translateY, currentPosition.z);
            }
            if (ImGui::IsItemActivated())
            {
                this->initialPosition = currentPosition;
            }
            if (ImGui::IsItemDeactivatedAfterEdit())
            {
                this->history.executeCommand(std::make_unique<SetPositionCommand>(*this->selectedNode, glm::vec3(currentPosition.x, this->translateY, currentPosition.z), this->initialPosition));
            }

            if (ImGui::SliderFloat("TranslateZ", &this->translateZ, -static_cast<float>(ofGetWidth()), static_cast<float>(ofGetWidth())))
            {
                inner->setPosition(currentPosition.x, currentPosition.y, this->translateZ);
            }
            if (ImGui::IsItemActivated())
            {
                this->initialPosition = currentPosition;
            }
            if (ImGui::IsItemDeactivatedAfterEdit())
            {
                this->history.executeCommand(std::make_unique<SetPositionCommand>(*this->selectedNode, glm::vec3(currentPosition.x, currentPosition.y, this->translateZ), this->initialPosition));
            }

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

void Scene3D::DrawCommandHistoryUI()
{
    ImGui::SetNextWindowPos(ImVec2(10, 440), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(300, 400), ImGuiCond_FirstUseEver);
    ImGui::Begin("Command history");

    // Buttons for undo and redo
    ImGui::BeginDisabled(this->history.GetUndoStack().size() == 0);
    if (ImGui::Button("Undo"))
    {
        this->history.undo();
    }
    ImGui::EndDisabled();

    ImGui::SameLine();
    ImGui::BeginDisabled(this->history.GetRedoStack().size() == 0);
    if (ImGui::Button("Redo"))
    {
        this->history.redo();
    }
    ImGui::EndDisabled();

    // Undo and redo history
    if (ImGui::BeginTable("UndoRedoTable", 2, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_Resizable))
    {
        if (ImGui::TableNextColumn())
        {
            ImGui::SeparatorText("Undo history");

            auto& undoStack = this->history.GetUndoStack();
            for (auto& it: std::ranges::reverse_view(undoStack))
            {
                ImGui::TextUnformatted(it->DisplayName().c_str());
            }
        }

        if (ImGui::TableNextColumn())
        {
            ImGui::SeparatorText("Redo history");

            auto& redoStack = this->history.GetRedoStack();
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

    glm::vec3 currentPosition = node->GetInner()->getPosition();
    translateX = currentPosition.x;
    translateY = currentPosition.y;

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
