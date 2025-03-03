#pragma once
#include "ofMain.h"

enum class CursorState {
    Default,
    HoverButton,
    WaitingSecondClick,
    DrawPoint,
    DrawPrimitive
};

class CursorManager {
public:
    CursorManager();
    void setup();
    void draw();
    void drawForeground();

    void setState(CursorState newState);
    CursorState getState() const;

private:
    CursorState state;

    ofImage imgDefault;
    ofImage imgHoverButton;
    ofImage imgWaiting;
    ofImage imgDrawPoint;
    ofImage imgDrawPrimitive;
};
