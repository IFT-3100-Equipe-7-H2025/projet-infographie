#include "CursorManager.h"
#include "ofMain.h"
#include "ofxImGui.h"

CursorManager::CursorManager() : state(CursorState::Default) {}

void CursorManager::setup() {
    ofDisableArbTex();

    bool loaded = true;
    loaded &= imgDefault.load("cursors/cursor_default.png");
    loaded &= imgHoverButton.load("cursors/cursor_hand.png");
    loaded &= imgWaiting.load("cursors/cursor_crosshair.png");
    loaded &= imgDrawPoint.load("cursors/cursor_plus.png");
    loaded &= imgDrawPrimitive.load("cursors/cursor_brush.png");
    if(!loaded) {
        ofLogError("CursorManager") << "Erreur de chargement d'images de curseurs";
    }

    ofEnableArbTex();
}

void CursorManager::draw() {}

void CursorManager::drawForeground() {
    ImDrawList* draw_list = ImGui::GetForegroundDrawList();
    ImVec2 mousePos = ImGui::GetIO().MousePos;
    const float fixedWidth = 32.0f;
    const float fixedHeight = 32.0f;

        switch(state) {
        case CursorState::Default: {
            if (imgDefault.isAllocated()) {
                draw_list->AddImage(
                    (ImTextureID)imgDefault.getTexture().getTextureData().textureID,
                    mousePos,
                    ImVec2(mousePos.x + fixedWidth, mousePos.y + fixedHeight)
                );
            } else {
                draw_list->AddCircle(mousePos, 10.0f, IM_COL32(255, 0, 0, 255));
            }
            break;
        }
        case CursorState::HoverButton: {
            if (imgHoverButton.isAllocated()) {
                ImVec2 pos(mousePos.x - fixedWidth / 2, mousePos.y);
                draw_list->AddImage(
                    (ImTextureID)imgHoverButton.getTexture().getTextureData().textureID,
                    pos,
                    ImVec2(pos.x + fixedWidth, pos.y + fixedHeight)
                );
            } else {
                draw_list->AddCircle(mousePos, 10.0f, IM_COL32(0, 255, 0, 255));
            }
            break;
        }
        case CursorState::WaitingSecondClick: {
            if (imgWaiting.isAllocated()) {
                ImVec2 pos(mousePos.x - fixedWidth / 2, mousePos.y - fixedWidth / 2);
                draw_list->AddImage(
                    (ImTextureID)imgWaiting.getTexture().getTextureData().textureID,
                    pos,
                    ImVec2(pos.x + fixedWidth, pos.y + fixedHeight)
                );
            } else {
                draw_list->AddCircle(mousePos, 10.0f, IM_COL32(0, 0, 255, 255));
            }
            break;
        }
        case CursorState::DrawPoint: {
            if (imgDrawPoint.isAllocated()) {
                ImVec2 pos(mousePos.x - fixedWidth / 2, mousePos.y - fixedWidth / 2);
                draw_list->AddImage(
                    (ImTextureID)imgDrawPoint.getTexture().getTextureData().textureID,
                    pos,
                    ImVec2(pos.x + fixedWidth, pos.y + fixedHeight)
                );
            } else {
                draw_list->AddCircle(mousePos, 10.0f, IM_COL32(255, 255, 0, 255));
            }
            break;
        }
        case CursorState::DrawPrimitive: {
            if (imgDrawPrimitive.isAllocated()) {
                draw_list->AddImage(
                    (ImTextureID)imgDrawPrimitive.getTexture().getTextureData().textureID,
                    mousePos,
                    ImVec2(mousePos.x + fixedWidth, mousePos.y + fixedHeight)
                );
            } else {
                draw_list->AddCircle(mousePos, 10.0f, IM_COL32(255, 0, 255, 255));
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
