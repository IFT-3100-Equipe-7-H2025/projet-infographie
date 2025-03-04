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
    void mousePressed(int x, int y, int button) override {};

    std::string GetName() override { return "Geometry Scene"; }

private:
    float backgroundColor[4] = {0.1f, 0.1f, 0.1f, 1.0f};
    static of3dPrimitive createTriangle();
};
