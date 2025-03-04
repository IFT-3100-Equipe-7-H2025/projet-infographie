#include "Scene3D.h"

#include "3dscene/createShapes/CreateCubeUI.h"
#include "imgui.h"
#include "ofAppRunner.h"
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
}

void Scene3D::draw()
{
    this->DrawSceneGraphUI();
    this->DrawSelectedNodeUI();
    this->DrawCommandHistoryUI();

    graph.Draw();
}

void Scene3D::DrawSceneGraphUI()
{
    ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(300, 400), ImGuiCond_FirstUseEver);
    ImGui::Begin("3D scene graph");

    this->ShowChildren(this->graph.GetRoot());


    ImGui::End();
}

void Scene3D::DrawSelectedNodeUI()
{
    if ((*this->selectedNode) != nullptr)
    {
        ImGui::SetNextWindowPos(ImVec2(320, 10), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(300, 400), ImGuiCond_FirstUseEver);

        if (ImGui::Begin((*this->selectedNode)->GetName().c_str()))
        {
            if (ImGui::SliderFloat("TranslateX", &this->translateX, -static_cast<float>(ofGetWidth()), static_cast<float>(ofGetWidth())))
            {
                (*this->selectedNode)->GetInner()->setPosition(this->translateX, (*this->selectedNode)->GetInner()->getPosition().y, (*this->selectedNode)->GetInner()->getPosition().z);
            }

            if (ImGui::SliderFloat("TranslateY", &translateY, -static_cast<float>(ofGetHeight()), static_cast<float>(ofGetHeight())))
            {
                (*this->selectedNode)->GetInner()->setPosition((*this->selectedNode)->GetInner()->getPosition().x, translateY, (*this->selectedNode)->GetInner()->getPosition().z);
            }

            if (ImGui::CollapsingHeader("Add child", ImGuiTreeNodeFlags_DefaultOpen))
            {
                if (ImGui::SliderFloat("r", &this->sharedParams->r, 0, 255)) { this->sharedParams->r = std::clamp(this->sharedParams->r, 0.0f, 255.0f); }
                if (ImGui::SliderFloat("g", &this->sharedParams->g, 0, 255)) { this->sharedParams->g = std::clamp(this->sharedParams->g, 0.0f, 255.0f); }
                if (ImGui::SliderFloat("b", &this->sharedParams->b, 0, 255)) { this->sharedParams->b = std::clamp(this->sharedParams->b, 0.0f, 255.0f); }

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
    ImGui::SetNextWindowPos(ImVec2(10, 420), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(300, 400), ImGuiCond_FirstUseEver);
    ImGui::Begin("Command history");

    // Buttons
    bool undoDisabled = this->history.GetUndoStack().size() == 0;
    ImGui::BeginDisabled(undoDisabled);
    if (ImGui::Button("Undo"))
    {
        this->history.undo();
    }
    ImGui::EndDisabled();

    ImGui::SameLine();

    bool redoDisabled = this->history.GetRedoStack().size() == 0;
    ImGui::BeginDisabled(redoDisabled);
    if (ImGui::Button("Redo"))
    {
        this->history.redo();
    }
    ImGui::EndDisabled();

    ImGui::Separator();

    // Undo and redo history
    if (ImGui::BeginTable("UndoRedoTable", 2, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_Resizable))
    {
        if (ImGui::TableNextColumn())
        {
            ImGui::TextUnformatted("Undo");

            auto& undoStack = this->history.GetUndoStack();
            for (auto& it: std::ranges::reverse_view(undoStack))
            {
                ImGui::TextUnformatted(it->DisplayName().c_str());
            }
        }

        if (ImGui::TableNextColumn())
        {
            ImGui::TextUnformatted("Redo");

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

    translateX = node->GetInner()->getPosition().x;
    translateY = node->GetInner()->getPosition().y;

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
