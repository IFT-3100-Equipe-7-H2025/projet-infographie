#pragma once
#include "PrimitiveCreator.h"
#include "Scene.h"
#include "imgui.h"
#include "of3dPrimitives.h"
#include "ofMain.h"
#include "ofMesh.h"
#include <ofxAssimpModelLoader.h>
#include <vector>


class GeometryScene : public Scene
{

    friend class Scene3D;

public:
    void draw() override;
    void setup() override;
    void reset();
    void update() override;
    void mousePressed(int x, int y, int button) override;
    void mouseDragged(int x, int y, int button) override;
    void dragEvent(ofDragInfo dragInfo) override;
    void keyPressed(ofKeyEventArgs& key) override;
    void keyReleased(ofKeyEventArgs& key) override;
    void nextCam();
    void previousCam();

    std::string GetName() override { return "Geometry Scene"; }

private:
    ofCamera* camera;
    vector<ofCamera> cameras;
    ofMesh selectionMesh;
    bool is_selected;
    bool debugger;
    int current_cam = 0;
    int selected_primitive = 0;


    std::vector<std::shared_ptr<ofxAssimpModelLoader>> objects;
    vector<of3dPrimitive> primitives;


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

    float backgroundColor[4] = {0.1f, 0.1f, 0.1f, 1.0f};
    ofMesh createBox(of3dPrimitive& primitive);
    void getBoundingBox(of3dPrimitive& primitive, ofVec3f& minVertex, ofVec3f& maxVertex);
    vector<ofVec3f> getPrimitiveVertices(of3dPrimitive& primitive);
    void focus();
};
