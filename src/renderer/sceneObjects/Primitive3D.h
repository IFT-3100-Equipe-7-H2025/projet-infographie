#pragma once

#include "SceneObject.h"
#include "ofMain.h"

class Primitive3D : public SceneObject
{
public:
    Primitive3D(of3dPrimitive primitive)
    {
        model = primitive;
    }

    void customDraw() override
    {
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

    void SetColor(const ofColor& color)
    {
        auto& mesh = model.getMesh();
        mesh.clearColors();

        auto floatColor = ofFloatColor(color);
        for (size_t i = 0; i < mesh.getNumVertices(); ++i)
        {
            mesh.addColor(floatColor);
        }

        this->color = floatColor;
    }

    [[nodiscard]] const ofFloatColor& GetColor() const
    {
        return this->color;
    }

private:
    of3dPrimitive model;
    bool wireFrame = false;
    ofFloatColor color = ofFloatColor(1.0f, 1.0f, 1.0f, 1.0f);
};
