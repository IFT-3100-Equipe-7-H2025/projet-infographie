#include "CursorManager.h"

CursorManager::CursorManager() : state(CursorState::Default) {}

void CursorManager::setup() {
    bool loaded = true;
    loaded &= imgDefault.load("cursors/cursor_default.png");
    loaded &= imgHoverButton.load("cursors/cursor_hand.png");
    loaded &= imgWaiting.load("cursors/cursor_crosshair.png");
    loaded &= imgDrawPoint.load("cursors/cursor_plus.png");
    loaded &= imgDrawPrimitive.load("cursors/cursor_brush.png");
    if(!loaded) {
        ofLogError("CursorManager") << "Erreur de chargement d'images de curseurs";
    }
}

void CursorManager::draw() {
    ofVec2f mousePos(ofGetMouseX(), ofGetMouseY());
    const float fixedWidth = 32.0f;
    const float fixedHeight = 32.0f;

    switch(state) {
        case CursorState::Default: {
            if (imgDefault.isAllocated()) {
                imgDefault.draw(mousePos.x, mousePos.y, fixedWidth, fixedHeight);
            } else {
                ofSetColor(255, 0, 0);
                ofDrawCircle(mousePos, 10);
            }
            break;
        }
        case CursorState::HoverButton: {
            if (imgHoverButton.isAllocated()) {
                imgHoverButton.draw(mousePos.x - fixedWidth/2, mousePos.y, fixedWidth, fixedHeight);
            } else {
                ofSetColor(0, 255, 0);
                ofDrawCircle(mousePos, 10);
            }
            break;
        }
        case CursorState::WaitingSecondClick: {
            if (imgWaiting.isAllocated()) {
                imgWaiting.draw(mousePos.x - fixedWidth/2, mousePos.y - fixedWidth/2, fixedWidth, fixedHeight);
            } else {
                ofSetColor(0, 0, 255);
                ofDrawCircle(mousePos, 10);
            }
            break;
        }
        case CursorState::DrawPoint: {
            if (imgDrawPoint.isAllocated()) {
                imgDrawPoint.draw(mousePos.x - fixedWidth/2, mousePos.y - fixedWidth/2, fixedWidth, fixedHeight);
            } else {
                ofSetColor(255, 255, 0);
                ofDrawCircle(mousePos, 10);
            }
            break;
        }
        case CursorState::DrawPrimitive: {
            if (imgDrawPrimitive.isAllocated()) {
                imgDrawPrimitive.draw(mousePos.x, mousePos.y, fixedWidth, fixedHeight);
            } else {
                ofSetColor(255, 0, 255);
                ofDrawCircle(mousePos, 10);
            }
            break;
        }
    }
}

void CursorManager::setState(CursorState newState) {
    state = newState;
}

CursorState CursorManager::getState() const {
    return state;
}
