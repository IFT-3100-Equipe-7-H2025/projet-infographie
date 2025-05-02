#include "ImportImageScene.h"

#include "imgui.h"
#include "ofLog.h"


void ImportImageScene::setup()
{
    ofDisableArbTex();
    ofEnableTextureEdgeHack();

    toneMapingshader.load("shaders/tone_mapping_vs.glsl", "shaders/tone_mapping_fs.glsl");
    sharpeningShader.load("shaders/filter.vert", "shaders/sharpening_filter.frag");
    toonShader.load("shaders/filter.vert", "shaders/toonShading_filter.frag");
    edgeDetectShader.load("shaders/filter.vert", "shaders/edgeDetection_filter.frag");
    negateShader.load("shaders/filter.vert", "shaders/negate_filter.frag");

    shaderLabels = {"Default", "Tone Mapping", "Sharpen", "Toon", "Edge Detect", "Negate"};
    shaderIDs = {0, 1, 2, 3, 4, 5};
}

void ImportImageScene::draw()
{
    ImGui::SetNextWindowPos(ImVec2(10, 30), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(300, 600), ImGuiCond_FirstUseEver);
    ImGui::Begin(this->GetName().c_str());
    if (ImGui::Button("Import image")) { this->ImportImageButtonPressed(); }
    if (ImGui::Button("Sample image")) { this->SampleImage(); }
    if (ImGui::Button("Generate new images")) { this->GenerateNewImages(); }

    const char* preview = shaderLabels[currentIndex].c_str();
    if (ImGui::BeginCombo("Shader Type", preview))
    {
        for (int i = 0; i < shaderLabels.size(); ++i)
        {
            bool isSelected = (i == currentIndex);
            if (ImGui::Selectable(shaderLabels[i].c_str(), isSelected))
            {
                currentIndex = i;
            }
            if (isSelected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }
   
    switch (currentIndex)
    {
        case 0:
            for (auto& [image, position]: images)
            {
                image.draw(position);
            }
            break;
        case 1:
            ImGui::SliderFloat("Exposure", &exposure, 0.0f, 5.0f);
            ImGui::SliderFloat("Gamma", &gamma, 0.0f, 5.0f);
            ImGui::SliderFloat("Contrast", &contrast, 0.0f, 5.0f);
            ImGui::SliderFloat("Lift", &lift, 0.0f, 1.0f);
            ImGui::SliderFloat("Flatten", &flatten, 0.0f, 5.0f);
            ImGui::SliderFloat("Compress", &compress, 0.0f, 1.0f);
            ImGui::SliderFloat("Clamp", &clamp, 0.0f, 1.0f);

            if (ImGui::Button("Reset Tone Mapping")) { ResetToneMapping(); }
            break;
        case 2:
            ImGui::SliderFloat("Sharpness", &sharpness, 1.0f, 10.0f);
            break;
        case 3:
            ImGui::SliderInt("Toon Steps", &toonSteps, 2, 10);

            break;
        case 4:
            ImGui::SliderFloat("Edge Threshold", &threshold, 0.0f, 1.0f);

            break;
        case 5:
            ImGui::SliderFloat("Negate Amount", &negateAmount, 0.0f, 1.0f);
            break;
        default:
            ofLogError() << "Invalid shader ID: " << currentIndex;
            break;
    }
    if (currentIndex !=0) {
        for (auto& [image, position]: images)
        {
            executeShader(currentIndex, image, position);
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

void ImportImageScene::executeShader(int shaderId, ofImage image, ofVec2f position)
{
    if (shaderId == 1)
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
    else if (shaderId == 2)
    {
        ofTexture& tex = image.getTexture();
        glm::vec2 texelSize = glm::vec2(1.0f / tex.getWidth(), 1.0f / tex.getHeight());

        if (sharpeningShader.isLoaded() == false)
        {
            ofLogError() << "Sharpening shader failed to load!";
            return;
        }

        sharpeningShader.begin();
        sharpeningShader.setUniformTexture("image", image.getTexture(), 1);
        sharpeningShader.setUniform2f("u_texelSize", texelSize);
        sharpeningShader.setUniform1f("sharpness", sharpness);
        image.draw(position);

        sharpeningShader.end();
    }
    else if (shaderId == 3)
    {
        toonShader.begin();
        toonShader.setUniformTexture("image", image.getTexture(), 1);
        toonShader.setUniform1i("toonSteps", toonSteps);
        image.draw(position);
        toonShader.end();
    }
    else if (shaderId == 4)
    {
        ofTexture& tex = image.getTexture();
        glm::vec2 texelSize = glm::vec2(1.0f / tex.getWidth(), 1.0f / tex.getHeight());

        edgeDetectShader.begin();
        edgeDetectShader.setUniformTexture("image", image.getTexture(), 1);
        edgeDetectShader.setUniform2f("u_texelSize", texelSize);
        edgeDetectShader.setUniform1f("threshold", threshold);
        image.draw(position);
        edgeDetectShader.end();
    }
    else if (shaderId == 5)
    {
        negateShader.begin();
        negateShader.setUniformTexture("image", image.getTexture(), 1);
        negateShader.setUniform1f("negateAmount", negateAmount);
        image.draw(position);
        negateShader.end();
    }
}
