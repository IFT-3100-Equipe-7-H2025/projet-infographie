#include "ImageHistogramScene.h"
#include "imgui.h"
#include "ofAppRunner.h"
#include "ofGraphics.h"
#include "ofGraphicsBaseTypes.h"
#include "ofPolyline.h"
#include "ofSystemUtils.h"

#include <ranges>

constexpr int FONT_SIZE = 20;

void ImageHistogramScene::setup()
{
    font.load("fonts/JetBrainsMono-Regular.ttf", FONT_SIZE, true, true);
}

void ImageHistogramScene::draw()
{
    this->ShowUI();

    ofClear(ofColor::gray);
    this->ShowHistogram();
}

void ImageHistogramScene::ShowUI()
{
    ImGui::SetNextWindowPos(ImVec2(10, 30), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(300, 400), ImGuiCond_FirstUseEver);
    ImGui::Begin(this->GetName().c_str());
    if (ImGui::Button("Import image"))
    {
        this->ImportImageButtonPressed();
        this->CalculateHistogram();
    }

    ImGui::Checkbox("Show red", &showRed);
    ImGui::SameLine();
    ImGui::Checkbox("Show green", &showGreen);
    ImGui::SameLine();
    ImGui::Checkbox("Show blue", &showBlue);

    ImGui::Text("Include in histogram:");
    for (auto& [image, include]: images)
    {
        ImGui::PushID(image.filename.c_str());
        if (ImGui::Checkbox("##include", &include))
        {
            if (this->showHistogram)
            {
                this->CalculateHistogram();
            }
        }
        ImGui::SameLine();
        ImGui::TextUnformatted(image.filename.c_str());
        ImGui::PopID();
    }

    ImGui::End();
}

void ImageHistogramScene::ImportImageButtonPressed()
{
    if (const ofFileDialogResult result = ofSystemLoadDialog("Import image", false, "."); result.bSuccess)
    {
        ofLog() << "Successfully imported image: " << result.filePath;

        this->ImportImage(result.filePath);
    }
}

void ImageHistogramScene::ImportImage(const std::string& path)
{
    std::string filename = ofFilePath::getFileName(path);
    for ( const auto& image: std::views::keys(images) )
    {
        if ( image.filename == filename )
        {
            ofLogError() << "There is already an image with this name imported. Aborting import.";
            return;
        }
    }

    ImageWithName image;
    image.image.load(path);
    image.position = ofVec2f((static_cast<float>(ofGetWidth()) / 2.0f) - image.image.getWidth() / 2.0f,
                             (static_cast<float>(ofGetHeight()) / 2.0f) - image.image.getHeight() / 2.0f);
    image.filename = filename;
    if (this->images.empty())
    {
        this->images.emplace_back(image, true);
    }
    else
    {
        this->images.emplace_back(image, false);
    }
}

void ImageHistogramScene::CalculateHistogram()
{
    this->histogram.clear();
    if (images.empty()) return;

    for (auto& [image, include]: images)
    {
        if (!include) continue;
        ofPixels& pixels = image.image.getPixels();
        for (size_t i = 0; i < pixels.size(); i += 3)
        {
            histogram[RED][pixels[i]]++;
            histogram[GREEN][pixels[i + 1]]++;
            histogram[BLUE][pixels[i + 2]]++;
        }
    }
}

void ImageHistogramScene::ShowHistogram()
{
    const float     offsetX = static_cast<float>(ofGetWidth()) / 10.0f;
    constexpr float offsetY = 50.0f;
    const float     regionWidth = static_cast<float>(ofGetWidth()) / 1.33f;
    const float     regionHeight = static_cast<float>(ofGetHeight()) / 2.0f;

    constexpr int numBins = 256;

    int maxValue = 0;
    for (int i = 0; i < numBins; i++)
    {
        maxValue = std::max({maxValue, histogram[RED][i], histogram[GREEN][i], histogram[BLUE][i]});
    }

    const float scaleY = (maxValue > 0) ? (regionHeight / static_cast<float>(maxValue)) : 1.0f;
    const float scaleX = regionWidth / static_cast<float>(numBins);

    ofPolyline red, green, blue;
    for (int i = 0; i < numBins; i++)
    {
        const float xPos = offsetX + static_cast<float>(i) * scaleX;

        const float yRed = offsetY + regionHeight - static_cast<float>(histogram[RED][i]) * scaleY;
        const float yGreen = offsetY + regionHeight - static_cast<float>(histogram[GREEN][i]) * scaleY;
        const float yBlue = offsetY + regionHeight - static_cast<float>(histogram[BLUE][i]) * scaleY;

        red.addVertex(xPos, yRed);
        green.addVertex(xPos, yGreen);
        blue.addVertex(xPos, yBlue);
    }

    ofPushStyle();
    ofSetLineWidth(2);

    ofSetColor(ofColor::black);

    font.drawString("0", offsetX, offsetY + regionHeight + 25);
    font.drawString("255", offsetX + regionWidth - 25, offsetY + regionHeight + 25);
    font.drawString("Intensity", offsetX + regionWidth / 2 - 25, offsetY + regionHeight + 50);
    ofDrawLine(offsetX, offsetY, offsetX, offsetY + regionHeight);

    font.drawString("0", offsetX - 25, offsetY + regionHeight);
    font.drawString(std::to_string(maxValue), offsetX - 10 - 15 * static_cast<float>(std::to_string(maxValue).size()), offsetY);
    font.drawString("Frequency", offsetX - 8 * FONT_SIZE, offsetY + regionHeight / 2);
    ofDrawLine(offsetX, offsetY + regionHeight, offsetX + regionWidth, offsetY + regionHeight);

    if (showRed)
    {
        ofSetColor(ofColor::red);
        red.draw();
    }
    if (showGreen)
    {
        ofSetColor(ofColor::green);
        green.draw();
    }
    if (showBlue)
    {
        ofSetColor(ofColor::blue);
        blue.draw();
    }
    ofPopStyle();

    // show included images below the histogram, resized to fit the window
    std::vector<ImageWithName*> shownImages;
    for ( auto& [image, include]: this->images ) { if ( include ) { shownImages.push_back(&image); } }

    float       imageOffsetX = offsetX;
    float       imageOffsetY = offsetY + regionHeight + 75;
    const float imageWidth = (regionWidth * 0.9f) / static_cast<float>(shownImages.size());

    const float screenWidth = static_cast<float>(ofGetWidth());
    const float screenHeight = static_cast<float>(ofGetHeight());

    for ( const auto image: shownImages )
    {
        const float aspectRatio = image->image.getWidth() / image->image.getHeight();
        float       resizedWidth = imageWidth;
        float       resizedHeight = imageWidth / aspectRatio;

        if ( imageOffsetY + resizedHeight > screenHeight )
        {
            resizedHeight = screenHeight - imageOffsetY - 15;
            resizedWidth = resizedHeight * aspectRatio;
        }

        if ( imageOffsetX + resizedWidth > screenWidth )
        {
            imageOffsetX = offsetX;
            imageOffsetY += resizedHeight + 15;
        }

        image->image.draw(imageOffsetX, imageOffsetY, resizedWidth, resizedHeight);
        imageOffsetX += resizedWidth;
    }
}
