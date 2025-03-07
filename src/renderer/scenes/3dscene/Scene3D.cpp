#include "Scene3D.h"

#include "3dscene/commands/RemoveNodeCommand.h"
#include "3dscene/commands/SetPositionCommand.h"
#include "3dscene/commands/SetRotationCommand.h"
#include "3dscene/commands/SetScaleCommand.h"
#include "3dscene/createShapes/CreateCubeUI.h"
#include "3dscene/createShapes/CreateLightUI.h"
#include "3dscene/createShapes/CreateSphereUI.h"
#include "imgui.h"
#include "ofAppRunner.h"
#include "ofGraphics.h"
#include <cmath>
#include <numbers>
#include "renderer/ImportModel.h"
#include "SceneObject.h"
#include "Light.h"
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
    this->sceneGraph.AddNode(light_ptr);

    // Add base camera
    //cam

    camera = std::make_shared<ofCamera>();
    camera->setPosition(0, 0, -200);
    camera->lookAt(ofVec3f(0, 0, 0));
    auto cam_ptr = std::make_shared<Node>("Camera", camera);
    this->sceneGraph.AddNode(cam_ptr);
    cameraMap.emplace(cam_ptr.get()->GetId(), std::pair(camera, true));
    updateViewPorts();


    shared_ptr<ofCamera> camera2 = std::make_shared<ofCamera>();
    camera2->setPosition(0, 0, -200);
    camera2->lookAt(ofVec3f(0, 0, 0));
    auto cam_ptr2 = std::make_shared<Node>("Camera", camera2);
    this->sceneGraph.AddNode(cam_ptr2);
    cameraMap.emplace(cam_ptr2.get()->GetId(), std::pair(camera2, true));
    updateViewPorts();


    shared_ptr<ofCamera> camera3 = std::make_shared<ofCamera>();
    camera3->setPosition(0, 0, -200);
    camera3->lookAt(ofVec3f(0, 0, 0));
    auto cam_ptr3 = std::make_shared<Node>("Camera", camera3);
    this->sceneGraph.AddNode(cam_ptr3);
    cameraMap.emplace(cam_ptr3.get()->GetId(), std::pair(camera3, false));
    updateViewPorts();


    shared_ptr<ofCamera> camera4 = std::make_shared<ofCamera>();
    camera4->setPosition(0, 0, -200);
    camera4->lookAt(ofVec3f(0, 0, 0));
    auto cam_ptr4 = std::make_shared<Node>("Camera", camera4);
    this->sceneGraph.AddNode(cam_ptr4);
    cameraMap.emplace(cam_ptr4.get()->GetId(), std::pair(camera4, false));
    updateViewPorts();

    translate_speed = 1000;
    rotate_speed = 100;
}

void Scene3D::updateViewPorts()
{

    cameras.clear();
    vector<shared_ptr<ofCamera>> activatedCameras;
    for (auto& [id, pair]: cameraMap)
    {
        auto& [camera, toggled] = pair;

        if (toggled)
        {
            auto ptr = camera.lock();
            if (ptr)
            {
                activatedCameras.push_back(ptr);
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
    if (camNumber >= 3)
    {
        cameras.push_back(std::pair(activatedCameras[0], ofRectangle(0, 0, ofGetWidth() / 2, ofGetHeight() / 2)));
        cameras.push_back(std::pair(activatedCameras[1], ofRectangle(ofGetWidth() / 2, 0, ofGetWidth() / 2, ofGetHeight() / 2)));
        if (camNumber == 3)
        {
            cameras.push_back(std::pair(activatedCameras[2], ofRectangle(0, ofGetHeight() / 2, ofGetWidth(), ofGetHeight() / 2)));
            if (previous_x < ofGetWidth() / 2)
            {
                if (previous_y < ofGetHeight() / 2)
                {
                    camera = cameras[0].first;
                    current_viewPort = cameras[0].second;
                }
                else
                {
                    camera = cameras[2].first;
                    current_viewPort = cameras[2].second;
                }
            }

            else
            {
                if (previous_y < ofGetHeight() / 2)
                {
                    camera = cameras[1].first;
                    current_viewPort = cameras[1].second;
                }
                else
                {
                    camera = cameras[2].first;
                    current_viewPort = cameras[2].second;
                }
            }

        }
        else
        {
            cameras.push_back(std::pair(activatedCameras[2], ofRectangle(0, ofGetHeight() / 2, ofGetWidth() / 2, ofGetHeight() / 2)));
            cameras.push_back(std::pair(activatedCameras[3], ofRectangle(ofGetWidth() / 2, ofGetHeight() / 2, ofGetWidth() / 2, ofGetHeight() / 2)));


            if (previous_x < ofGetWidth() / 2)
            {
                if (previous_y < ofGetHeight() / 2)
                {
                    camera = cameras[0].first;
                    current_viewPort = cameras[0].second;
                }
                else
                {
                    camera = cameras[2].first;
                    current_viewPort = cameras[2].second;
                }
            }

            else
            {
                if (previous_y < ofGetHeight() / 2)
                {
                    camera = cameras[1].first;
                    current_viewPort = cameras[1].second;
                }
                else
                {
                    camera = cameras[3].first;
                    current_viewPort = cameras[3].second;
                }
            }
        }


        
    }
    else if (camNumber == 2)
    {
        ofLog() << "Changing Viewport" << current_viewPort.x;
        current_viewPort = ofRectangle(0, 0, ofGetWidth(), ofGetHeight() / 2);
        camera = activatedCameras[0];
        cameras.push_back(std::pair(activatedCameras[0], ofRectangle(0, 0, ofGetWidth(), ofGetHeight() / 2)));
        cameras.push_back(std::pair(activatedCameras[1], ofRectangle(0, ofGetHeight() / 2, ofGetWidth(), ofGetHeight() / 2 )));

        if (previous_y < ofGetHeight() / 2)
        {
            camera = cameras[0].first;
            current_viewPort = cameras[0].second;
        }
        else
        {
            camera = cameras[1].first;
            current_viewPort = cameras[1].second;
        }
    }
    else if (camNumber == 1)
    {
        current_viewPort = ofRectangle(0, 0, ofGetWidth(), ofGetHeight());
        cameras.push_back(std::pair(activatedCameras[0], ofRectangle(0, 0, ofGetWidth(), ofGetHeight())));
        camera = cameras[0].first;
        current_viewPort = cameras[0].second;
    }

}

void Scene3D::draw()
{
    this->DrawSceneGraphWindow();
    this->DrawSelectedNodeWindow();
    this->DrawCommandHistoryWindow();



    for (auto& [camera, box] : cameras) {
        camera->begin(box);
        for (auto& [camera, box]: cameras)
        {
            camera->drawFrustum();
        }
        drawScene();
        camera->end();
        ofNoFill();
        ofDrawRectangle(box);
    }
    
    ofSetColor(0, 255, 0);
    ofDrawRectangle(onScreenCorners);

    ofSetColor(255, 255, 255);

}

void Scene3D::drawScene() {

    ofSetColor(0, 255, 0);
    material.begin();
    sceneGraph.Draw();

    if (is_selected || debugger)
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

        if (ImGui::Begin((*this->selectedNode)->GetName().c_str()))
        {
            if (ImGui::Button("Delete node"))
            {
                this->history.executeCommand(std::make_shared<RemoveNodeCommand>(*this->selectedNode));
                this->SelectNode(this->sceneGraph.GetRoot());
            }

            this->DrawModifyNodeSliders(*this->selectedNode);


            if (auto camera = std::dynamic_pointer_cast<ofCamera>(this->selectedNode.get()->get()->GetInner()); camera)
            {
                NodeId id = this->selectedNode.get()->get()->GetId();
                if (!cameraMap.contains(id)) {
                    cameraMap.emplace(id, std::pair(camera, false));
                }
                bool &activated = cameraMap.at(id).second;
                if (ImGui::Checkbox("Activate", &activated))
                {
                    updateViewPorts();
                }

                //this->DrawModifyCameraNodeSliders(*this->selectedNode, camera);
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


void Scene3D::DrawModifyCameraNodeSliders(const std::shared_ptr<Node>& node, shared_ptr<ofCamera> camera) {
    const std::shared_ptr<ofNode>& inner = node->GetInner();

    // Fov
    const float current_fov = camera->getFov();
    if (ImGui::SliderFloat("FOV", &this->fov, 0, 360))
    {
        camera->setFov(fov);
    }
    if (ImGui::IsItemActivated())
    {
        this->initialFov = current_fov;
    }
 /*   if (ImGui::IsItemDeactivatedAfterEdit())
    {
        this->history.executeCommand(std::make_shared<SetPositionCommand>(node, glm::vec3(this->translate[0], this->translate[1], this->translate[2]), this->initialPosition));
    }*/



    /*if (ImGui::Checkbox("Visible Frustrum", &activated))
    {
        updateViewPorts();
    }*/

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


void Scene3D::reset()
{
    camera->resetTransform();
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
                selectedNode->get()->GetInner()->setScale(scaleVec);
            }
    }

    previous_x = x;
    previous_y = y;
}

ofVec3f Scene3D::worldToViewPort(ofVec3f worldPos)
{
    camera->begin(current_viewPort);
    ofVec3f screenPos = camera->worldToScreen(worldPos);
    camera->end();
    if (current_viewPort.y > 0)
    {
        screenPos.y += (ofGetHeight() - current_viewPort.getHeight());
    }
    else
    {
        screenPos.y -= (ofGetHeight() - current_viewPort.getHeight());
    }
    
    return screenPos;
}

ofVec3f Scene3D::screenToViewPort(ofVec3f screenPos)
{
    ofLog() << "Before screen to viewPort : X : " << screenPos.x << " Y " << screenPos.y << " Z " << screenPos.z;
    ofVec3f viewPos;
    viewPos.x = (float) screenPos.x * ((float) current_viewPort.getHeight() / (float) ofGetHeight()) + current_viewPort.x;
    viewPos.y = (float) screenPos.y * (current_viewPort.getHeight() / (float) ofGetHeight()) + current_viewPort.y;
    viewPos.z = (float) screenPos.z;

    ofLog() << "After screen to viewPort : X : " << viewPos.x << " Y " << viewPos.y << " Z " << viewPos.z;
    return viewPos;
}

ofVec3f Scene3D::viewPortToWorld(ofVec3f viewPos)
{
    if (!current_viewPort.y > 0)
    {
        viewPos.y += (ofGetHeight() - current_viewPort.getHeight());
    }
    else
    {
        viewPos.y -= (ofGetHeight() - current_viewPort.getHeight());
    }
    camera->begin(current_viewPort);
    ofVec3f worldPos = camera->screenToWorld(viewPos);
    camera->end();
    return worldPos;
}



ofVec3f Scene3D::viewPortToScreen(ofVec3f viewPos)
{
    ofVec3f screenPos;
    screenPos.x = (viewPos.x - current_viewPort.x) / ((float) current_viewPort.getHeight() / (float) ofGetHeight());
    screenPos.y = (viewPos.y - current_viewPort.y) / ((float) current_viewPort.getHeight() / (float) ofGetHeight());
    screenPos.z = viewPos.z;
    return viewPos;
}

void Scene3D::mousePressed(int x, int y, int button)
{
    previous_x = x;
    previous_y = y;


    updateViewPorts();

    ofLog() << "Current View Port  Y " << current_viewPort.y << " X " << current_viewPort.x << " Width " << current_viewPort.width << " Height " << current_viewPort.height;

    is_selected = false;
    float closeness = -1;
    std::vector<std::pair<std::shared_ptr<SceneObject>, NodeId>> primitive_pairs = getSceneObjects();
    for (int i = 0; i < primitive_pairs.size(); i++)
    {

        std::shared_ptr<SceneObject> primitive = primitive_pairs[i].first;
        NodeId id = primitive_pairs[i].second;

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

        selectionMesh = primitive->getSelectionBox();

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
                SelectNode(sceneGraph.GetNode(id).value());
                
                ofLog() << "Object : " << i << " selected. ";
            }
        }
    }
}


//void Scene3D::nextCam()
//{
//    current_cam = ++current_cam < cameras.size() ? current_cam : 0;
//    camera = &cameras[current_cam];
//}
//
//void Scene3D::previousCam()
//{
//    current_cam = --current_cam >= 0 ? current_cam : cameras.size() - 1;
//    camera = &cameras[current_cam];
//}

void Scene3D::dragEvent(ofDragInfo dragInfo)
{

    if (!dragInfo.files.empty())
    {
        auto model = std::make_shared<ImportModel>();
        std::string filePath = dragInfo.files[0];// Get the first dropped file
        if (ofFilePath::getFileExt(filePath) == "obj")
        {
            model->loadModel(filePath);// Load the OBJ model
            model->setPosition(0, 0, 0);
            shared_ptr<Node> node = make_shared<Node>("Object ", model);
            sceneGraph.AddNode(node);
        }
        else
        {
            ofLog() << "Not an OBJ file!";
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
    /*if (is_selected)
    {
        camera->lookAt(primitives[selected_primitive].getPosition());
    }*/
}


void Scene3D::keyPressed(int key)
{
    switch (key)
    {
        case 119://w
            is_key_press_w = true;
            break;
        case 100://d
            is_key_press_d = true;
            break;
        case 115://s
            is_key_press_s = true;
            break;
        case 97://a
            is_key_press_a = true;
            break;
        case 57357://up
            is_key_press_up = true;
            break;
        case 57358://right
            is_key_press_right = true;
            break;
        case 57359://down
            is_key_press_down = true;
            break;
        case 57356://left
            is_key_press_left = true;
            break;
        case 113://q
            is_key_press_q = true;
            break;
        case 101://e
            is_key_press_e = true;
            break;
        case 102://f
            is_key_press_f = true;
            break;
        case 103://g
            is_key_press_g = true;
            break;
        case 45://-
            is_key_press_minus = true;
            break;
        case 61:
            is_key_press_plus = true;
            break;
    }
}

void Scene3D::keyReleased(int key)
{
    switch (key)
    {
        case 119://w
            is_key_press_w = false;
            break;
        case 100://d
            is_key_press_d = false;
            break;
        case 115://s
            is_key_press_s = false;
            break;
        case 97://a
            is_key_press_a = false;
            break;
        case 57357://up
            is_key_press_up = false;
            break;
        case 57358://right
            is_key_press_right = false;
            break;
        case 57359://down
            is_key_press_down = false;
            break;
        case 57356://left
            is_key_press_left = false;
            break;
        case 113://q
            is_key_press_q = false;
            break;
        case 101://e
            is_key_press_e = false;
            break;
        case 102://f
            is_key_press_f = false;
            break;
        case 103:
            is_key_press_g = false;
            break;
        case 114://r
            reset();
            break;
        case 45://-
            is_key_press_minus = false;
            break;
        case 61:
            is_key_press_plus = false;
            break;
        case 107://k
            debugger = !debugger;
            break;
        case 'y':
            focus();
            break;
    }
}


void Scene3D::update()
{
    time_current = ofGetElapsedTimef();
    time_elapsed = time_current - time_last;
    time_last = time_current;


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
        camera->tilt(speed_rotation);
    }
    if (is_key_press_down)
    {
        camera->tilt(-speed_rotation);
    }
    if (is_key_press_left)
    {
        camera->pan(speed_rotation);
    }
    if (is_key_press_right)
    {
        camera->pan(-speed_rotation);
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
        camera->setFov(camera->getFov() + 0.2);
    }
    if (is_key_press_g)
    {
        camera->setFov(camera->getFov() - 0.2);
    }
    if (is_key_press_plus)
    {
        camera->roll(speed_rotation);
    }
    if (is_key_press_minus)
    {
        camera->roll(-speed_rotation);
    }
}

std::vector<std::pair<std::shared_ptr<SceneObject>, NodeId>> Scene3D::getSceneObjects()
{

    std::vector<std::shared_ptr<Node>> nodes =  sceneGraph.GetNodes();
    std::vector<std::pair<std::shared_ptr<SceneObject>, NodeId>> sceneObjects{};

    for (const auto& node: nodes)
    {
        auto inner = node->GetInner();
        if (inner) {
            auto prim = std::dynamic_pointer_cast<SceneObject>(inner);
            if (prim)
            {
                sceneObjects.push_back(std::pair(prim, node->GetId()));
            }
        }
    }
    return sceneObjects;
}
