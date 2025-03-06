#pragma once

#include "3dscene/commands/CommandHistory.h"
#include "Scene.h"
#include "createShapes/CreateShapeUI.h"
#include "createShapes/SharedShapeCreationParams.h"
#include "scenegraph/SceneGraph.h"
#include <ofxAssimpModelLoader.h>
#include "ofMain.h"
#include "SceneObject.h"

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
    void DrawModifyNodeSliders(const std::shared_ptr<Node>& node);
    void DrawCommandHistoryWindow();

    void ResetParams(const std::shared_ptr<Node>& node);


    void reset();
    void update() override;
    void mousePressed(int x, int y, int button) override;
    void mouseDragged(int x, int y, int button) override;
    void mouseMoved(int x, int y) override;
    void dragEvent(ofDragInfo dragInfo) override;
    void keyPressed(int key) override;
    void keyReleased(int key) override;
    /*void nextCam();
    void previousCam();*/

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

    std::shared_ptr<SharedShapeCreationParams> sharedParams;

    std::vector<std::unique_ptr<CreateShapeUI>> createShapeUIs;

    ofMaterial material;








    std::shared_ptr<ofCamera> camera;


    std::vector<std::shared_ptr<ofCamera>> cameras;
    ofMesh selectionMesh;
    bool is_selected;
    bool debugger;
    int current_cam = 0;
    int selected_primitive = 0;


    std::vector<std::shared_ptr<ofxAssimpModelLoader>> objects;


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

    float angle_y;
    float angle_x;


    float translate_speed;
    float rotate_speed;
    float speed_translation;
    float speed_rotation;
    float offset_y;
    float offset_x;

    int previous_x;
    int previous_y;

    ofRectangle viewport1;
    ofRectangle viewport2;

    float backgroundColor[4] = {0.1f, 0.1f, 0.1f, 1.0f};
    std::vector<ofVec3f> getPrimitiveVertices(of3dPrimitive& primitive);
    void focus();
    void drawScene();

    std::vector<std::pair<std::shared_ptr<SceneObject>, NodeId>> getSceneObjects();
};
