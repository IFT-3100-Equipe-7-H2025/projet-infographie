#pragma once
#include "Scene.h"
#include "ofMain.h"

struct Image
{
    ofImage image;
    ofVec2f position;
};


class ImportImportExportScene : public Scene
{
public:
    void draw() override;

    void ImportImage(const std::string& path);
    void ImportImageButtonPressed();

    void ExportImage(const std::string& path);
    void ExportImageButtonPressed();

    std::string GetName() override { return "Image importing and exporting"; }

private:
    std::vector<Image> images{};
};
