#pragma once

#include "3dscene/commands/CommandHistory.h"
#include "Light.h"
#include "Scene.h"
#include "SceneObject.h"
#include "Shader.h"
#include "createShapes/CreateShapeUI.h"
#include "createShapes/SharedShapeCreationParams.h"
#include "ofMain.h"
#include "sceneObjects/SceneObject.h"
#include "scenegraph/SceneGraph.h"
#include <ofxAssimpModelLoader.h>

class Scene3D : public Scene
{
public:
    Scene3D();

    void setup() override;

    void draw() override;

    std::string GetName() override { return "3D scene"; }

    void SelectNode(const std::shared_ptr<Node>& node);

    void AddNode(std::shared_ptr<Node> node) { sceneGraph.AddNode(std::move(node)); }
    void ShowChildren(const std::shared_ptr<Node>& node);

    void DrawSceneGraphWindow();
    void DrawSelectedNodeWindow();
    void DrawModifyMaterialWindow();
    void DrawSelectLightingModelWindow();

    void DrawModifyLightSliders(const std::shared_ptr<Light>& light);
    void DrawModifyCameraNodeSliders(const std::shared_ptr<Node>& node, shared_ptr<ofCamera> camera);

    void DrawModifyNodeSliders(const std::shared_ptr<Node>& node);

    void DrawCommandHistoryWindow();

    void ResetParams(const std::shared_ptr<Node>& node);


    void reset();
    void update() override;
    void mousePressed(int x, int y, int button) override;
    void mouseDragged(int x, int y, int button) override;
    void mouseReleased(int x, int y, int button) override;
    void dragEvent(ofDragInfo dragInfo) override;
    void keyPressed(int key) override;
    void keyPressed(ofKeyEventArgs& key) override;
    void keyReleased(ofKeyEventArgs& key) override;
    void windowResized(int w, int h) override;
    /*void nextCam();
    void previousCam();*/

    // Finds the first light in the scene graph
    // Returns a shared pointer to the first light found in the scene graph, or nullptr if no light is found.
    std::shared_ptr<Light> FindLight()
    {
        std::shared_ptr<Light> light = nullptr;
        for (const auto& node: sceneGraph.GetNodes())
        {
            if (auto lightNode = std::dynamic_pointer_cast<Light>(node->GetInner()); lightNode)
            {
                light = lightNode;
                return light;
            }
        }
        return light;
    }

private:
    CommandHistory history;

    SceneGraph sceneGraph;
    std::shared_ptr<std::shared_ptr<Node>> selectedNode;

    float translate[3] = {0.0f, 0.0f, 0.0f};
    glm::vec3 initialPosition;// Used to store the initial position of the selected node when using the sliders, so that we can undo the change in a single command

    float scale[3] = {1.0f, 1.0f, 1.0f};
    glm::vec3 initialScale;// Used to store the initial scale of the selected node when using the sliders, so that we can undo the change in a single command

    float rotate[3] = {0.0f, 0.0f, 0.0f};
    glm::quat initialRotation;// Used to store the initial rotation of the selected node when using the sliders, so that we can undo the change in a single command

    float color[4] = {1.0f, 1.0f, 1.0f, 1.0f};
    ofFloatColor initialColor;// Used to store the initial color of the selected node when using the sliders, so that we can undo the change in a single command

    float ambientLight[3] = {0.2f, 0.2f, 0.2f};
    ofFloatColor initialAmbientLight;// Used to store the initial ambient light color of the selected node when using the sliders, so that we can undo the change in a single command

    float specularLight[3] = {0.5f, 0.5f, 0.5f};
    ofFloatColor initialSpecularLight;// Used to store the initial specular light color of the selected node when using the sliders, so that we can undo the change in a single command

    float diffuseLight[3] = {0.5f, 0.5f, 0.5f};
    ofFloatColor initialDiffuseLight;// Used to store the initial diffuse light color of the selected node when using the sliders, so that we can undo the change in a single command

    float lightAttenuation[3] = {1.0f, 0.0f, 0.0f};
    glm::vec3 initialLightAttenuation;

    float spotCutOff = 45.0f;
    float initialSpotCutOff;

    float spotConcentration = 128.0f;
    float initialSpotConcentration;

    float fov = 0;
    float initialFov = 0;

    std::shared_ptr<SharedShapeCreationParams> sharedParams;

    std::vector<std::unique_ptr<CreateShapeUI>> createShapeUIs;

    using Toggled = bool;
    using DrawFrustum = bool;

    map<NodeId, std::pair<weak_ptr<ofCamera>, pair<Toggled, DrawFrustum>>> cameraMap;

    std::shared_ptr<ofCamera> camera;
    NodeId current_camera_id;
    ofRectangle current_viewPort;


    ofMesh selectionMesh;
    ofVec3f initialSelectedPosition;
    ofVec3f initialSelectedScale;


    int cameraTranslateCount = 0;
    int cameraRotateCount = 0;
    ofVec3f initialCameraPosition;
    glm::quat initialCameraRotation;
    float initialCameraFov;


    ofRectangle onScreenCorners;
    bool is_selected;
    bool debugger;


    float time_current;
    float time_elapsed;
    float time_last;

    bool is_key_press_up = false;
    bool is_key_press_right = false;
    bool is_key_press_down = false;
    bool is_key_press_left = false;

    bool is_key_press_w = false;
    bool is_key_press_a = false;
    bool is_key_press_s = false;
    bool is_key_press_d = false;
    bool is_key_press_q = false;
    bool is_key_press_e = false;
    bool is_key_press_f = false;
    bool is_key_press_g = false;
    bool is_key_press_plus = false;
    bool is_key_press_minus = false;

    float translate_speed;
    float rotate_speed;
    float speed_translation;
    float speed_rotation;

    int pressed_x;
    int pressed_y;
    int previous_x;
    int previous_y;

    using ViewPort = ofRectangle;

    std::vector<std::pair<shared_ptr<ofCamera>, pair<ViewPort, DrawFrustum>>> cameras;

    ofRectangle viewport1;
    ofRectangle viewport2;

    float backgroundColor[4] = {0.1f, 0.1f, 0.1f, 1.0f};
    std::vector<ofVec3f> getPrimitiveVertices(of3dPrimitive& primitive);
    void focus();
    void drawScene();

    std::vector<std::pair<std::shared_ptr<SceneObject>, NodeId>> getSceneObjects() const;
    void updateViewPorts();
    ofVec3f worldToViewPort(ofVec3f worldPos) const;
    ofVec3f screenToViewPort(ofVec3f screenPos) const;
    ofVec3f viewPortToWorld(ofVec3f worldPos) const;
    ofVec3f viewPortToScreen(ofVec3f viewPos) const;

    bool ortho;
    void toggleOrtho();

    void storeCameraRotation();
    void applyCameraRotation();

    void storeCameraTranslation();
    void applyCameraTranslation();

    int getCameraRotationCommands() const;
    int getCameraTranslationCommands() const;

    int charToLower(int key);

    std::queue<std::shared_ptr<Command>> commandQueue;

    void ExecuteQueuedCommands()
    {
        while (!commandQueue.empty())
        {
            auto command = commandQueue.front();
            commandQueue.pop();
            this->history.executeCommand(command);
        }
    }

    std::vector<std::shared_ptr<Material>> registeredMaterials;
    std::vector<std::shared_ptr<Shader>> lightingModels;
    std::shared_ptr<Shader> selectedLightingModel;
};
