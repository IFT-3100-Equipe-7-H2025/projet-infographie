#pragma once
#include "Scene.h"
#include "imgui.h"
#include "ofMain.h"
#include "ofPoint.h"
#include <vector>

enum class PrimitiveType
{
    Point,
    Ligne,
    Carre,
    Rectangle,
    Triangle,
    Delauney,
    P1,
    P2,
    Midpoints
};

struct Primitive
{
    PrimitiveType type;
    std::vector<ofVec2f> points;
    float strokeThickness;
    float strokeColor[4];
    float fillColor[4];
};


class PrimitiveScene : public Scene
{
public:
    void draw() override;
    void mousePressed(int x, int y, int button) override;

    std::string GetName() override { return "Primitive Scene"; }

    bool getIsWaitingForSecondClick() const { return waitingForLineSecondClick; }
    bool getIsAdding() const { return adding; }
    PrimitiveType getSelectedType() const { return selectedType; }

    void undo();
    void redo();

private:
    std::vector<Primitive> primitives;
    std::vector<Primitive> redoStack;
    std::vector<ofPoint> delauneyPoints;
    std::optional<ofPoint> p1;
    std::optional<ofPoint> p2;
    std::vector<ofPoint> midpoints;
    bool imguiClick = false;

    bool adding = false;
    PrimitiveType selectedType = PrimitiveType::Point;

    bool waitingForLineSecondClick = false;
    ofVec2f pendingLineStart;
    float tempStrokeThickness = 0.0f;
    float tempStrokeColor[4] = {0.0f, 0.0f, 0.0f, 1.0f};
    float tempFillColor[4] = {1.0f, 1.0f, 1.0f, 1.0f};

    float currentStrokeThickness = 2.0f;
    float currentStrokeColor[4] = {0.0f, 0.0f, 0.0f, 1.0f};
    float currentFillColor[4] = {1.0f, 1.0f, 1.0f, 1.0f};
    float backgroundColor[4] = {0.1f, 0.1f, 0.1f, 1.0f};

    bool modeHSBActivated = false;

    float hueContour = 0.0f;
    float satContour = 0.0f;
    float briContour = 0.0f;
    float hueFill = 0.0f;
    float satFill = 0.0f;
    float briFill = 0.0f;
    float hueBG = 0.0f;
    float satBG = 0.0f;
    float briBG = 0.0f;

    static void drawThickLine(const ofVec2f& p1, const ofVec2f& p2, float thickness, ofColor color);

    static void drawThickPolygon(const std::vector<ofVec2f>& vertices, float thickness, ofColor color);

    static void colorUpdate(float currentColor[4], ofColor hsbColor);

    static ofPoint catmull_rom(const ofPoint& p0, const ofPoint& p1, const ofPoint& p2, const ofPoint& p3, float t);

    static std::vector<ofPoint> generate_catmull_rom_spline(const std::vector<ofPoint>& points, int samples_per_segment);
};
