#pragma once

#include "SceneObject.h"
#include "ofMain.h"

class Primitive3D : public SceneObject
{
public:
    Primitive3D(of3dPrimitive primitive) {
        model = primitive;
    }

    void customDraw() override
    {
        model.draw();
    }

    std::pair<ofVec3f, ofVec3f> getBoundingVertices() override
    {
        vector<ofVec3f> vertices = getMeshVertices(model.getMesh());
        return getBoundingVerticesFromVector(vertices);
    };

private:
    of3dPrimitive model;
};
