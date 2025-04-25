#pragma once

#include "SceneObject.h"
#include "ofMain.h"
#include "renderer/rayTracer/ray.h"



class Primitive3D : public SceneObject
{
public:
    Primitive3D() {}

    Primitive3D(of3dPrimitive primitive)
    {
        model = primitive;
    }

    void customDraw() override
    {
        if (model.getMesh().getNumVertices() == 0) {
            return;
        }
        if (wireFrame)
        {
            model.drawWireframe();
        }
        else
        {
            model.draw();
        }
    }

    std::pair<ofVec3f, ofVec3f> getBoundingVertices() override
    {
        vector<ofVec3f> vertices = getMeshVertices(model.getMesh());
        return getBoundingVerticesFromVector(vertices);
    };

    void setWireframe(bool on)
    {
        this->wireFrame = on;
    }

    bool* getWireframe()
    {
        return &this->wireFrame;
    }

    ofMesh getMesh() {
        return model.getMesh();
    }

    of3dPrimitive getModel() {
        return model;
    }

private:
    bool wireFrame = false;

protected:
    of3dPrimitive model;
};
