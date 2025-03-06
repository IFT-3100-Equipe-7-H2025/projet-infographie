#pragma once

#pragma once

#include "SceneObject.h"
#include "ofMain.h"
#include "ofLight.h"

class Light : public SceneObject
{
public:

    Light(ofLight light)
    {
        this->light = std::make_shared<ofLight>(light);
    }

    void enable()
    {
        light->enable();
    }

    void setDiffuseColor(const ofFloatColor& color)
    {
        light->setDiffuseColor(color);
    }

    void setSpecularColor(const ofFloatColor& color)
    {
        light->setSpecularColor(color);
    }

    void setAmbientColor(const ofFloatColor& color)
    {
        light->setAmbientColor(color);
    }

    void lookAt(const ofVec3f& target)
    {
        light->lookAt(target);
    }

    void customDraw() override
    {
       ofPushMatrix();
       light->draw();
       light->setPosition(ofNode::getPosition());
       ofPopMatrix();
    }

    std::pair<ofVec3f, ofVec3f> getBoundingVertices() override {
        ofVec3f pos = getGlobalPosition();
        ofVec3f minVertex = ofVec3f(pos.x - 5, pos.y - 5, pos.z - 5);
        ofVec3f maxVertex = ofVec3f(pos.x + 5, pos.y + 5, pos.z + 5);

        return std::pair(minVertex, maxVertex);
    };

private:
    std::shared_ptr<ofLight> light;
};
