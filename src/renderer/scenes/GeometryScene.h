#pragma once
#include "Scene.h"
#include "imgui.h"
#include "of3dPrimitives.h"
#include "ofMain.h"
#include "ofMesh.h"
#include <vector>
#include <ofxAssimpModelLoader.h>


class GeometryScene : public Scene
{
public:
    void draw() override;
    void setup() override;
    void reset();
    void update() override;
    void mousePressed(int x, int y, int button) override;
    void dragEvent(ofDragInfo dragInfo) override;
    void keyPressed(int key) override;
    void keyReleased(int key) override;
    void nextCam();
    void previousCam();

    std::string GetName() override { return "Geometry Scene"; }

private:
    ofCamera* camera;
    vector<ofCamera> cameras;
    ofMesh selectionMesh;
    bool is_selected;
    int current_cam = 0;


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

    float backgroundColor[4] = {0.1f, 0.1f, 0.1f, 1.0f};
    static of3dPrimitive createTriangle();
    static ofMesh createBox(ofVec3f minVertex, ofVec3f maxVertex);
    void getBoundingBox(ofMesh& mesh, ofVec3f& minVertex, ofVec3f& maxVertex);
};
