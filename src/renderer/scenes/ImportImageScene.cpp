#include "ImportImageScene.h"

#include "imgui.h"
#include "ofLog.h"


void ImportImageScene::draw()
{
    ImGui::Begin(this->GetName().c_str());
    if (ImGui::Button("Import image")) { this->ImportImageButtonPressed(); }
    if (ImGui::Button("Sample image")) { this->SampleImage(); }
    if (ImGui::Button("Generate new images")) { this->GenerateNewImages(); }
    ImGui::End();

    for (auto& [image, position]: images)
    {
        image.draw(position);
    }

    float xOffset = 20;
    for (auto& img: generatedImages)
    {
        img.draw(xOffset, 20);
        xOffset += img.getWidth() + 10;
    }
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
