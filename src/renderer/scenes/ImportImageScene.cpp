#include "ImportImageScene.h"

#include "imgui.h"
#include "ofLog.h"


void ImportImageScene::setup()
{
    ofDisableArbTex();
    ofEnableTextureEdgeHack();
    toneMapingshader.load("shaders/tone_mapping_vs.glsl", "shaders/tone_mapping_fs.glsl");

    if (!toneMapingshader.isLoaded())
    {
        ofLogError() << "toneMapingshader failed to load!";
    }
}

void ImportImageScene::draw()
{
    ImGui::SetNextWindowPos(ImVec2(10, 30), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(300, 600), ImGuiCond_FirstUseEver);
    ImGui::Begin(this->GetName().c_str());
    if (ImGui::Button("Import image")) { this->ImportImageButtonPressed(); }
    if (ImGui::Button("Sample image")) { this->SampleImage(); }
    if (ImGui::Button("Generate new images")) { this->GenerateNewImages(); }

    ImGui::Text("Tone mapping");
    ImGui::Checkbox("ACES Filmic", &toneMappingActive);
   

    if (toneMappingActive)
    {
        ImGui::SliderFloat("Exposure", &exposure, 0.0f, 5.0f);
        ImGui::SliderFloat("Gamma", &gamma, 0.0f, 5.0f);
        ImGui::SliderFloat("Contrast", &contrast, 0.0f, 5.0f);
        ImGui::SliderFloat("Lift", &lift, 0.0f, 1.0f);
        ImGui::SliderFloat("Flatten", &flatten, 0.0f, 5.0f);
        ImGui::SliderFloat("Compress", &compress, 0.0f, 1.0f);
        ImGui::SliderFloat("Clamp", &clamp, 0.0f, 1.0f);
        

        for (auto& [image, position]: images)
        {

            toneMapingshader.begin();
            toneMapingshader.setUniformTexture("image", image.getTexture(), 1);

            toneMapingshader.setUniform1f("tone_mapping_exposure", exposure);
            toneMapingshader.setUniform1f("tone_mapping_gamma", gamma);

            toneMapingshader.setUniform1f("tone_mapping_contrast", contrast);
            toneMapingshader.setUniform1f("tone_mapping_lifts", lift);
            toneMapingshader.setUniform1f("tone_mapping_flattens", flatten);
            toneMapingshader.setUniform1f("tone_mapping_compres", compress);
            toneMapingshader.setUniform1f("tone_mapping_clamp", clamp);


            image.draw(position);

            toneMapingshader.end();
        }
        if (ImGui::Button("Reset Tone Mapping")) {ResetToneMapping();}
    }
    else
    {
        
        for (auto& [image, position]: images)
        {
            image.draw(position);
        }  
    }
    float xOffset = 20;
    for (auto& img: generatedImages)
    {
        img.draw(xOffset, 20);
        xOffset += img.getWidth() + 10;
    }

    ImGui::End();
}

void ImportImageScene::ImportImageButtonPressed()
{
    if (const ofFileDialogResult result = ofSystemLoadDialog("Import image", false, "."); result.bSuccess)
    {
        ofLog() << "Successfully imported image: " << result.filePath;

        this->ImportImage(result.filePath);
    }
}

void ImportImageScene::ImportImage(const std::string& path)
{
    Image image;
    image.image.load(path);
    // place image in center of window
    image.position = ofVec2f((static_cast<float>(ofGetWidth()) / 2.0f) - image.image.getWidth() / 2.0f,
                             (static_cast<float>(ofGetHeight()) / 2.0f) - image.image.getHeight() / 2.0f);
    this->images.push_back(image);
}

void ImportImageScene::SampleImage()
{
    if (images.empty()) return;

    generatedImages.clear();
    int numParts = 3;
    for (auto& img: images)
    {
        if (img.image.getWidth() < numParts) return;

        int partWidth = img.image.getWidth() / numParts;
        int height = img.image.getHeight();

        for (int i = 0; i < numParts; i++)
        {
            ofImage part;
            part.allocate(partWidth, height, img.image.getPixels().getImageType());
            part.cropFrom(img.image, i * partWidth, 0, partWidth, height);
            generatedImages.push_back(part);
        }
    }

    images.clear();
}

void ImportImageScene::GenerateNewImages()
{
    if (generatedImages.empty()) return;

    float xOffset = 20;
    for (auto& img: generatedImages)
    {
        if (img.isAllocated())
        {
            img.draw(xOffset, 20);
            xOffset += img.getWidth() + 10;
        }
    }
}

void ImportImageScene::ResetToneMapping()
{
    toneMappingActive = true;
    exposure = 1.0f;
    gamma = 2.2f;
    contrast = 2.51f;
    lift = 0.03f;
    flatten = 2.43f;
    compress = 0.59f;
    clamp = 0.14f;
}
