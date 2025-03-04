#pragma once
#include "Scene.h"
#include "imgui.h"
#include "of3dPrimitives.h"
#include "ofMain.h"
#include "ofMesh.h"
#include <vector>


class GeometryScene : public Scene
{
public:
    void draw() override;
    void setup() override;
    void update() override;
    void mousePressed(int x, int y, int button) override {};
    void keyPressed(int key) override;
    void keyReleased(int key) override;

    std::string GetName() override { return "Geometry Scene"; }

private:
    ofCamera camera;


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


    float camera_speed;
    float speed_translation;
    float offset_y;
    float offset_x;

    float backgroundColor[4] = {0.1f, 0.1f, 0.1f, 1.0f};
    static of3dPrimitive createTriangle();
};
