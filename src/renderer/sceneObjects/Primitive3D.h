#pragma once

#include "SceneObject.h"
#include "ofMain.h"
#include "renderer/rayTracer/Ray.h"
#include "RayMaterial.h"



class Primitive3D : public SceneObject
{
public:
    shared_ptr<ofVec3f> center;
    shared_ptr<ofVec3f> scale;
    shared_ptr<ofQuaternion> orientation;


    Primitive3D() {}

    Primitive3D(of3dPrimitive primitive)
    {
        mat = make_shared<Lambert>(ofColor(5, 50, 255));
        ofLog() << "Material created!";
        model = primitive;
        initialize();
    }

    Primitive3D(of3dPrimitive primitive, shared_ptr<RayMaterial> material)
    {
        mat = material;
        ofLog() << "Material created!";
        model = primitive;
        initialize();
    }


    Primitive3D(Primitive3D& other)
    {
        ofLog() << "Here for some reason";
        mat = other.mat;
        initialize();
        model = other.model;
        center = other.center;
        scale = other.scale;
        orientation = other.orientation;

    }

    void copy(Primitive3D& other)
    {
        model = other.model;
        center = other.center;
        scale = other.scale;
        orientation = other.orientation;
        mat = other.mat;
    }

    void initialize()
    {
        center = make_shared<ofVec3f>(getGlobalPosition());
        scale = make_shared<ofVec3f>(getScale());
        orientation = make_shared<ofQuaternion>(getOrientationQuat());
    }

    void customDraw() override
    {
        //ofLog() << "In primitive custom draw";

        if (model.getMesh().getNumVertices() == 0 || !isVisible) {
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
    virtual void update() {}

    virtual void customDraw(ofMatrix4x4 transform) {}

    bool updateSettings()
    {
        time_current = ofGetElapsedTimef();
        time_elapsed = time_current - time_last;

        ofVec3f new_position = getGlobalPosition();
        ofVec3f new_scale = getScale();
        ofQuaternion new_orientation = getOrientationQuat();
        if (*center == new_position && *scale == new_scale && *orientation == new_orientation || time_elapsed < 0.5)
        {
            return false;
        }
        time_last = time_current;

        //else
        //{
        //    ofLog() << "Primitive3D update";
        //    //calculateBbox(); an idea to update the bbox

        //}
        *center = new_position;
        *scale = new_scale;
        *orientation = new_orientation;
        return true;
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

    void setVisible(bool on)
    {
        this->isVisible = on;
    }

    bool* getVisible()
    {
        return &this->isVisible;
    }

    ofMesh getMesh() {
        return model.getMesh();
    }

    of3dPrimitive getModel() {
        return model;
    }

     virtual AABB bounding_box() const
    {
        return bbox;
    };

private:
    bool isVisible = true;
    float time_current;
    float time_elapsed;

    float time_last = 0;

protected:
    of3dPrimitive model;
    bool wireFrame = false;
    ofFloatColor color = ofFloatColor(1.0f, 1.0f, 1.0f, 1.0f);
    AABB bbox;

    
};
