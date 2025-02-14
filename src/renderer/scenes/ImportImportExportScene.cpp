#include "ImportImportExportScene.h"

#include "Macros.h"
#include "imgui.h"
#include "ofLog.h"


void ImportImportExportScene::draw()
{
    ImGui::Begin(this->GetName().c_str());
    if (ImGui::Button("Import image")) { this->ImportImageButtonPressed(); }
    if (ImGui::Button("Export image")) { this->ExportImageButtonPressed(); }
    ImGui::End();

    for (auto& [image, position]: images)
    {
        image.draw(position);
    }
}

void ImportImportExportScene::ImportImageButtonPressed()
{
    if (const ofFileDialogResult result = ofSystemLoadDialog("Import image", false, "."); result.bSuccess)
    {
        ofLog() << "Successfully imported image: " << result.filePath;

        this->ImportImage(result.filePath);
    }
}

void ImportImportExportScene::ImportImage(const std::string& path)
{
    Image image;
    image.image.load(path);
    // place image in center of window
    image.position = ofVec2f((static_cast<float>(ofGetWidth()) / 2.0f) - image.image.getWidth() / 2.0f,
                             (static_cast<float>(ofGetHeight()) / 2.0f) - image.image.getHeight() / 2.0f);
    this->images.push_back(image);
}


void ImportImportExportScene::ExportImage(const std::string& path)
{
    GLCall(glReadBuffer(GL_FRONT));
    ofSaveScreen(path);
}

void ImportImportExportScene::ExportImageButtonPressed()
{
    // there is a bug in the current version of openFrameworks that causes the parameters to not be used on Windows
    if (const ofFileDialogResult result = ofSystemSaveDialog("test.png", "Export image"); result.bSuccess)
    {
        ofLog() << "Successfully exported image: " << result.filePath;
        this->ExportImage(result.filePath);
    }
}
