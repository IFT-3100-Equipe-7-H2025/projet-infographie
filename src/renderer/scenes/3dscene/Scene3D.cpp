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
#include "SetColorCommand.h"
#include "SetMaterialCommand.h"
#include "Shader.h"
#include "imgui.h"
#include "material/ShaderMaterial.h"
#include "of3dPrimitives.h"
#include "ofAppRunner.h"
#include "ofGraphics.h"
#include "renderer/sceneObjects/ImportModel.h"
#include <cmath>
#include <memory>
#include <numbers>
#include "renderer/rayTracer/Ray.h"
#include "renderer/rayTracer/RayObjects/Cube.h"
#include "Material.h"
#include "RayMaterial.h"
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

    this->registeredMaterials.push_back(DEFAULT_MATERIAL);

    ofMaterial specularMaterial;
    specularMaterial.setAmbientColor(ofFloatColor(0.1f, 0.1f, 0.1f));
    specularMaterial.setDiffuseColor(ofFloatColor(0.6f, 0.5f, 0.5f));
    specularMaterial.setSpecularColor(ofFloatColor(0.5f, 0.5f, 0.3f));
    specularMaterial.setShininess(64);
    this->registeredMaterials.push_back(std::make_shared<DefaultMaterial>(std::make_shared<ofMaterial>(specularMaterial), "Specular"));

    ofMaterial emissiveMaterial;
    emissiveMaterial.setShininess(1);
    emissiveMaterial.setAmbientColor(ofFloatColor(0.2f, 0.2f, 0.2f, 1.0f));
    emissiveMaterial.setDiffuseColor(ofFloatColor(0.8f, 0.5f, 0.2f, 1.0f));
    emissiveMaterial.setSpecularColor(ofFloatColor(0.0f, 0.0f, 0.0f, 1.0f));
    emissiveMaterial.setEmissiveColor(ofFloatColor(0.6f, 0.0f, 0.0f, 1.0f));
    this->registeredMaterials.push_back(std::make_shared<DefaultMaterial>(std::make_shared<ofMaterial>(emissiveMaterial), "Emissive"));

    this->registeredMaterials.push_back(std::make_shared<PBRMaterial>("PBR"));

    // Add base light
    auto light = ofLight();
    light.enable();
    light.setDiffuseColor(ofFloatColor(1.0, 1.0, 1.0));
    light.setSpecularColor(ofFloatColor(1.0, 1.0, 1.0));
    light.setPosition(ofVec3f(0.0f, 0.0f, 0.0f));
    light.lookAt(ofVec3f(static_cast<float>(ofGetWidth()) / 2.0f, static_cast<float>(ofGetHeight()) / 2.0f, 0));

    auto light_ptr = std::make_shared<Node>("Light", std::make_shared<Light>(light, LightType::POINT));
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
    auto primCube = PrimitiveCreator::createCube(50, 50, 50);

    shared_ptr<Sphere> ground = make_shared<Sphere>(primGround, 100.0, material_ground);
    shared_ptr<Sphere> center = make_shared<Sphere>(primCenter, 20, material_center);
    shared_ptr<Sphere> left = make_shared<Sphere>(primLeft, 20, material_left);
    shared_ptr<Sphere> bubble = make_shared<Sphere>(primBubble, 18, material_bubble);
    shared_ptr<Sphere> right = make_shared<Sphere>(primRight, 20, material_right);
    ground->setGlobalPosition(Vec3(0.0, 0.0, 0.0));
    center->setGlobalPosition(Vec3(0.0, 0.0, -1.2));
    left->setGlobalPosition(Vec3(-30, 0.0, -1.0));
    bubble->setGlobalPosition(Vec3(-30, 0.0, -1.0));
    right->setGlobalPosition(Vec3(30, 0.0, -1.0));

    //sceneGraph.AddNode(make_shared<Node>("Ground", ground));
    //sceneGraph.AddNode(make_shared<Node>("Center", center));
    //sceneGraph.AddNode(make_shared<Node>("Left", left));
    //sceneGraph.AddNode(make_shared<Node>("Bubble", bubble));
    //sceneGraph.AddNode(make_shared<Node>("Right", right));

    shared_ptr<ofVec3f> reference = make_shared<ofVec3f>(0, 0, 0);

    //sceneGraph.AddNode(make_shared<Node>("Ground", make_shared<Quad>(reference, Vec3(0.0f, 0.0f, 0.0f), Vec3(400.0f, 0.0f, 0.0f), Vec3(0.0f, -400.00f, 0.0f), material_ground)));
    //sceneGraph.AddNode(make_shared<Node>("Center", make_shared<Quad>(Vec3(-20, -20, 0), Vec3(40, 0, 0), Vec3(0, 40, 0), material_center)));
    //sceneGraph.AddNode(make_shared<Node>("Left", make_shared<Quad>(Vec3(30, -20, 10), Vec3(0, 0, 40), Vec3(0, 40, 0), material_left)));
    //sceneGraph.AddNode(make_shared<Node>("Bubble", make_shared<Quad>(Vec3(-20, 30, 10), Vec3(40, 0, 0), Vec3(0, 0, 40), material_bubble)));
    //sceneGraph.AddNode(make_shared<Node>("Right", make_shared<Quad>(Vec3(-20, -30, 50), Vec3(40, 0, 0), Vec3(0, 0, -40), material_right)));
    //sceneGraph.AddNode(make_shared<Node>("Cube", make_shared<Cube>(Vec3(50,50,50), material_ground, primCube)));


    font.load("fonts/JetBrainsMono-Regular.ttf", 12, true, true);


    auto lambert = ofShader();
    lambert.load("shaders/lambert.vert", "shaders/lambert.frag");

    auto gouraud = ofShader();
    gouraud.load("shaders/gouraud.vert", "shaders/gouraud.frag");

    auto phong = ofShader();
    phong.load("shaders/phong.vert", "shaders/phong.frag");

    auto blinn_phong = ofShader();
    blinn_phong.load("shaders/blinn_phong.vert", "shaders/blinn_phong.frag");

    auto gooch = ofShader();
    gooch.load("shaders/gooch.vert", "shaders/gooch.frag");


    this->lightingModels.push_back(std::make_shared<Shader>(DEFAULT_SHADER));
    this->lightingModels.push_back(std::make_shared<Shader>(lambert, "Lambert"));
    this->lightingModels.push_back(std::make_shared<Shader>(gouraud, "Gouraud"));
    this->lightingModels.push_back(std::make_shared<Shader>(phong, "Phong"));
    this->lightingModels.push_back(std::make_shared<Shader>(blinn_phong, "Blinn-Phong"));
    this->lightingModels.push_back(std::make_shared<Shader>(gooch, "Gooch"));

    this->selectedLightingModel = this->lightingModels[0];
}

void Scene3D::draw()
{
    time_start_draw = ofGetElapsedTimef();


    ofClear(clearColor);
    ofSetColor(0, 255, 0);
    this->DrawSceneGraphWindow();
    this->DrawSelectedNodeWindow();
    this->DrawCommandHistoryWindow();
    this->DrawModifyMaterialWindow();
    this->DrawSelectLightingModelWindow();

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
            rayImage = camera->getRayImage();
            ofDisableDepthTest();
            rayImage.draw(viewPort.getX() + viewPort.getWidth() - new_width, viewPort.getY(), new_width, new_height);
            float percent = camera->portionDone() * 100;
            string time_left = camera->timeLeft();
            string perc = std::format("Time Left: {} , %: {:.4f}", time_left, percent);
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
    time_current_draw = ofGetElapsedTimef();
    time_elapsed_draw = time_current_draw - time_start_draw;
}

void Scene3D::drawScene()
{
    if (this->selectedLightingModel->GetName() == "Default") { sceneGraph.Draw(); }
    else
    {
        auto light = this->FindLight();
        if (light)
        {
            sceneGraph.Draw(this->selectedLightingModel, light->getPosition());
        }
    }

    if (is_selected)
    {
        selectionMesh.enableColors();
        selectionMesh.setColorForIndices(255, 255, 0);

        selectionMesh.draw();
    }
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


            if (auto light = std::dynamic_pointer_cast<Light>(this->selectedNode->get()->GetInner()); light)
            {
                this->DrawModifyLightSliders(light);
            }
            if (this->selectedNode->get()->GetName() == "World 0") {
                ImGui::Text("World node");
                float color[4] = {clearColor.r / 255.0f, clearColor.g / 255.0f, clearColor.b / 255.0f, clearColor.a / 255.0f};
                ImGui::ColorEdit4("World Color", color);
                clearColor.set(color[0] * 255.0f, color[1] * 255.0f, color[2] * 255.0f, color[3] * 255.0f);
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

            if (const std::shared_ptr<Primitive3D> prim = std::dynamic_pointer_cast<Primitive3D>(this->selectedNode->get()->GetInner()); prim) { ImGui::Checkbox("Visible", prim->getVisible()); }
            if (const std::shared_ptr<Primitive3D> prim = std::dynamic_pointer_cast<Primitive3D>(this->selectedNode->get()->GetInner()); prim) { ImGui::Checkbox("Wireframe", prim->getWireframe()); }

            if (ImGui::CollapsingHeader("Add child", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::Checkbox("Use BVH", &sharedParams->useBVH);
                ImGui::ColorEdit4("Color", sharedParams->color);
                ofFloatColor color(sharedParams->color[0], sharedParams->color[1], sharedParams->color[2], sharedParams->color[3]);
                
                sharedParams->material->setColor(color);

                if (sharedParams->mat == matType::GlassT)
                {
                    ImGui::SliderFloat("Refract Index", &sharedParams->refract, 0.1f, 10.0f);
                    sharedParams->material = make_shared<Dielectric>(Dielectric(sharedParams->refract));
                }
                else if (sharedParams->mat == matType::MetalT)
                {
                    ImGui::SliderFloat("Metal Fuzz", &sharedParams->fuzz, 0.1f, 10.0f);
                    sharedParams->material = make_shared<Metal>(Metal(color, sharedParams->fuzz));
                }
                else if (sharedParams->mat == matType::LambertT)
                {
                    sharedParams->material = make_shared<Lambert>(Lambert(color));
                }
                else if (sharedParams->mat == matType::DiffuseLightT)
                {
                    sharedParams->material = make_shared<DiffuseLight>(DiffuseLight(color));
                }

                int selected = static_cast<int>(sharedParams->mat);
                if (ImGui::Combo("Choose Material", &selected, materialLabels, 4))
                {
                    ofLog() << "Choosing material" << endl;
                    sharedParams->mat = static_cast<matType>(selected);

                }

                for (auto& createShapeUI: this->createShapeUIs)
                {
                    createShapeUI->Draw();
                }
            }
        }
        ImGui::End();
    }
}


void Scene3D::DrawModifyLightSliders(const std::shared_ptr<Light>& light)
{
    bool tempIsEnabled = light->GetIsEnabled();
    if (ImGui::Checkbox("Activate", &tempIsEnabled))
    {
        if (tempIsEnabled)
        {
            light->Enable();
        }
        else
        {
            light->Disable();
        }
    }

    LightType lightType = light->GetLightType();

    bool isDirectional = lightType == LightType::DIRECTIONAL;
    bool isSpotlight = lightType == LightType::SPOT;
    bool isPointLight = lightType == LightType::POINT;
    bool isAmbientLight = lightType == LightType::AMBIENT;
    if (ImGui::Checkbox("Directional", &isDirectional))
    {
        light->SetLightType(LightType::DIRECTIONAL);
    }
    ImGui::SameLine();
    if (ImGui::Checkbox("Spotlight", &isSpotlight))
    {
        light->SetLightType(LightType::SPOT);
    }
    ImGui::SameLine();
    if (ImGui::Checkbox("Point light", &isPointLight))
    {
        light->SetLightType(LightType::POINT);
    }
    ImGui::SameLine();
    if (ImGui::Checkbox("Ambient light", &isAmbientLight)) { light->SetLightType(LightType::AMBIENT); }


    lightType = light->GetLightType();

    isDirectional = lightType == LightType::DIRECTIONAL;
    isSpotlight = lightType == LightType::SPOT;
    isPointLight = lightType == LightType::POINT;
    isAmbientLight = lightType == LightType::AMBIENT;

    if (!isAmbientLight)
    {
        ofFloatColor diffuseColor = light->GetDiffuseColor();
        float diffuseColorArr[3] = {diffuseColor.r, diffuseColor.g, diffuseColor.b};
        if (ImGui::ColorEdit3("Diffuse color", diffuseColorArr)) { light->SetDiffuseColor(ofFloatColor(diffuseColorArr[0], diffuseColorArr[1], diffuseColorArr[2])); }

        ofFloatColor specularColor = light->GetSpecularColor();
        float specularColorArr[3] = {specularColor.r, specularColor.g, specularColor.b};
        if (ImGui::ColorEdit3("Specular color", specularColorArr)) { light->SetSpecularColor(ofFloatColor(specularColorArr[0], specularColorArr[1], specularColorArr[2])); }
    }
    else
    {
        ofFloatColor ambientColor = light->GetAmbientColor();
        float ambientColorArr[3] = {ambientColor.r, ambientColor.g, ambientColor.b};
        if (ImGui::ColorEdit3("Ambient color", ambientColorArr)) { light->SetAmbientColor(ofFloatColor(ambientColorArr[0], ambientColorArr[1], ambientColorArr[2])); }
    }

    if (isSpotlight)
    {
        float spotCutOff = light->GetSpotlightCutOff();
        if (ImGui::SliderFloat("Spotlight cut off", &spotCutOff, 0, 90))
        {
            light->SetSpotlightCutOff(spotCutOff);
        }

        float spotConcentration = light->GetSpotConcentration();
        if (ImGui::SliderFloat("Spotlight concentration", &spotConcentration, 0, 128))
        {
            light->SetSpotConcentration(spotConcentration);
        }
    }

    float attenuationConstant = light->GetAttenuationConstant();
    if (ImGui::SliderFloat("Attenuation constant", &attenuationConstant, 0, 1)) { light->SetAttenuationConstant(attenuationConstant); }

    float attenuationLinear = light->GetAttenuationLinear();
    if (ImGui::SliderFloat("Attenuation linear", &attenuationLinear, 0, 0.01, "%.5f")) { light->SetAttenuationLinear(attenuationLinear); }

    float attenuationQuadratic = light->GetAttenuationQuadratic();
    if (ImGui::SliderFloat("Attenuation quadratic", &attenuationQuadratic, 0, 0.001, "%.6f")) { light->SetAttenuationQuadratic(attenuationQuadratic); }
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
        ImGui::SliderInt("Samples", &camera->getSamples(), 1, 4000);
        ImGui::SliderInt("Depth", &camera->getDepth(), 1, 100);
        ImGui::SliderInt("Resolution", &camera->getWidth(), 10, 4000);
        ImGui::SliderFloat("Portion of Screen", &camera->getScreenCrop(), 0.05, 1);
        ofColor color1 = camera->getAmbient1();
        float colorf[4] = {color1.r / 255.0f, color1.g / 255.0f, color1.b / 255.0f, color1.a / 255.0f};
        if (ImGui::ColorEdit4("Ambient 1##ChangeColor", colorf))
        {
            camera->setAmbient1(ofColor(colorf[0] * 255, colorf[1] * 255, colorf[2] * 255, colorf[3] * 255));
        }
        ofColor color2 = camera->getAmbient2();
        float colorf2[4] = {color2.r / 255.0f, color2.g / 255.0f, color2.b / 255.0f, color2.a / 255.0f};
        if (ImGui::ColorEdit4("Ambient 2##ChangeColor", colorf2))
        {
            camera->setAmbient2(ofColor(colorf2[0] * 255, colorf2[1] * 255, colorf2[2] * 255, colorf2[3] * 255));
        }
        if (ImGui::Button("Save Render"))
        {
            camera->saveImage();
        }
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
        if (auto light = std::dynamic_pointer_cast<Light>(inner); light) { light->setOrientation(newRotation); }
        else { inner->setOrientation(newRotation); }
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

    // Color
    if (const auto primitive = std::dynamic_pointer_cast<Primitive3D>(node->GetInner()); primitive)
    {
        const ofFloatColor currentColor = primitive->GetColor();
        if (ImGui::ColorEdit4("Color##ChangeColor", this->color))
        {
            primitive->SetColor(ofFloatColor(this->color[0], this->color[1], this->color[2], this->color[3]));
        }
        if (ImGui::IsItemActivated())
        {
            this->initialColor = currentColor;
        }
        if (ImGui::IsItemDeactivatedAfterEdit())
        {
            this->history.executeCommand(std::make_shared<SetColorCommand>(node, ofFloatColor(this->color[0], this->color[1], this->color[2], this->color[3]), this->initialColor));
        }

        matType type = getMaterialType(primitive->getMaterial());
        ofColor color = ofColor(currentColor[0] * 255.0f, currentColor[1] * 255.0f, currentColor[2] * 255.0f, currentColor[3] * 255.0f);
        if (type == matType::GlassT)
        {
            float refraction = primitive->getMaterial()->getRefractionIndex();
            ImGui::SliderFloat("Change Refract", &refraction, 0.1f, 10.0f);
            primitive->setMaterial(make_shared<Dielectric>(refraction));
        }
        else if (type == matType::MetalT)
        {
            float fuzz = primitive->getMaterial()->getFuzz();
            ImGui::SliderFloat("Change Fuzz", &fuzz, 0.1f, 10.0f);
            primitive->setMaterial(make_shared<Metal>(color, fuzz));
        }
        else if (type == matType::LambertT)
        {
            primitive->setMaterial(make_shared<Lambert>(color));
        }
    }
}

void Scene3D::DrawModifyMaterialWindow()
{
    if ((*this->selectedNode) != nullptr)
    {
        ImGui::SetNextWindowPos(ImVec2(320, 440), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(300, 400), ImGuiCond_FirstUseEver);
        ImGui::Begin("Select material");
        ImGui::Text("Selected node: %s", (*this->selectedNode)->GetName().c_str());
        ImGui::Text("Material: %s", (*this->selectedNode)->GetMaterial()->GetName().c_str());

        ImGui::Text("Available materials:");
        for (const std::shared_ptr<Material>& material: this->registeredMaterials)
        {
            if (ImGui::Button(material->GetName().c_str()))
            {
                this->history.executeCommand(std::make_shared<SetMaterialCommand>(*this->selectedNode, material));
            }
        }
        ImGui::End();
    }
}

void Scene3D::DrawSelectLightingModelWindow()
{
    ImGui::SetNextWindowPos(ImVec2(630, 30), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(300, 400), ImGuiCond_FirstUseEver);
    ImGui::Begin("Select scene lighting model");
    ImGui::Text("Lighting model: %s", this->selectedLightingModel->GetName().c_str());
    for (const std::shared_ptr<Shader>& shader: this->lightingModels)
    {
        if (ImGui::Button(shader->GetName().c_str())) { this->selectedLightingModel = shader; }
    }
    ImGui::End();
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

    if (const auto primitive = std::dynamic_pointer_cast<Primitive3D>(node->GetInner()); primitive)
    {
        const ofFloatColor currentColor = primitive->GetColor();
        color[0] = currentColor.r;
        color[1] = currentColor.g;
        color[2] = currentColor.b;
        color[3] = currentColor.a;
    }

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
            ofMesh mesh = model->getCombinedMesh();
            shared_ptr<RayMaterial> lambert = make_shared<Dielectric>(1.3f);
            RayMesh rayMesh = RayMesh(lambert, mesh);

            ComposedShape shape = ComposedShape(make_shared<BvhNode>(rayMesh), lambert);

            shared_ptr<Node> node = make_shared<Node>("Object ", make_shared<ComposedShape>(shape));
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
            break;
    }
}

void Scene3D::keyPressed(ofKeyEventArgs& key)
{
    ofLog() << "Key pressed: " << key.keycode << " with modifiers: " << key.modifiers;
    if (key.hasModifier(OF_KEY_CONTROL))
    {
        switch (charToLower(key.keycode))
        {
            case 'z'://z
                ofLog() << "Undo";
                this->history.undo();
                break;
            case 'y'://y
                ofLog() << "Redo";
                this->history.redo();
                break;
        }
        return;
    }
    else { this->keyPressed(key.key); }
}

void Scene3D::keyReleased(ofKeyEventArgs& key)
{
    key.key = charToLower(key.key);
    if (key.modifiers == 0)
    {
        switch (key.key)
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
                ofLog() << "focus";
                storeCameraRotation();
                focus();
                applyCameraRotation();
                break;
        }
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
    //time_left = std::fmax(0, std::min(1.0f / ofGetFrameRate() - time_elapsed, 1.0f / ofGetTargetFrameRate() - time_elapsed));
    //time_left = std::fmin(std::fmax(0, (1.0f / ofGetFrameRate()) - time_elapsed), 1.0f / 30.0f);
    //time_left = (((1.0f / ofGetFrameRate()) - time_elapsed) * 0.98f) * (1.0f / fmax(1.0f, fabs(ofGetFrameRate() - ofGetTargetFrameRate())));
     time_left = 1.0f / fmax(45.0f, ofGetFrameRate()) - time_elapsed;
    //time_left = 1.0f / 240.0f;
    time_elapsed_timer = time_current - time_last_timer;
    time_last = time_current;

    if (time_elapsed_timer > 5)
    {
        ofLog() << "Target Fps" << ofGetTargetFrameRate() << endl;
        ofLog() << "Real Fps" << ofGetFrameRate() << endl;

        time_last_timer = time_current;
        ofLog() << "Time elapsed" << time_elapsed << " Time Left : " << time_left << endl;
        ofLog() << "Time elapsed draw" << time_elapsed_draw << " Time Left : " << time_left << endl;
        ofLog() << "target frame rate" << ofGetTargetFrameRate() << " frame rate : " << ofGetFrameRate() << endl;
        ofLog() << "Time elapsed draw" << time_elapsed_draw << " Time Left : " << time_left << endl;

        for (auto& camera : cameras) {
            cameraRayCount = 0;
            if (camera->isRayTracing()) {
                cameraRayCount++;
            }
        }
     
    }

    time_elapsed += ofGetElapsedTimef() - time_last;

    exportRayTrace(time_left);

    time_last = ofGetElapsedTimef();

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


void Scene3D::saveImage(ofImage rayTrace)
{
    int width = rayTrace.getWidth();
    int height = rayTrace.getHeight();
    string filename = "rayImage_" + ofToString(width) + "x " + ofToString(height) + ".png";
    ofFileDialogResult saveDialog = ofSystemSaveDialog("default.png", "Save your image");
    if (saveDialog.bSuccess)
    {
        rayTrace.save(saveDialog.getPath() + ".png");
    }
}

void Scene3D::exportRayTrace(float time_left)
{
    float current_time = ofGetElapsedTimef();
    float elapsed_time = 0;
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
    HitRecord rec;

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


double Scene3D::hitAnything(const Ray& r, Interval ray_t, HitRecord& rec) {
    HitRecord temp_rec;
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
