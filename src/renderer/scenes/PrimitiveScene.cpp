#include "PrimitiveScene.h"
#include <cmath>

void PrimitiveScene::drawThickLine(const ofVec2f & p1, const ofVec2f & p2, float thickness, ofColor color) {
    ofPushStyle();
    ofSetColor(color);
    ofVec2f dir = p2 - p1;
    float length = dir.length();
    if(length == 0) {
        ofPopStyle();
        return;
    }
    dir.normalize();
    ofVec2f perp(-dir.y, dir.x);
    perp *= thickness / 2.0f;
    ofVec2f v1 = p1 + perp;
    ofVec2f v2 = p1 - perp;
    ofVec2f v3 = p2 - perp;
    ofVec2f v4 = p2 + perp;
    ofFill();
    ofDrawTriangle(v1, v2, v3);
    ofDrawTriangle(v1, v3, v4);
    ofPopStyle();
}

void PrimitiveScene::drawThickPolygon(const std::vector<ofVec2f>& vertices, float thickness, ofColor color) {
    if(vertices.size() < 2) return;
    for (size_t i = 0; i < vertices.size(); i++) {
        const ofVec2f& p1 = vertices[i];
        const ofVec2f& p2 = vertices[(i+1) % vertices.size()];
        drawThickLine(p1, p2, thickness, color);
    }
}

void PrimitiveScene::draw() {
    ofColor bgColor(backgroundColor[0]*255, backgroundColor[1]*255, backgroundColor[2]*255, backgroundColor[3]*255);
    ofClear(bgColor);

    for (const auto& prim : primitives) {
        switch (prim.type) {
            case PrimitiveType::Point: {
                if (!prim.points.empty()) {
                    ofFill();
                    ofSetColor(prim.fillColor[0]*255, prim.fillColor[1]*255, prim.fillColor[2]*255, prim.fillColor[3]*255);
                    ofDrawCircle(prim.points[0], 3);
                    const int circleResolution = 50;
                    std::vector<ofVec2f> circleVertices;
                    for (int i = 0; i < circleResolution; i++) {
                        float angle = TWO_PI * i / circleResolution;
                        circleVertices.push_back(ofVec2f(prim.points[0].x + 3 * cos(angle),
                                                         prim.points[0].y + 3 * sin(angle)));
                    }
                    ofColor strokeCol(prim.strokeColor[0]*255, prim.strokeColor[1]*255, prim.strokeColor[2]*255, prim.strokeColor[3]*255);
                    drawThickPolygon(circleVertices, prim.strokeThickness, strokeCol);
                }
                break;
            }
            case PrimitiveType::Ligne: {
                if (prim.points.size() >= 2) {
                    ofColor lineColor(prim.strokeColor[0]*255, prim.strokeColor[1]*255, prim.strokeColor[2]*255, prim.strokeColor[3]*255);
                    drawThickLine(prim.points[0], prim.points[1], prim.strokeThickness, lineColor);
                }
                break;
            }
            case PrimitiveType::Carre: {
                if (!prim.points.empty()) {
                    float side = 100.0f;
                    ofVec2f center = prim.points[0];
                    float half = side / 2.0f;
                    ofFill();
                    ofSetColor(prim.fillColor[0]*255, prim.fillColor[1]*255, prim.fillColor[2]*255, prim.fillColor[3]*255);
                    ofDrawRectangle(center.x - half, center.y - half, side, side);
                    ofVec2f tl(center.x - half, center.y - half);
                    ofVec2f tr(center.x + half, center.y - half);
                    ofVec2f br(center.x + half, center.y + half);
                    ofVec2f bl(center.x - half, center.y + half);
                    ofColor strokeCol(prim.strokeColor[0]*255, prim.strokeColor[1]*255, prim.strokeColor[2]*255, prim.strokeColor[3]*255);
                    drawThickLine(tl, tr, prim.strokeThickness, strokeCol);
                    drawThickLine(tr, br, prim.strokeThickness, strokeCol);
                    drawThickLine(br, bl, prim.strokeThickness, strokeCol);
                    drawThickLine(bl, tl, prim.strokeThickness, strokeCol);
                }
                break;
            }
            case PrimitiveType::Rectangle: {
                if (!prim.points.empty()) {
                    float width = 150.0f, height = 100.0f;
                    ofVec2f center = prim.points[0];
                    ofFill();
                    ofSetColor(prim.fillColor[0]*255, prim.fillColor[1]*255, prim.fillColor[2]*255, prim.fillColor[3]*255);
                    ofDrawRectangle(center.x - width/2, center.y - height/2, width, height);
                    ofVec2f tl(center.x - width/2, center.y - height/2);
                    ofVec2f tr(center.x + width/2, center.y - height/2);
                    ofVec2f br(center.x + width/2, center.y + height/2);
                    ofVec2f bl(center.x - width/2, center.y + height/2);
                    ofColor strokeCol(prim.strokeColor[0]*255, prim.strokeColor[1]*255, prim.strokeColor[2]*255, prim.strokeColor[3]*255);
                    drawThickLine(tl, tr, prim.strokeThickness, strokeCol);
                    drawThickLine(tr, br, prim.strokeThickness, strokeCol);
                    drawThickLine(br, bl, prim.strokeThickness, strokeCol);
                    drawThickLine(bl, tl, prim.strokeThickness, strokeCol);
                }
                break;
            }
            case PrimitiveType::Triangle: {
                if (!prim.points.empty()) {
                    float side = 100.0f;
                    ofVec2f center = prim.points[0];
                    float height = side * (std::sqrt(3.0f) / 2.0f);
                    ofVec2f p1(center.x, center.y - 2.0f/3.0f * height);
                    ofVec2f p2(center.x - side/2, center.y + height/3);
                    ofVec2f p3(center.x + side/2, center.y + height/3);
                    ofFill();
                    ofSetColor(prim.fillColor[0]*255, prim.fillColor[1]*255, prim.fillColor[2]*255, prim.fillColor[3]*255);
                    ofDrawTriangle(p1, p2, p3);
                    ofColor strokeCol(prim.strokeColor[0]*255, prim.strokeColor[1]*255, prim.strokeColor[2]*255, prim.strokeColor[3]*255);
                    drawThickLine(p1, p2, prim.strokeThickness, strokeCol);
                    drawThickLine(p2, p3, prim.strokeThickness, strokeCol);
                    drawThickLine(p3, p1, prim.strokeThickness, strokeCol);
                }
                break;
            }
        }
    }

    if (waitingForLineSecondClick) {
        ofSetColor(255, 255, 0);
        ofDrawCircle(pendingLineStart, 5);
    }

    ImGui::Begin("Toolbox");

    if (ImGui::Button("Dot")) {
        selectedType = PrimitiveType::Point;
        adding = true;
        waitingForLineSecondClick = false;
    }
    ImGui::SameLine();
    if (ImGui::Button("Line")) {
        selectedType = PrimitiveType::Ligne;
        adding = true;
        waitingForLineSecondClick = false;
    }
    ImGui::SameLine();
    if (ImGui::Button("Square")) {
        selectedType = PrimitiveType::Carre;
        adding = true;
        waitingForLineSecondClick = false;
    }
    ImGui::SameLine();
    if (ImGui::Button("Rectangle")) {
        selectedType = PrimitiveType::Rectangle;
        adding = true;
        waitingForLineSecondClick = false;
    }
    ImGui::SameLine();
    if (ImGui::Button("Triangle")) {
        selectedType = PrimitiveType::Triangle;
        adding = true;
        waitingForLineSecondClick = false;
    }
    ImGui::Checkbox("Mode HSB", &modeHSBActivated);
    ImGui::Separator();
    if (!modeHSBActivated)
    {
        ImGui::Text("Drawing tools RGBA:");
        ImGui::SliderFloat("Border thickness", &currentStrokeThickness, 1.0f, 10.0f);
        ImGui::ColorEdit4("Border color", currentStrokeColor);
        ImGui::ColorEdit4("Fill color", currentFillColor);
        ImGui::ColorEdit4("Background color", backgroundColor);
    }
    else
    {
        ofColor selectedStrokeColor(currentStrokeColor[0] * 255, currentStrokeColor[1] * 255, currentStrokeColor[2] * 255, currentStrokeColor[3] * 255);
        ofColor selectedFillColor(currentFillColor[0] * 255, currentFillColor[1] * 255, currentFillColor[2] * 255, currentFillColor[3] * 255);
        ofColor selectedBGColor(backgroundColor[0] * 255, backgroundColor[1] * 255, backgroundColor[2] * 255, backgroundColor[3] * 255);

        ImVec4 strokeColorVec = ImVec4(currentStrokeColor[0],
                                       currentStrokeColor[1],
                                       currentStrokeColor[2],
                                       currentStrokeColor[3]);

        ImVec4 fillColorVec = ImVec4(currentFillColor[0],
                                     currentFillColor[1],
                                     currentFillColor[2],
                                     currentFillColor[3]);

        ImVec4 bgColorVec = ImVec4(backgroundColor[0],
                                   backgroundColor[1],
                                   backgroundColor[2],
                                   backgroundColor[3]);

        ImGui::Text("Drawing tools HSB:");
        ImGui::SliderFloat("Border thickness", &currentStrokeThickness, 1.0f, 10.0f);

        hueContour = selectedStrokeColor.getHue();
        satContour = selectedStrokeColor.getSaturation();
        briContour = selectedStrokeColor.getBrightness();
        ImGui::Text("Border color");
        ImGui::ColorButton("Preview##Contour", strokeColorVec, ImGuiColorEditFlags_NoPicker, ImVec2(50, 50));
        ImGui::SliderFloat("Hue##Contour", &hueContour, 0.0f, 255.0f);
        ImGui::SliderFloat("Saturation##Contour", &satContour, 0.0f, 255.0f);
        ImGui::SliderFloat("Brightness##Contour", &briContour, 0.0f, 255.0f);
        selectedStrokeColor.setHsb(hueContour, satContour, briContour);
        colorUpdate(currentStrokeColor, selectedStrokeColor);


        hueFill = selectedFillColor.getHue();
        satFill = selectedFillColor.getSaturation();
        briFill = selectedFillColor.getBrightness();
        ImGui::Text("Fill color");
        ImGui::ColorButton("Preview##Remplissage", fillColorVec, ImGuiColorEditFlags_NoPicker, ImVec2(50, 50));
        ImGui::SliderFloat("Hue##Remplissage", &hueFill, 0.0f, 255.0f);
        ImGui::SliderFloat("Saturation##Remplissage", &satFill, 0.0f, 255.0f);
        ImGui::SliderFloat("Brightness##Remplissage", &briFill, 0.0f, 255.0f);
        selectedFillColor.setHsb(hueFill, satFill, briFill);
        colorUpdate(currentFillColor, selectedFillColor);

        hueBG = selectedBGColor.getHue();
        satBG = selectedBGColor.getSaturation();
        briBG = selectedBGColor.getBrightness();
        ImGui::Text("Background color");
        ImGui::ColorButton("Preview##BG", bgColorVec, ImGuiColorEditFlags_NoPicker, ImVec2(50, 50));
        ImGui::SliderFloat("Hue##BG", &hueBG, 0.0f, 255.0f);
        ImGui::SliderFloat("Saturation##BG", &satBG, 0.0f, 255.0f);
        ImGui::SliderFloat("Brightness##BG", &briBG, 0.0f, 255.0f);
        selectedBGColor.setHsb(hueBG, satBG, briBG);
        colorUpdate(backgroundColor, selectedBGColor);
    }
    ImGui::Separator();

    if (adding) {
        if (selectedType == PrimitiveType::Ligne) {
            if (!waitingForLineSecondClick) {
                ImGui::Text("Click to define starting point of the line");
            } else {
                ImGui::Text("Click to define ending point of the line");
            }
        } else {
            ImGui::Text("Click on the scene to add primitive");
        }
    }

    if (ImGui::Button("Undo")) {
        undo();
    }
    ImGui::SameLine();

    if (ImGui::Button("Redo")) {
        redo();
    }

    ImGui::End();
}

void PrimitiveScene::mousePressed(int x, int y, int button) {
    if (!adding)
        return;

    if (selectedType == PrimitiveType::Ligne) {
        if (!waitingForLineSecondClick) {
            pendingLineStart = ofVec2f(x, y);
            waitingForLineSecondClick = true;
            tempStrokeThickness = currentStrokeThickness;
            for (int i = 0; i < 4; i++) {
                tempStrokeColor[i] = currentStrokeColor[i];
                tempFillColor[i] = currentFillColor[i];
            }
        } else {
            Primitive prim;
            prim.type = PrimitiveType::Ligne;
            prim.points.push_back(pendingLineStart);
            prim.points.push_back(ofVec2f(x, y));
            prim.strokeThickness = tempStrokeThickness;
            for (int i = 0; i < 4; i++) {
                prim.strokeColor[i] = tempStrokeColor[i];
                prim.fillColor[i] = tempFillColor[i];
            }
            primitives.push_back(prim);
            waitingForLineSecondClick = false;
            adding = false;
        }
    } else {
        Primitive prim;
        prim.type = selectedType;
        prim.points.push_back(ofVec2f(x, y));
        prim.strokeThickness = currentStrokeThickness;
        for (int i = 0; i < 4; i++) {
            prim.strokeColor[i] = currentStrokeColor[i];
            prim.fillColor[i] = currentFillColor[i];
        }
        primitives.push_back(prim);
        adding = false;
    }
}

void PrimitiveScene::undo() {
    if (!primitives.empty()) {
        redoStack.push_back(primitives.back());
        primitives.pop_back();
        waitingForLineSecondClick = false;
    }
}

void PrimitiveScene::redo() {
    if (!redoStack.empty()) {
        primitives.push_back(redoStack.back());
        redoStack.pop_back();
    }
}

void PrimitiveScene::colorUpdate(float currentColor[4], ofColor hsbColor){
    currentColor[0] = hsbColor.r / 255.0f;
    currentColor[1] = hsbColor.g / 255.0f;
    currentColor[2] = hsbColor.b / 255.0f;
    currentColor[3] = hsbColor.a / 255.0f;
}
