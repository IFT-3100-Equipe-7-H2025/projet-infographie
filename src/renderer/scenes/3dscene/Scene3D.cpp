#include "Scene3D.h"

#include "3dscene/commands/RemoveNodeCommand.h"
#include "3dscene/commands/SetPositionCommand.h"
#include "3dscene/commands/SetRotationCommand.h"
#include "3dscene/commands/SetScaleCommand.h"
#include "3dscene/createShapes/CreateCameraUI.h"
#include "3dscene/createShapes/CreateCubeUI.h"
#include "3dscene/createShapes/CreateLasagnaUI.h"
#include "3dscene/createShapes/CreateLightUI.h"
#include "3dscene/createShapes/CreatePyramidUI.h"
#include "3dscene/createShapes/CreateSphereUI.h"
#include "Light.h"
#include "MoveChildCommand.h"
#include "SceneObject.h"
#include "SetCameraFovCommand.h"
#include "imgui.h"
#include "of3dPrimitives.h"
#include "ofAppRunner.h"
#include "ofGraphics.h"
#include "renderer/sceneObjects/ImportModel.h"
#include <cmath>
#include <numbers>
#include "renderer/rayTracer/ray.h"

#include <ranges>

Scene3D::Scene3D() : history(CommandHistory()),
                     sceneGraph(SceneGraph()),
                     selectedNode(std::make_shared<std::shared_ptr<Node>>(nullptr)),
                     sharedParams(std::make_shared<SharedShapeCreationParams>())
{
    this->sharedParams->selectedNode = this->selectedNode;
}


void Scene3D::setup()
{
    this->SelectNode(this->sceneGraph.GetRoot());

    this->createShapeUIs.push_back(std::make_unique<CreateCubeUI>(CreateCubeUI(this->sharedParams, this->history)));
    this->createShapeUIs.emplace_back(std::make_unique<CreateSphereUI>(CreateSphereUI(this->sharedParams, this->history)));
    this->createShapeUIs.emplace_back(std::make_unique<CreateLightUI>(CreateLightUI(this->sharedParams, this->history)));
    this->createShapeUIs.emplace_back(std::make_unique<CreateLasagnaUI>(CreateLasagnaUI(this->sharedParams, this->history)));
    this->createShapeUIs.push_back(std::make_unique<CreatePyramidUI>(CreatePyramidUI(this->sharedParams, this->history)));
    this->createShapeUIs.emplace_back(std::make_unique<CreateCameraUI>(CreateCameraUI(this->sharedParams, this->history)));

    material.setAmbientColor(ofFloatColor(0.1, 0.1, 0.1));
    material.setDiffuseColor(ofFloatColor(1.0, 0.5, 0.5));
    material.setSpecularColor(ofFloatColor(1.0, 1.0, 1.0));
    material.setShininess(64);

    // Add base light
    auto light = ofLight();
    light.enable();
    light.setDiffuseColor(ofFloatColor(1.0, 1.0, 1.0));
    light.setSpecularColor(ofFloatColor(1.0, 1.0, 1.0));
    light.setAmbientColor(ofFloatColor(0.1, 0.1, 0.1));
    light.lookAt(ofVec3f((float) ofGetWidth() / 2.0f, (float) ofGetHeight() / 2.0f, 0));

    auto light_ptr = std::make_shared<Node>("Light", std::make_shared<ofLight>(light));
    this->sceneGraph.AddNode(light_ptr);

    ofBoxPrimitive box(100, 100, 100);

    // Add base camera
    //cam

    camera = std::make_shared<Camera>();
    camera->setPosition(0, 0, -200);
    camera->lookAt(ofVec3f(0, 0, 0));
    camera->activate();
    camera->hideFrustrum();
    auto cam_ptr = std::make_shared<Node>("Camera", camera);
    this->sceneGraph.AddNode(cam_ptr);
    cameraMap.emplace(cam_ptr->GetId(), camera);
    updateViewPorts();

    translate_speed = 750;
    rotate_speed = 75;



    auto aspect_ratio = 16.0 / 9.0;
    int image_width = 400;
    int image_height = int(image_width / aspect_ratio);
    clearColor.set(0, 77, 98);
    rayImage.allocate(image_width, image_height, OF_IMAGE_COLOR);

    auto material_ground = make_shared<Lambert>(ofColor(204, 204, 0.0));
    auto material_center = make_shared<Lambert>(ofColor(26, 51, 125));
    auto material_left = make_shared<Dielectric>(1.50);
    auto material_bubble = make_shared<Dielectric>(1.00 / 1.50);
    auto material_right = make_shared<Metal>(ofColor(204, 153, 51), 1.0);

    auto primGround = PrimitiveCreator::createSphere(30, 30, 100);
    auto primCenter = PrimitiveCreator::createSphere(30, 30, 20);
    auto primLeft = PrimitiveCreator::createSphere(30, 30, 20);
    auto primBubble = PrimitiveCreator::createSphere(30, 30, 18);
    auto primRight = PrimitiveCreator::createSphere(30, 30, 20);

    shared_ptr<Sphere> ground = make_shared<Sphere>(primGround, 100.0, material_ground);
    shared_ptr<Sphere> center = make_shared<Sphere>(primCenter, 20, material_center);
    shared_ptr<Sphere> left = make_shared<Sphere>(primLeft, 20, material_left);
    shared_ptr<Sphere> bubble = make_shared<Sphere>(primBubble, 18, material_bubble);
    shared_ptr<Sphere> right = make_shared<Sphere>(primRight, 20, material_right);
    ground->setGlobalPosition(Vec3(0.0, -100.5, -1.0));
    center->setGlobalPosition(Vec3(0.0, 0.0, -1.2));
    left->setGlobalPosition(Vec3(-30, 0.0, -1.0));
    bubble->setGlobalPosition(Vec3(-30, 0.0, -1.0));
    right->setGlobalPosition(Vec3(30, 0.0, -1.0));

    sceneGraph.AddNode(make_shared<Node>("Ground", ground));
    sceneGraph.AddNode(make_shared<Node>("Center", center));
    sceneGraph.AddNode(make_shared<Node>("Left", left));
    sceneGraph.AddNode(make_shared<Node>("Bubble", bubble));
    sceneGraph.AddNode(make_shared<Node>("Right", right));


    font.load("fonts/JetBrainsMono-Regular.ttf", 12, true, true);



}


void Scene3D::draw()
{
    ofClear(clearColor);
    ofSetColor(0, 255, 0);
    this->DrawSceneGraphWindow();
    this->DrawSelectedNodeWindow();
    this->DrawCommandHistoryWindow();

    for (auto& camera: cameras)
    {
        camera->begin(camera->getViewPort());
        for (auto& camera: cameras)
        {
            

            if (camera->frustrumVisible())
            {
                camera->drawFrustum();
            }
        }
        drawScene();
        camera->end();
         
        if (camera->isRayTracing()) {
            //ofSetColor(clearColor);
            ofSetColor(ofColor(255,255,255));
            ViewPort viewPort = camera->getViewPort();
            int new_width = viewPort.getWidth() * camera->getScreenCrop();
            int new_height = viewPort.getHeight() * camera->getScreenCrop();
            ofImage image = camera->getRayImage();
            ofDisableDepthTest();
            image.draw(viewPort.getX() + viewPort.getWidth() - new_width, viewPort.getY(), new_width, new_height);
            float percent = camera->portionDone() * 100;
            std::string perc = std::format("%: {:.4f}", percent);
            ofPushStyle();
            ofSetColor(0, 255, 0);
            ofDisableDepthTest();

            ofPushStyle();
            ofRectangle rectangle(viewPort.getX() + viewPort.getWidth() - static_cast<float>(perc.size() - 1) * 12.0f, viewPort.getY() + new_height - 30.0f,
                                  static_cast<float>(perc.size() - 1) * 12.0f, 30.0f);
            ofFill();
            ofSetColor(ofColor(0, 0, 0));
            ofDrawRectangle(rectangle);
            ofPopStyle();

            font.drawString(perc, static_cast<float>(viewPort.getX() + viewPort.getWidth() - static_cast<float>(perc.size() - 1) * 12.0f), viewPort.getY() + new_height - 12.0f);
            ofEnableDepthTest();
            
           

            ofPopStyle();

            ofEnableDepthTest();
            ofSetColor(0, 255, 0);
        }
        
        ofNoFill();
        ofDrawRectangle(camera->getViewPort());
    }

    ofSetColor(255, 255, 255);

    this->ExecuteQueuedCommands();
}

void Scene3D::drawScene()
{
    material.begin();
    sceneGraph.Draw();

    if (is_selected)
    {
        selectionMesh.enableColors();
        selectionMesh.setColorForIndices(255, 255, 0);

        selectionMesh.draw();
    }

    material.end();
}

void Scene3D::DrawSceneGraphWindow()
{
    ImGui::SetNextWindowPos(ImVec2(10, 30), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(300, 400), ImGuiCond_FirstUseEver);
    ImGui::Begin("3D scene graph");
    this->ShowChildren(this->sceneGraph.GetRoot());
    ImGui::End();
}

void Scene3D::DrawSelectedNodeWindow()
{
    if ((*this->selectedNode) != nullptr)
    {
        ImGui::SetNextWindowPos(ImVec2(320, 30), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(300, 400), ImGuiCond_FirstUseEver);

        if (ImGui::Begin("Selected Node"))
        {
            ImGui::Text("Selected node: %s", (*this->selectedNode)->GetName().c_str());
            if (ImGui::Button("Delete node"))
            {
                if (auto camera = std::dynamic_pointer_cast<Camera>(this->selectedNode->get()->GetInner()); camera)
                {
                    NodeId id = this->selectedNode->get()->GetId();

                    cameraMap.erase(id);
                    updateViewPorts();
                }
                this->history.executeCommand(std::make_shared<RemoveNodeCommand>(*this->selectedNode));
                this->SelectNode(this->sceneGraph.GetRoot());
            }

            this->DrawModifyNodeSliders(*this->selectedNode);


            if (auto light = std::dynamic_pointer_cast<ofLight>(this->selectedNode->get()->GetInner()); light)
            {
                this->DrawModifyLightSliders(light);
            }

            if (auto camera = std::dynamic_pointer_cast<Camera>(this->selectedNode->get()->GetInner()); camera)
            {
                NodeId id = this->selectedNode->get()->GetId();
                if (!cameraMap.contains(id))
                {
                    cameraMap.emplace(id, camera);
                }
                bool& activated = cameraMap.at(id).lock()->isActivated();
                if (ImGui::Checkbox("Activate", &activated))
                {
                    updateViewPorts();
                }

                bool& frustumActivated = cameraMap.at(id).lock()->frustrumVisible();
                if (ImGui::Checkbox("Visible Frustum", &frustumActivated))
                {
                    updateViewPorts();
                }

                bool& rayTrace = cameraMap.at(id).lock()->isRayTracing();
                if (ImGui::Checkbox("Show Ray Trace", &rayTrace))
                {
                    updateViewPorts();
                }

                bool tempOrtho = camera->getOrtho();
                if (ImGui::Checkbox("Orthogonal Projection", &tempOrtho))
                {
                    if (tempOrtho)
                    {
                        camera->enableOrtho();
                    }
                    else
                    {
                        camera->disableOrtho();
                    }
                    updateViewPorts();
                }

                this->DrawModifyCameraNodeSliders(*this->selectedNode, camera);
            }

            if (const std::shared_ptr<Primitive3D> prim = std::dynamic_pointer_cast<Primitive3D>(this->selectedNode->get()->GetInner()); prim) { ImGui::Checkbox("Wireframe", prim->getWireframe()); }

            if (ImGui::CollapsingHeader("Add child", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::ColorEdit4("Color", sharedParams->color);
                ImGui::Checkbox("Glass", &sharedParams->isGlass);

                for (auto& createShapeUI: this->createShapeUIs)
                {
                    createShapeUI->Draw();
                }
            }
        }
        ImGui::End();
    }
}

void Scene3D::DrawModifyLightSliders(const std::shared_ptr<ofLight>& light)
{
    bool tempIsEnabled = light->getIsEnabled();
    if (ImGui::Checkbox("Activate", &tempIsEnabled))
    {
        if (tempIsEnabled)
        {
            light->enable();
        }
        else
        {
            light->disable();
        }
    }

    bool isDirectional = light->getIsDirectional();
    bool isSpotlight = light->getIsSpotlight();
    bool isPointLight = light->getIsPointLight();
    if (ImGui::Checkbox("Directional", &isDirectional))
    {
        light->setDirectional();
    }
    ImGui::SameLine();
    if (ImGui::Checkbox("Spotlight", &isSpotlight))
    {
        light->setSpotlight();
    }
    ImGui::SameLine();
    if (ImGui::Checkbox("Point light", &isPointLight))
    {
        light->setPointLight();
    }

    ofFloatColor diffuseColor = light->getDiffuseColor();
    float diffuseColorArr[3] = {diffuseColor.r, diffuseColor.g, diffuseColor.b};
    if (ImGui::ColorEdit3("Diffuse color", diffuseColorArr))
    {
        light->setDiffuseColor(ofFloatColor(diffuseColorArr[0], diffuseColorArr[1], diffuseColorArr[2]));
    }

    ofFloatColor specularColor = light->getSpecularColor();
    float specularColorArr[3] = {specularColor.r, specularColor.g, specularColor.b};
    if (ImGui::ColorEdit3("Specular color", specularColorArr))
    {
        light->setSpecularColor(ofFloatColor(specularColorArr[0], specularColorArr[1], specularColorArr[2]));
    }

    isDirectional = light->getIsDirectional();
    isSpotlight = light->getIsSpotlight();
    isPointLight = light->getIsPointLight();

    if (isSpotlight)
    {
        float spotCutOff = light->getSpotlightCutOff();
        if (ImGui::SliderFloat("Spotlight cut off", &spotCutOff, 0, 90))
        {
            light->setSpotlightCutOff(spotCutOff);
        }

        float spotConcentration = light->getSpotConcentration();
        if (ImGui::SliderFloat("Spotlight concentration", &spotConcentration, 0, 128))
        {
            light->setSpotConcentration(spotConcentration);
        }
    }
    else if (isPointLight)
    {
        float attenuationConstant = light->getAttenuationConstant();
        if (ImGui::SliderFloat("Attenuation constant", &attenuationConstant, 0, 1))
        {
            light->setAttenuation(attenuationConstant, light->getAttenuationLinear(), light->getAttenuationQuadratic());
        }

        float attenuationLinear = light->getAttenuationLinear();
        if (ImGui::SliderFloat("Attenuation linear", &attenuationLinear, 0, 0.01, "%.5f"))
        {
            light->setAttenuation(attenuationConstant, attenuationLinear, light->getAttenuationQuadratic());
        }

        float attenuationQuadratic = light->getAttenuationQuadratic();
        if (ImGui::SliderFloat("Attenuation quadratic", &attenuationQuadratic, 0, 0.001, "%.6f"))
        {
            light->setAttenuation(attenuationConstant, attenuationLinear, attenuationQuadratic);
        }
    }
}

void Scene3D::DrawModifyCameraNodeSliders(const std::shared_ptr<Node>& node, shared_ptr<Camera> camera)
{
    const std::shared_ptr<ofNode>& inner = node->GetInner();

    // Fov
    const float current_fov = camera->getFov();
    if (ImGui::SliderFloat("FOV", &this->fov, 0, 180))
    {
        camera->setFov(fov);
    }
    if (ImGui::IsItemActivated())
    {
        this->initialFov = current_fov;
    }
    if (camera->isRayTracing()) {
        ImGui::SliderInt("Samples", &camera->getSamples(), 1, 200);
        ImGui::SliderInt("Depth", &camera->getDepth(), 1, 50);
        ImGui::SliderInt("Resolution", &camera->getWidth(), 10, 4000);
        ImGui::SliderFloat("Portion of Screen", &camera->getScreenCrop(), 0.05, 1);
        if (ImGui::Button("Reset Render")) {
            camera->reset();
        }
    }
    if (ImGui::IsItemDeactivatedAfterEdit()) { this->history.executeCommand(std::make_shared<SetCameraFovCommand>(camera, this->fov, this->initialFov)); }
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
    if (ImGui::SliderFloat3("Scale", this->scale, 0.001f, 10.0f))
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

    ImGui::BeginDisabled(undoStack.empty());
    if (ImGui::Button("Undo"))
    {
        this->history.undo();
        if (!this->sceneGraph.IsNodeCurrentlyInGraph(this->selectedNode->get()->GetId()))
        {
            this->SelectNode(this->sceneGraph.GetRoot());
        }
        else
        {
            this->ResetParams(*this->selectedNode);
        }
    }
    ImGui::EndDisabled();

    ImGui::SameLine();
    ImGui::BeginDisabled(redoStack.empty());
    if (ImGui::Button("Redo"))
    {
        this->history.redo();
        if (!this->sceneGraph.IsNodeCurrentlyInGraph(this->selectedNode->get()->GetId()))
        {
            this->SelectNode(this->sceneGraph.GetRoot());
        }
        else
        {
            this->ResetParams(*this->selectedNode);
        }
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

// NOLINTNEXTLINE(misc-no-recursion)
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

    const bool nodeOpen = ImGui::TreeNodeEx(node->GetName().c_str(), flags);


    if (ImGui::IsItemClicked())
    {
        this->SelectNode(node);
    }

    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DRAG_NODE"))
        {
            IM_ASSERT(payload->DataSize == sizeof(std::shared_ptr<Node>));

            if (std::shared_ptr<Node> payload_node = *static_cast<std::shared_ptr<Node>*>(payload->Data); !payload_node->GetId() == 0) { commandQueue.push(std::make_shared<MoveChildCommand>(payload_node, node)); }
            else { ofLogError() << "Cannot move the root node"; }
        }
        ImGui::EndDragDropTarget();
    }

    if (ImGui::BeginDragDropSource())
    {
        ImGui::SetDragDropPayload("DRAG_NODE", &node, sizeof(node));
        ImGui::EndDragDropSource();
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


    if (auto camera = std::dynamic_pointer_cast<Camera>(node->GetInner()); camera)
    {
        const float currentFov = camera->getFov();
        fov = currentFov;
    }
}


void Scene3D::reset()
{
    camera->resetTransform();
}

void Scene3D::mouseReleased(int x, int y, int button)
{
    auto difference = ofVec3f(static_cast<float>(x - pressed_x), static_cast<float>(y - pressed_y), 0);

    if (button == 0)
    {
        // translate
        if (is_selected)
        {

            ofVec3f current_pos = selectedNode->get()->GetInner()->getPosition();
            glm::vec3 initial = glm::vec3(initialSelectedPosition.x, initialSelectedPosition.y, initialSelectedPosition.z);
            shared_ptr<Node> node = *(this->selectedNode);
            this->history.executeCommand(std::make_shared<SetPositionCommand>(node, glm::vec3(current_pos[0], current_pos[1], current_pos[2]), initial));
        }
    }
    else if (button == 2)
    {
        // scale
        if (is_selected)
        {

            ofVec3f current_scale = selectedNode->get()->GetInner()->getScale();
            glm::vec3 initial = glm::vec3(initialSelectedScale.x, initialSelectedScale.y, initialSelectedScale.z);
            shared_ptr<Node> node = *(this->selectedNode);
            this->history.executeCommand(std::make_shared<SetScaleCommand>(node, glm::vec3(current_scale[0], current_scale[1], current_scale[2]), initial));
        }
    }
}

void Scene3D::mouseDragged(int x, int y, int button)
{
    switch (button)
    {
        case 0://left
            if (is_selected)
            {
                ofVec3f current_pos = selectedNode->get()->GetInner()->getGlobalPosition();

                ofVec3f current_view_pos = worldToViewPort(current_pos);

                ofVec3f current = ofVec3f(x, y, current_view_pos.z);

                ofVec3f new_pos = viewPortToWorld(current);

                selectedNode->get()->GetInner()->setGlobalPosition(new_pos);
            }

            break;
        case 2://right
            if (is_selected)
            {
                ofVec3f current_pos = selectedNode->get()->GetInner()->getPosition();
                ofVec3f current_screen_pos = camera->worldToScreen(current_pos);

                ofVec3f current = ofVec3f(x, y, current_screen_pos.z);
                ofVec3f past = ofVec3f(previous_x, previous_y, current_screen_pos.z);

                ofVec3f new_pos = camera->screenToWorld(current_screen_pos + (current - past));

                float scale = 1 + ((x - previous_x) + (previous_y - y)) * 0.1;
                ofVec3f scaleVec = selectedNode->get()->GetInner()->getScale() * scale;
                scaleVec.x = std::max(0.01f, scaleVec.x);
                scaleVec.y = std::max(0.01f, scaleVec.y);
                scaleVec.z = std::max(0.01f, scaleVec.z);
                selectedNode->get()->GetInner()->setScale(scaleVec);
            }
    }

    previous_x = x;
    previous_y = y;
}

void Scene3D::windowResized(int w, int h) { updateViewPorts(); }


ofVec3f Scene3D::worldToViewPort(ofVec3f worldPos) const
{
    camera->begin(current_viewPort);
    ofVec3f screenPos = camera->worldToScreen(worldPos);
    camera->end();
    if (current_viewPort.y > 0)
    {
        screenPos.y += (static_cast<float>(ofGetHeight()) - current_viewPort.getHeight());
    }
    else
    {
        screenPos.y -= (static_cast<float>(ofGetHeight()) - current_viewPort.getHeight());
    }

    return screenPos;
}

ofVec3f Scene3D::screenToViewPort(ofVec3f screenPos) const
{
    ofVec3f viewPos;
    viewPos.x = screenPos.x * (current_viewPort.getHeight() / static_cast<float>(ofGetHeight())) + current_viewPort.x;
    viewPos.y = screenPos.y * (current_viewPort.getHeight() / static_cast<float>(ofGetHeight())) + current_viewPort.y;
    viewPos.z = (float) screenPos.z;

    return viewPos;
}

ofVec3f Scene3D::viewPortToWorld(ofVec3f viewPos) const
{
    if (!(current_viewPort.y > 0))
    {
        viewPos.y += (ofGetHeight() - current_viewPort.getHeight());
    }
    else
    {
        viewPos.y -= (ofGetHeight() - current_viewPort.getHeight());
    }
    camera->begin(current_viewPort);
    const ofVec3f worldPos = camera->screenToWorld(viewPos);
    camera->end();
    return worldPos;
}

ofVec3f Scene3D::viewPortToScreen(ofVec3f viewPos) const
{
    ofVec3f screenPos;
    screenPos.x = (viewPos.x - current_viewPort.x) / (current_viewPort.getHeight() / static_cast<float>(ofGetHeight()));
    screenPos.y = (viewPos.y - current_viewPort.y) / (current_viewPort.getHeight() / static_cast<float>(ofGetHeight()));
    screenPos.z = viewPos.z;
    return viewPos;
}

void Scene3D::mousePressed(int x, int y, int button)
{
    is_selected = false;
    pressed_x = x;
    pressed_y = y;
    previous_x = x;
    previous_y = y;
    if (ImGui::GetIO().WantCaptureMouse)
    {
        return;
    }

    updateViewPorts();

    float closeness = -1;
    for (const std::vector<std::pair<std::shared_ptr<SceneObject>, NodeId>> primitive_pairs = getSceneObjects(); auto& primitive_pair: primitive_pairs)
    {
        std::shared_ptr<SceneObject> primitive = primitive_pair.first;
        NodeId id = primitive_pair.second;

        std::pair<ofVec3f, ofVec3f> vertices = primitive->getBoundingVertices();
        ofVec3f minVertex = vertices.first, maxVertex = vertices.second;


        std::vector<ofVec3f> corners = {
                worldToViewPort(maxVertex),
                worldToViewPort(ofVec3f(minVertex.x, maxVertex.y, maxVertex.z)),
                worldToViewPort(ofVec3f(minVertex.x, minVertex.y, maxVertex.z)),
                worldToViewPort(ofVec3f(maxVertex.x, minVertex.y, maxVertex.z)),

                worldToViewPort(minVertex),
                worldToViewPort(ofVec3f(minVertex.x, maxVertex.y, minVertex.z)),
                worldToViewPort(ofVec3f(minVertex.x, minVertex.y, minVertex.z)),
                worldToViewPort(ofVec3f(maxVertex.x, minVertex.y, minVertex.z)),
        };

        ofVec3f camPos = camera->getPosition();
        ofVec3f objPos = primitive->getPosition();
        float new_closeness = camPos.distance(objPos);

        float minX = 0, maxX = 0;
        float minY = 0, maxY = 0;


        minX = corners[0].x, maxX = corners[0].x;
        minY = corners[0].y, maxY = corners[0].y;

        for (auto& corner: corners)
        {

            minX = std::min(minX, corner.x);
            maxX = std::max(maxX, corner.x);
            minY = std::min(minY, corner.y);
            maxY = std::max(maxY, corner.y);
        }

        onScreenCorners = ofRectangle(minX, minY, maxX - minX, maxY - minY);

        if (minX < x && x < maxX && minY < y && y < maxY)
        {
            if (new_closeness < closeness || closeness == -1)
            {
                closeness = new_closeness;
                selectionMesh = primitive->getSelectionBox();
                is_selected = true;
                initialSelectedPosition = primitive->getGlobalPosition();
                initialSelectedScale = primitive->getScale();
                SelectNode(sceneGraph.GetNode(id).value());
            }
        }
    }
}

void Scene3D::dragEvent(ofDragInfo dragInfo)
{
    if (!dragInfo.files.empty())
    {
        auto model = std::make_shared<ImportModel>();
        std::string filePath = dragInfo.files[0];// Get the first dropped file
        model->loadModel(filePath);
        if (model->loaded())
        {
            model->setPosition(0, 0, 0);
            shared_ptr<Node> node = make_shared<Node>("Object ", model);
            shared_ptr<Node> parent = *selectedNode;
            history.executeCommand(std::make_shared<AddChildToNodeCommand>(parent, node));
        }
        else
        {
            ofLog() << "Cannot Import.";
        }
    }
}

std::vector<ofVec3f> Scene3D::getPrimitiveVertices(of3dPrimitive& primitive)
{
    ofMesh mesh = primitive.getMesh();
    ofMatrix4x4 transform = primitive.getLocalTransformMatrix();

    std::vector<ofVec3f> transformedVertices;
    for (auto& vertex: mesh.getVertices())
    {
        ofVec4f transformedVertex = transform.preMult(ofVec4f(vertex.x, vertex.y, vertex.z, 1.0f));
        transformedVertices.push_back(ofVec3f(transformedVertex.x, transformedVertex.y, transformedVertex.z));
    }
    return transformedVertices;
}


void Scene3D::focus()
{
    camera->lookAt(selectedNode->get()->GetInner()->getGlobalPosition());
}

int Scene3D::charToLower(int key)
{
    if (key >= 65 && key <= 90) { return key + 32; }
    return key;
}

void Scene3D::keyPressed(int key)
{
    key = charToLower(key);

    switch (key)
    {
        case 119://w
            if (!is_key_press_w)
            {
                is_key_press_w = true;
                storeCameraTranslation();
            }
            break;
        case 100://d
            if (!is_key_press_d)
            {
                is_key_press_d = true;
                storeCameraTranslation();
            }
            break;
        case 115://s
            if (!is_key_press_s)
            {
                is_key_press_s = true;
                storeCameraTranslation();
            }
            break;
        case 97://a
            if (!is_key_press_a)
            {
                is_key_press_a = true;
                storeCameraTranslation();
            }

            break;
        case 57357://up
            if (!is_key_press_up)
            {
                is_key_press_up = true;
                storeCameraRotation();
            }

            break;
        case 57358://right
            if (!is_key_press_right)
            {
                is_key_press_right = true;
                storeCameraRotation();
            }

            break;
        case 57359://down
            if (!is_key_press_down)
            {
                is_key_press_down = true;
                storeCameraRotation();
            }


            break;
        case 57356://left
            if (!is_key_press_left)
            {
                is_key_press_left = true;
                storeCameraRotation();
            }
            break;
        case 113://q
            if (!is_key_press_q)
            {
                is_key_press_q = true;
                storeCameraTranslation();
            }
            break;
        case 101://e
            if (!is_key_press_e)
            {
                is_key_press_e = true;
                storeCameraTranslation();
            }

            break;
        case 102://f
            is_key_press_f = true;
            break;
        case 103://g
            is_key_press_g = true;
            break;
        case 45://-
            if (!is_key_press_minus)
            {
                is_key_press_minus = true;
                storeCameraRotation();
            }

            break;
        case 61:// =
            if (!is_key_press_plus)
            {
                is_key_press_plus = true;
                storeCameraRotation();
            }
            break;
        case 'p':
            toggleOrtho();
    }
}

void Scene3D::keyPressed(ofKeyEventArgs& key)
{
    if (key.hasModifier(OF_KEY_CONTROL))
    {
        switch (charToLower(key.key))
        {
            case 26://z
                this->history.undo();
                break;
            case 25://y
                this->history.redo();
                break;
        }
    }
    else { this->keyPressed(key.key); }
}

void Scene3D::keyReleased(int key)
{
    key = charToLower(key);

    switch (key)
    {
        case 119://w
            is_key_press_w = false;
            applyCameraTranslation();
            break;
        case 100://d
            is_key_press_d = false;
            applyCameraTranslation();
            break;
        case 115://s
            is_key_press_s = false;
            applyCameraTranslation();
            break;
        case 97://a
            is_key_press_a = false;
            applyCameraTranslation();
            break;
        case 57357://up
            is_key_press_up = false;
            applyCameraRotation();
            break;
        case 57358://right
            is_key_press_right = false;
            applyCameraRotation();
            break;
        case 57359://down
            is_key_press_down = false;
            applyCameraRotation();
            break;
        case 57356://left
            is_key_press_left = false;
            applyCameraRotation();
            break;
        case 113://q
            is_key_press_q = false;
            applyCameraTranslation();
            break;
        case 101://e
            is_key_press_e = false;
            applyCameraTranslation();
            break;
        case 102://f
            is_key_press_f = false;
            break;
        case 103:
            is_key_press_g = false;
            break;
        case 114://r
            storeCameraTranslation();
            storeCameraRotation();
            reset();
            applyCameraTranslation();
            applyCameraRotation();
            break;
        case 45://-
            is_key_press_minus = false;
            applyCameraRotation();
            break;
        case 61:// =
            is_key_press_plus = false;
            applyCameraRotation();
            break;
        case 107://k
            debugger = !debugger;
            break;
        case 'y':
            storeCameraRotation();
            focus();
            applyCameraRotation();
            break;
    }
}

void Scene3D::toggleOrtho()
{

    if (!camera->getOrtho())
    {
        ofRectangle viewPort = current_viewPort;
        //camera->setOrtho(viewPort.getLeft(), viewPort.getRight(), viewPort.getTop(), viewPort.getBottom(), -1000, 1000);
        camera->enableOrtho();
    }
    else
    {
        camera->disableOrtho();
    }
}

void Scene3D::update()
{
    time_current = ofGetElapsedTimef();
    time_elapsed = time_current - time_last;
    time_left = std::fmax(0, 1.0f / 60 - time_elapsed);
    time_elapsed_timer = time_current - time_last_timer;
    time_last = time_current;

    if (time_elapsed_timer > 5)
    {
        ofLog() << "Target Fps" << ofGetTargetFrameRate() << endl;

        //ofLog() << "Exporting ray trace";
        time_last_timer = time_current;
        //exportRayTrace(time_left);
        ofLog() << "Time elapsed " << time_elapsed << " Time Left : " << time_left << endl;

        for (auto& camera : cameras) {
            cameraRayCount = 0;
            if (camera->isRayTracing()) {
                cameraRayCount++;
            }
        }
     
    }

    exportRayTrace(time_left);

    speed_translation = translate_speed * time_elapsed;
    speed_rotation = rotate_speed * time_elapsed;

    // d?placement sur le plan XZ en fonction de l'?tat des fl?ches du clavier
    if (is_key_press_w)
    {
        camera->boom(speed_translation);
    }
    if (is_key_press_s)
    {
        camera->boom(-speed_translation);
    }
    if (is_key_press_a)
    {
        camera->truck(-speed_translation);
    }
    if (is_key_press_d)
    {
        camera->truck(speed_translation);
    }
    if (is_key_press_up)
    {
        camera->tiltDeg(speed_rotation);
    }
    if (is_key_press_down)
    {
        camera->tiltDeg(-speed_rotation);
    }
    if (is_key_press_left)
    {
        camera->panDeg(speed_rotation);
    }
    if (is_key_press_right)
    {
        camera->panDeg(-speed_rotation);
    }
    if (is_key_press_q)
    {
        camera->dolly(speed_translation);
    }
    if (is_key_press_e)
    {
        camera->dolly(-speed_translation);
    }
    if (is_key_press_f)
    {
        camera->setFov(camera->getFov() + 1);
    }
    if (is_key_press_g)
    {
        camera->setFov(camera->getFov() - 1);
    }
    if (is_key_press_plus)
    {
        camera->rollDeg(speed_rotation);
    }
    if (is_key_press_minus)
    {
        camera->rollDeg(-speed_rotation);
    }
}

std::vector<std::pair<std::shared_ptr<SceneObject>, NodeId>> Scene3D::getSceneObjects() const
{
    const std::vector<std::shared_ptr<Node>> nodes = sceneGraph.GetNodes();
    std::vector<std::pair<std::shared_ptr<SceneObject>, NodeId>> sceneObjects{};

    for (const auto& node: nodes)
    {
        if (auto inner = node->GetInner())
        {
            if (auto prim = std::dynamic_pointer_cast<SceneObject>(inner))
            {
                sceneObjects.emplace_back(prim, node->GetId());
            }
        }
    }
    return sceneObjects;
}

void Scene3D::storeCameraRotation()
{
    if (const int count = getCameraRotationCommands(); count == 1)
    {
        initialCameraRotation = camera->getOrientationQuat();
    }
}

void Scene3D::applyCameraRotation()
{
    if (const int count = getCameraRotationCommands(); count == 0)
    {
        glm::quat camR = camera->getOrientationQuat();

        if (const std::optional<std::shared_ptr<Node>> optionalNode = sceneGraph.GetNode(current_camera_id); optionalNode.has_value())
        {
            shared_ptr<Node> node = optionalNode.value();
            this->history.executeCommand(std::make_shared<SetRotationCommand>(node, camR, initialCameraRotation));
        }
    }
}

void Scene3D::storeCameraTranslation()
{
    if (const int count = getCameraTranslationCommands(); count == 1)
    {
        initialCameraPosition = camera->getPosition();
    }
}

void Scene3D::applyCameraTranslation()
{
    if (const int count = getCameraTranslationCommands(); count == 0)
    {
        ofVec3f current_pos = camera->getPosition();
        auto initial = glm::vec3(initialCameraPosition.x, initialCameraPosition.y, initialCameraPosition.z);
        if (const std::optional<std::shared_ptr<Node>> optionalNode = sceneGraph.GetNode(current_camera_id); optionalNode.has_value())
        {
            shared_ptr<Node> node = optionalNode.value();
            this->history.executeCommand(std::make_shared<SetPositionCommand>(node, glm::vec3(current_pos[0], current_pos[1], current_pos[2]), initial));
        }
    }
}

int Scene3D::getCameraRotationCommands() const
{
    vector<bool> vec = {is_key_press_up,
                        is_key_press_left,
                        is_key_press_down,
                        is_key_press_right,
                        is_key_press_minus,
                        is_key_press_plus};
    return std::ranges::count(vec, true);
}

int Scene3D::getCameraTranslationCommands() const
{
    vector<bool> vec = {is_key_press_w,
                        is_key_press_a,
                        is_key_press_s,
                        is_key_press_d,
                        is_key_press_q,
                        is_key_press_e};
    return std::ranges::count(vec, true);
}

void Scene3D::divideCamera(int first, int last, int x1, int y1, int width, int height, vector<pair<NodeId, shared_ptr<Camera>>> activatedCameras)
{
    int halfWidth = width / 2;
    int halfHeight = height / 2;
    if (first == last)
    {
        ofRectangle newViewport = ofRectangle(x1, y1, width, height);
        activatedCameras[first].second->setViewPort(newViewport);
        cameras.emplace_back(activatedCameras[first].second);
        if (x1 <= previous_x && previous_x <= x1 + width && y1 <= previous_y && previous_y <= y1 + height)
        {
            current_camera_id = activatedCameras[first].first;
            current_viewPort = newViewport;
            camera = activatedCameras[first].second;
        }
    }
    else if (last - first == 1)
    {
        divideCamera(first, first, x1, y1, width, halfHeight, activatedCameras);
        divideCamera(last, last, x1, y1 + halfHeight, width, halfHeight, activatedCameras);
    }
    else if (last - first == 2)
    {

        divideCamera(first, first, x1, y1, halfWidth, halfHeight, activatedCameras);
        divideCamera(first + 1, first + 1, halfWidth + x1, y1, halfWidth, halfHeight, activatedCameras);
        divideCamera(last, last, x1, halfHeight + y1, width, halfHeight, activatedCameras);

    }
    else {
        //devide into 4 groups
        int mid = floor((first + last) / 2);
        int topmid = floor((first + mid) / 2);
        int bottommid = floor(((mid + 1) + last) / 2);

        divideCamera(first, topmid, x1, y1, halfWidth, halfHeight, activatedCameras);
        divideCamera(topmid + 1, mid, halfWidth + x1, y1, halfWidth, halfHeight, activatedCameras);
        divideCamera(mid + 1, bottommid, x1, halfHeight + y1, halfWidth, halfHeight, activatedCameras);
        divideCamera(bottommid + 1, last, halfWidth + x1, y1 + halfHeight, halfWidth, halfHeight, activatedCameras);
            

    }
}

void Scene3D::updateViewPorts()
{
    cameras.clear();
    vector<pair<NodeId, shared_ptr<Camera>>> activatedCameras;
    for (auto& [id, cam]: cameraMap)
    {
        if (cam.lock()->isActivated())
        {
            if (auto ptr = cam.lock(); ptr)
            {
                activatedCameras.emplace_back(id, ptr);
            }
            else
            {
                cameraMap.erase(id);
            }
        }
    }

    int camNumber = activatedCameras.size();
    int prev_height = 0;
    int prev_width = 0;
    divideCamera(0, camNumber - 1, 0, 0, ofGetWidth(), ofGetHeight(), activatedCameras);
}


//inline double random_double()
//{
//    static std::uniform_real_distribution<double> distribution(0.0, 1.0);
//    static std::mt19937 generator;
//    return distribution(generator);
//}

void Scene3D::exportRayTrace(float time_left)
{
    if (cameraRayCount == 0) {
        return;
    }
    
    for (auto& camera : cameras) {
        if (!camera->isRayTracing()) {
            continue;
        }
        float divided_time = time_left / (float) cameraRayCount;
        float now = ofGetElapsedTimef();
        float time_last = now;
        float time_elapsed = now - divided_time;
        camera->renderPixel(sceneGraph);
        while (divided_time > 0 && !camera->doneRendering())
        {
            camera->renderPixel(sceneGraph);
            now = ofGetElapsedTimef();
            time_elapsed = now - time_last;
            time_last = now;
            divided_time -= time_elapsed;
        }
    }

}


ofColor Scene3D::rayColor(const Ray& r) {
    hit_record rec;

    if (hitAnything(r, Interval(0, INFINITY), rec)) {
        ofColor normal_color = ofColor(
                (rec.normal.x + 1) * 127.5f,
                (rec.normal.y + 1) * 127.5f,
                (rec.normal.z + 1) * 127.5f);
        return normal_color;
    }

    auto unit_direction = r.getDirection().getNormalized();
    auto a = 0.5 * (unit_direction.y + 1.0);
    return (1.0 - a) * ofColor(255, 255, 255) + a * ofColor(127, 200, 255);


}


double Scene3D::hitAnything(const Ray& r, Interval ray_t, hit_record& rec) {
    hit_record temp_rec;
    double closest_so_far = ray_t.max;
    bool hit_anything = false;

    for (const auto& node: sceneGraph.GetNodes())
    {

        if (auto object = std::dynamic_pointer_cast<Primitive3D>(node->GetInner()))
        {
            if (object->hit(r, Interval(ray_t.min, closest_so_far), temp_rec))
            {
                hit_anything = true;
                closest_so_far = temp_rec.t;
                rec = temp_rec;
            }
        }

    }
    if (hit_anything)
    {
        hitAnyPixel = true;
    }
    return hit_anything;
}
