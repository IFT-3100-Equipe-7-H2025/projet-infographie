#pragma once
#include "Scene.h"
#include "ofMain.h"

class RotatingCubeScene : public Scene
{
public:
    void draw() override
    {
        ofPushMatrix();
        ofTranslate(static_cast<float>(ofGetWidth()) / 2.0f, static_cast<float>(ofGetHeight()) / 2.0f);
        ofRotateDeg(angle, 1, 1, 1);

        ofPushStyle();
        ofSetColor(fmod(angle, 255.0f), 255, 255);
        ofDrawBox(0, 0, 0, 200);
        ofPopStyle();

        ofPopMatrix();
        angle = fmod(angle + 1.0f, 360.0f);
    }

    std::string GetName() override { return "Rotating cube"; }

private:
    float angle = 0.0f;
};
