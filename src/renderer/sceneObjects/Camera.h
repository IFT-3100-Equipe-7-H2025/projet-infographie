#pragma once

#pragma once

#include "SceneObject.h"
#include "ofLight.h"
#include "ofMain.h"

using ViewPort = ofRectangle;

class Camera : public ofCamera
{
public:
    void setViewPort(ofRectangle p_viewport) {
        viewPort = p_viewport;
    }

    ViewPort getViewPort() {
        return viewPort;
    }
    bool isActivated() {
        return activated;
    }

private:
    bool activated;
    ViewPort viewPort;

};
