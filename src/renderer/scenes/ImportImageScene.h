#pragma once
#include "Scene.h"
#include "ofMain.h"

struct Image
{
    ofImage image;
    ofVec2f position;
};

struct SampleBlock
{
    ofImage block;
    ofVec2f position;
};

class ImportImageScene : public Scene
{
public:
    void draw() override;
    void setup() override;

    void ImportImage(const std::string& path);
    void ImportImageButtonPressed();
    void SampleImage();
    void GenerateNewImages();
    void ResetToneMapping();

    ofShader toneMapingshader;
    bool toneMappingActive = false;
    float exposure = 1.0f;
    float gamma = 2.2f;

    //test
    float contrast = 2.51f;
    float lift = 0.03f;
    float flatten = 2.43f;
    float compress = 0.59f;
    float clamp = 0.14f;
    //----------------------
    std::string GetName() override { return "Image importing"; }

private:
    std::vector<Image> images{};
    std::vector<ofImage> generatedImages;
};
