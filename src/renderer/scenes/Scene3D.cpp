#include "Scene3D.h"
#include "imgui.h"
#include "ofAppRunner.h"


void Scene3D::ShowChildren(const std::shared_ptr<Node>& node)
{
    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
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
    ImGui::Begin("3D scene graph");
    ShowChildren(this->graph.GetRoot());
    ImGui::End();

    if (selectedNode != nullptr)
    {
        if (ImGui::Begin(selectedNode->GetName().c_str()))
        {
            if (ImGui::SliderFloat("TranslateX", &translateX, 0, (float) ofGetWidth()))
            {
                selectedNode->GetInner()->setPosition(translateX, selectedNode->GetInner()->getPosition().y, selectedNode->GetInner()->getPosition().z);
            }

            if (ImGui::SliderFloat("TranslateY", &translateY, 0, (float) ofGetHeight()))
            {
                selectedNode->GetInner()->setPosition(selectedNode->GetInner()->getPosition().x, translateY, selectedNode->GetInner()->getPosition().z);
            }

            ImGui::End();
        }
    }

    graph.Draw();
}
