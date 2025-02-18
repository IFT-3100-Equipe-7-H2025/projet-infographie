#include "Scene3D.h"
#include "imgui.h"
#include "ofAppRunner.h"


void Scene3D::ShowChildren(const std::shared_ptr<Node>& node)
{
    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_DefaultOpen;
    if (selectedNode == node)
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


void Scene3D::draw()
{
    ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(300, 400), ImGuiCond_FirstUseEver);
    ImGui::Begin("3D scene graph");
    ShowChildren(this->graph.GetRoot());
    ImGui::End();

    if (selectedNode != nullptr)
    {
        ImGui::SetNextWindowPos(ImVec2(320, 10), ImGuiCond_FirstUseEver);
        if (ImGui::Begin(selectedNode->GetName().c_str()))
        {
            if (ImGui::SliderFloat("TranslateX", &translateX, -(float) ofGetWidth(), (float) ofGetWidth()))
            {
                selectedNode->GetInner()->setPosition(translateX, selectedNode->GetInner()->getPosition().y, selectedNode->GetInner()->getPosition().z);
            }

            if (ImGui::SliderFloat("TranslateY", &translateY, -(float) ofGetHeight(), (float) ofGetHeight()))
            {
                selectedNode->GetInner()->setPosition(selectedNode->GetInner()->getPosition().x, translateY, selectedNode->GetInner()->getPosition().z);
            }

            if (ImGui::CollapsingHeader("Add child", ImGuiTreeNodeFlags_DefaultOpen))
            {
                if (ImGui::Button("Add cube"))
                {
                    auto cube = std::make_shared<Node>("Cube", std::make_shared<ofBoxPrimitive>(ofBoxPrimitive(100.0f, 100.0f, 100.0f)));
                    selectedNode->AddChild(cube);
                }
            }

            ImGui::End();
        }
    }

    graph.Draw();
}
