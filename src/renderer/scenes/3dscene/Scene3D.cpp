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
#include <ranges>
#include <cmath>
#include <numbers>
#include "renderer/ImportModel.h"
#include "SceneObject.h"
#include "Light.h"

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

    auto light_ptr = std::make_shared<Node>("Light", std::make_shared<Light>(light));
    this->sceneGraph.AddNode(light_ptr);


    // Add base camera
    /*camera = std::make_shared<ofCamera>();
    camera->setPosition(0, 0, -200);
    camera->lookAt(ofVec3f(0, 0, 0));
    auto cam_ptr = std::make_shared<Node>("Camera", camera);
    this->sceneGraph.AddNode(cam_ptr);*/
    translate_speed = 1000;
    rotate_speed = 100;

    for (int i = 0; i < 6; i++)
    {
        shared_ptr<ofCamera> new_cam = make_shared<ofCamera>();
        new_cam->lookAt(ofVec3f(0, 0, 0));
        cameras.push_back(new_cam);
    }

    camera = cameras[0];
    current_cam = 0;

    viewport1.set(0, 0, ofGetWidth() / 2, ofGetHeight());
    //viewport2.set(ofGetWidth() / 2, 0, ofGetWidth(), ofGetHeight());
   
}

void Scene3D::draw()
{
    this->DrawSceneGraphWindow();
    this->DrawSelectedNodeWindow();
    this->DrawCommandHistoryWindow();


    cameras[0]->begin();
    drawScene();
    cameras[0]->drawFrustum();
    
    cameras[0]->end();

    //cameras[1]->begin(viewport2);
    //drawScene();
    //cameras[1]->drawFrustum();

    //cameras[1]->end();

    /*ofNoFill();
    ofDrawRectangle(viewport1);
    ofDrawRectangle(viewport2);*/

}

void Scene3D::drawScene() {
    

    material.begin();
    sceneGraph.Draw();

    if (is_selected)
    {
        ofSetColor(255, 255, 255);
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
                ofVec3f cameraForward = camera->getLookAtDir();
                ofVec3f cameraRight = camera->getSideDir();
                ofVec3f cameraUp = camera->getUpDir();

                ofVec3f current_pos = selectedNode->get()->GetInner()->getGlobalPosition();
                ofVec3f current_screen_pos = camera->worldToScreen(current_pos);

                ofVec3f current = ofVec3f(x, y, current_screen_pos.z);
                ofVec3f past = ofVec3f(previous_x, previous_y, current_screen_pos.z);

                ofVec3f new_pos = camera->screenToWorld(current_screen_pos + (current - past));


                ofVec3f translate = new_pos - current_pos;

                selectedNode->get()->GetInner()->setGlobalPosition(new_pos);
            }

            break;
        case 2://right
            if (is_selected)
            {
               ofVec3f cameraForward = camera->getLookAtDir();
                ofVec3f cameraRight = camera->getSideDir();
                ofVec3f cameraUp = camera->getUpDir();

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

void Scene3D::mousePressed(int x, int y, int button)
{
    previous_x = x;
    previous_y = y;

    ofVec3f rayOrigin = camera->getPosition();
    ofVec3f rayDirection = camera->screenToWorld(ofVec3f(x, y, 0)) - rayOrigin;
    rayDirection.normalize();
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
                camera->worldToScreen(maxVertex),
                camera->worldToScreen(ofVec3f(minVertex.x, maxVertex.y, maxVertex.z)),
                camera->worldToScreen(ofVec3f(minVertex.x, minVertex.y, maxVertex.z)),
                camera->worldToScreen(ofVec3f(maxVertex.x, minVertex.y, maxVertex.z)),

                camera->worldToScreen(minVertex),
                camera->worldToScreen(ofVec3f(minVertex.x, maxVertex.y, minVertex.z)),
                camera->worldToScreen(ofVec3f(minVertex.x, minVertex.y, minVertex.z)),
                camera->worldToScreen(ofVec3f(maxVertex.x, minVertex.y, minVertex.z)),
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
            minX = min(minX, corner.x);
            maxX = max(maxX, corner.x);
            minY = min(minY, corner.y);
            maxY = max(maxY, corner.y);
        }

        ofLog() << "Clicked At x:" << x << " y: " << y;
        ofLog() << "Box at minX" << minX << " maxX : " << maxX << "minY" << minY << " maxY : " << maxY;

        if (minX < x && x < maxX && minY < y && y < maxY)
        {
            if (new_closeness < closeness || closeness == -1)
            {
                closeness = new_closeness;
                selectionMesh = primitive->getSelectionBox();
                SelectNode(sceneGraph.GetNode(id).value());
                is_selected = true;
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
            //previousCam();
            break;
        case 61:
            //nextCam();
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
}

std::vector<std::pair<std::shared_ptr<SceneObject>, NodeId>> Scene3D::getSceneObjects()
{

    std::vector<std::shared_ptr<Node>> nodes =  sceneGraph.GetNodes();
    std::vector<std::pair<std::shared_ptr<SceneObject>, NodeId>> sceneObjects{};
    for (const auto& node : nodes) {
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


void Scene3D::mouseMoved(int x, int y)
{
 /*   previous_x = x;
    previous_y = y;

    if (ofRectangle::getIn)*/
}