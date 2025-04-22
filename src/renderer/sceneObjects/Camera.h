#pragma once

#pragma once

#include "SceneObject.h"
#include "ofMain.h"

using ViewPort = ofRectangle;

class Camera : public ofCamera
{
public:
    Camera(): activated(false), drawFrustrum(false) {}

    void setViewPort(ofRectangle p_viewport) {
        viewPort = p_viewport;
    }

    ViewPort getViewPort() {
        return viewPort;
    }
    bool& isActivated() {
        return activated;
    }

    void activate() {
        activated = true;
    }

    void deactivate() {
        activated = false;
    }

    void viewFrustrum() {
        drawFrustrum = true;
    }
    
    void hideFrustrum() {
        drawFrustrum = false;
    }

    bool& frustrumVisible() {
        return drawFrustrum;
    }

private:
    bool activated;
    bool drawFrustrum;
    ViewPort viewPort;

};
