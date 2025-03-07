#pragma once

#pragma once

#include "SceneObject.h"
#include "ofLight.h"
#include "ofMain.h"

class Light : public SceneObject
{
public:

    void lookAt(const ofVec3f& target)
    {
        light->lookAt(target);
    }

    void customDraw() override
    {
        ofPushMatrix();
        light->draw();
        //light->setPosition(ofNode::getPosition());
        ofPopMatrix();
    }

    std::pair<ofVec3f, ofVec3f> getBoundingVertices() override
    {
        ofVec3f pos = getGlobalPosition();
        ofVec3f minVertex = ofVec3f(pos.x - 5, pos.y - 5, pos.z - 5);
        ofVec3f maxVertex = ofVec3f(pos.x + 5, pos.y + 5, pos.z + 5);

        return std::pair(minVertex, maxVertex);
    };

private:
    std::shared_ptr<ofLight> light;
};
