#pragma once

#include "Scene.h"
#include "ofImage.h"
#include "ofTrueTypeFont.h"
#include "ofVec2f.h"


struct ImageWithName
{
    ofImage image;
    ofVec2f position;
    std::string filename;
};

enum Color
{
    RED,
    GREEN,
    BLUE
};

class ImageHistogramScene : public Scene
{
public:
    void setup() override;
    void draw() override;

    void ImportImage(const std::string& path);
    void ImportImageButtonPressed();
    void CalculateHistogram();

    void ShowUI();

    void ShowHistogram();

    std::string GetName() override { return "Image histogram"; }

private:
    // Image, add in histogram or not
    std::vector<std::pair<ImageWithName, bool>> images;

    std::unordered_map<Color, int[256]> histogram;

    bool showHistogram = true;
    bool showRed = true;
    bool showGreen = true;
    bool showBlue = true;

    ofTrueTypeFont font;
};
