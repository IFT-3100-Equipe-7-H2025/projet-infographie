#pragma once
#include "Scene.h"
#include "ofMain.h"

struct Image
{
    ofImage image;
    ofVec2f position;
};


class ImportImageScene : public Scene
{
public:
    void draw() override;

    void ImportImage(const std::string& path);
    void ImportImageButtonPressed();

    std::string GetName() override { return "Image importing"; }

private:
    std::vector<Image> images{};
};
