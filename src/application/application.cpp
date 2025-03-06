#include "application.h"
#include "GeometryScene.h"
#include "imgui.h"
#include "ofAppRunner.h"


#include <Macros.h>

void Application::setup()
{
    ofSetWindowTitle("Projet Infographie - 3D Editor");

    ofHideCursor();
    cursorManager.setup();

    ofLog() << "<app::setup>";

    gui.setup(nullptr, true, ImGuiConfigFlags_DockingEnable, false);
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;


    renderer.Setup();

    this->geometryScene = std::make_shared<GeometryScene>();
    this->geometryScene->setup();
    renderer.scenes.AddScene(this->geometryScene);

    this->scene3D = std::make_shared<Scene3D>();
    this->scene3D->setup();
    renderer.scenes.AddScene(this->scene3D);

    this->importExportImageScene = std::make_shared<ImportImageScene>();
    this->importExportImageScene->setup();
    renderer.scenes.AddScene(this->importExportImageScene);

    this->rotatingCubeScene = std::make_shared<RotatingCubeScene>();
    this->rotatingCubeScene->setup();
    renderer.scenes.AddScene(this->rotatingCubeScene);

    this->primitiveScene = std::make_shared<PrimitiveScene>();
    this->primitiveScene->setup();
    renderer.scenes.AddScene(this->primitiveScene);

    this->imageHistogramScene = std::make_shared<ImageHistogramScene>();
    this->imageHistogramScene->setup();
    renderer.scenes.AddScene(this->imageHistogramScene);

    this->SelectScene(scene3D->getId());
}

void Application::update()
{
    renderer.Update();
}

void Application::draw()
{
    CaptureManager();
    gui.begin();
    ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);
    this->ShowMainMenuBar();
    this->ShowCaptureOption();
    renderer.Draw();

    if (ImGui::GetIO().WantCaptureMouse)
    {
        cursorManager.setState(CursorState::HoverButton);
    }
    else
    {
        auto currentScene = renderer.scenes.GetSelectedScene();
        if (currentScene)
        {
            auto primitiveScene = dynamic_cast<PrimitiveScene*>(currentScene.get());
            if (primitiveScene)
            {
                if (primitiveScene->getIsWaitingForSecondClick())
                {
                    cursorManager.setState(CursorState::WaitingSecondClick);
                }
                else if (primitiveScene->getIsAdding())
                {
                    if (primitiveScene->getSelectedType() == PrimitiveType::Point)
                    {
                        cursorManager.setState(CursorState::DrawPoint);
                    }
                    else
                    {
                        cursorManager.setState(CursorState::DrawPrimitive);
                    }
                }
                else
                {
                    cursorManager.setState(CursorState::Default);
                }
            }
            else
            {
                cursorManager.setState(CursorState::Default);
            }
        }
    }

    cursorManager.drawForeground();

    gui.end();
}

void Application::keyPressed(int key)
{
    ofLog() << "<app::keyPressed: " << key << ">";
    try
    {
        auto selectedScene = renderer.scenes.GetSelectedScene();
        if (selectedScene)
        {
            selectedScene->keyPressed(key);
        }
    } catch (std::exception& e)
    {
        ofLog() << "Aucune scène sélectionnée pour keyPressed : " << e.what();
    }
}

void Application::dragEvent(ofDragInfo dragInfo)
{

    ofLog() << "<app::dragged: >";
    ofLog() << "<app::ofDragInfo file[0]: " << dragInfo.files.at(0)
            << " at : " << dragInfo.position << ">";
    try
    {
        auto selectedScene = renderer.scenes.GetSelectedScene();
        if (selectedScene)
        {
            selectedScene->dragEvent(dragInfo);
        }
    } catch (std::exception& e)
    {
        ofLog() << "Aucune scène sélectionnée pour keyReleased : " << e.what();
    }
}

void Application::keyReleased(int key)
{
    ofLog() << "<app::keyReleased: " << key << ">";
    try
    {
        auto selectedScene = renderer.scenes.GetSelectedScene();
        if (selectedScene)
        {
            selectedScene->keyReleased(key);
        }
    } catch (std::exception& e)
    {
        ofLog() << "Aucune scène sélectionnée pour keyReleased : " << e.what();
    }
}

void Application::mouseReleased(int x, int y, int button)
{
    ofLog() << "<app::mouse released at: (" << x << ", " << y << ")>";
}

void Application::mousePressed(int x, int y, int button)
{
    try
    {
        auto selectedScene = renderer.scenes.GetSelectedScene();
        if (selectedScene)
        {
            selectedScene->mousePressed(x, y, button);
        }
    } catch (std::exception& e)
    {
        ofLog() << "Aucune scène sélectionnée pour mousePressed : " << e.what();
    }
}

void Application::mouseDragged(int x, int y, int button)
{
    try
    {
        auto selectedScene = renderer.scenes.GetSelectedScene();
        if (selectedScene)
        {
            selectedScene->mouseDragged(x, y, button);
        }
    } catch (std::exception& e)
    {
        ofLog() << "Aucune scène sélectionnée pour mouseDragged : " << e.what();
    }
}

void Application::exit()
{
    ofLog() << "<app::exit>";
}

void Application::SelectScene(SceneId id)
{
    renderer.scenes.SelectScene(id);
    this->selectedScene = renderer.scenes.GetSelectedScene();
}

void Application::ShowMainMenuBar()
{
    if (ImGui::BeginMainMenuBar())
    {
        std::shared_ptr<Scene> scene = this->selectedScene.lock();
        std::string sceneName = scene ? scene->GetName() : "No scene selected";
        if (ImGui::BeginMenu(("Scene: " + sceneName).c_str()))
        {
            for (const auto& scene: renderer.scenes.GetScenes())
            {
                SceneId id = scene.first;
                std::string name = scene.second->GetName();
                if (ImGui::MenuItem(name.c_str()))
                {
                    this->SelectScene(id);
                }
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Capture Options"))
        {
            if (ImGui::Button("Export screen as image"))
            {
                this->ExportImageButtonPressed();
            }
            if (ImGui::Button("Custom Capture Settings"))
            {
                showPopup = true;
            }
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
}

void Application::ExportImage(const std::string& path)
{
    GLCall(glReadBuffer(GL_FRONT));
    ofSaveScreen(path);
}

void Application::ExportImageButtonPressed()
{
    // there is a bug in the current version of openFrameworks that causes the parameters to not be used
    if (const ofFileDialogResult result = ofSystemSaveDialog("test.png", "Export image"); result.bSuccess)
    {
        ofLog() << "Successfully exported image: " << result.filePath;
        this->ExportImage(result.filePath);
    }
}

void Application::ShowCaptureOption()
{

    if (showPopup)
    {
        ImGui::OpenPopup("Set Capture Settings");
    }

    if (ImGui::BeginPopupModal("Set Capture Settings", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::InputInt("Number of Screenshots", &userScreenshots);
        ImGui::InputFloat("Capture Duration (sec)", &userDuration);

        if (ImGui::Button("Start Screenshot Capture"))
        {
            StartTimedCapture(userScreenshots, userDuration);
            showPopup = false;
            ImGui::CloseCurrentPopup();
        }

        if (ImGui::Button("Cancel"))
        {
            showPopup = false;
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

void Application::CaptureManager()
{
    if (capturing)
    {
        if (ofGetElapsedTimef() - lastCaptureTime > captureInterval)
        {
            if (screenshotCount < totalScreenshots)
            {
                std::string filename = exportPath + "_" + std::to_string(screenshotCount) + ".png";
                this->ExportImage(filename);
                lastCaptureTime = ofGetElapsedTimef();
                screenshotCount++;
            }
            else
            {
                capturing = false;
                ofLog() << "Finished capturing screenshots";
            }
        }
    }
}
void Application::StartTimedCapture(int numShots, float duration)
{
    ofFileDialogResult result = ofSystemSaveDialog("TimedCapture.png", "Choose Save Location");

    if (!result.bSuccess)
    {
        ofLog() << "Screenshot save canceled.";
        return;
    }
    else
    {
        exportPath = result.filePath;
    }

    capturing = true;
    screenshotCount = 0;
    lastCaptureTime = ofGetElapsedTimef();
    totalScreenshots = numShots;
    captureInterval = duration / numShots;
    ofLog() << "Started capturing " << numShots << " screenshots over " << duration << " sec.";
}
