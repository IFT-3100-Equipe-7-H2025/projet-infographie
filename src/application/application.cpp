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

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;

    static const ImWchar* normalCharRanges = ImGui::GetIO().Fonts->GetGlyphRangesDefault();
    static const ImWchar* myCharRanges = normalCharRanges;
    customFont = gui.addFont("fonts/Tahoma_Regular_font.ttf", fontSize, nullptr, myCharRanges, false);
    SetDarkishBlueTheme();
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

    ImGui::PushFont(customFont);//Change le font pour le reste de l'application
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
    ImGui::PopFont();//Remet le font par défaut avant de terminer
    gui.end();
}

void Application::keyPressed(int key)
{
    //ofLog() << "<app::keyPressed: " << key << ">";
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

    //ofLog() << "<app::dragged: >";
    //ofLog() << "<app::ofDragInfo file[0]: " << dragInfo.files.at(0)
            //<< " at : " << dragInfo.position << ">";
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
    //ofLog() << "<app::keyReleased: " << key << ">";
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
    //ofLog() << "<app::mouse released at: (" << x << ", " << y << ")>";
    try
    {
        auto selectedScene = renderer.scenes.GetSelectedScene();
        if (selectedScene)
        {
            selectedScene->mouseReleased(x, y, button);
        }
    } catch (std::exception& e)
    {
        ofLog() << "Aucune scène sélectionnée pour mouseReleased : " << e.what();
    }
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
            if (ImGui::MenuItem("Export screen as image"))
            {
                this->ExportImageButtonPressed();
            }
            if (ImGui::MenuItem("Custom Capture Settings"))
            {
                showPopup = true;
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Theme Selector"))
        {
            if (ImGui::MenuItem("Dark Blue Theme"))
            {
                SetDarkishBlueTheme();
            }
            if (ImGui::MenuItem("Dark Glass Theme"))
            {
                SetDarkGlassTheme();
            }
            if (ImGui::MenuItem("Windows XP Theme"))
            {
                SetWinXpTheme();
            }
            if (ImGui::MenuItem("Light Theme"))
            {
                SetLightTheme();
            }
            ImGui::End();
        }

        // Add padding to show the FPS checkbox on the right
        ImGui::SameLine(ImGui::GetWindowWidth() - (100 + fontSize * 16));
        if (ImGui::Checkbox("Vsync", &this->renderer.vsync))
        {
            this->renderer.SetVsync(this->renderer.vsync);
        }

        ImGui::Checkbox("Show FPS", &this->renderer.showFPS);

        ImGui::PushItemWidth(100);
        ImGui::InputInt("Target FPS", &this->renderer.targetFPS);
        if (ImGui::IsItemDeactivatedAfterEdit())
        {
            this->renderer.SetTargetFPS(this->renderer.targetFPS);
        }
        ImGui::PopItemWidth();
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

void Application::SetDarkishBlueTheme()
{
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;

    // Primary background
    colors[ImGuiCol_WindowBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
    colors[ImGuiCol_MenuBarBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);

    colors[ImGuiCol_PopupBg] = ImVec4(0.18f, 0.18f, 0.22f, 1.00f);

    // Headers
    colors[ImGuiCol_Header] = ImVec4(0.18f, 0.18f, 0.22f, 1.00f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.30f, 0.30f, 0.40f, 1.00f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.25f, 0.25f, 0.35f, 1.00f);

    // Buttons
    colors[ImGuiCol_Button] = ImVec4(0.20f, 0.22f, 0.27f, 1.00f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.30f, 0.32f, 0.40f, 1.00f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.35f, 0.38f, 0.50f, 1.00f);

    // Frame BG
    colors[ImGuiCol_FrameBg] = ImVec4(0.15f, 0.15f, 0.18f, 1.00f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.22f, 0.22f, 0.27f, 1.00f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.25f, 0.25f, 0.30f, 1.00f);

    // Tabs
    colors[ImGuiCol_Tab] = ImVec4(0.18f, 0.18f, 0.22f, 1.00f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.35f, 0.35f, 0.50f, 1.00f);
    colors[ImGuiCol_TabActive] = ImVec4(0.25f, 0.25f, 0.38f, 1.00f);
    colors[ImGuiCol_TabUnfocused] = ImVec4(0.13f, 0.13f, 0.17f, 1.00f);
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.20f, 0.20f, 0.25f, 1.00f);

    // Title
    colors[ImGuiCol_TitleBg] = ImVec4(0.12f, 0.12f, 0.15f, 1.00f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.15f, 0.15f, 0.20f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.10f, 0.10f, 0.12f, 1.00f);

    // Borders
    colors[ImGuiCol_Border] = ImVec4(0.20f, 0.20f, 0.25f, 0.50f);
    colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);

    // Text
    colors[ImGuiCol_Text] = ImVec4(0.90f, 0.90f, 0.95f, 1.00f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.55f, 1.00f);

    // Highlights
    colors[ImGuiCol_CheckMark] = ImVec4(0.50f, 0.70f, 1.00f, 1.00f);
    colors[ImGuiCol_SliderGrab] = ImVec4(0.50f, 0.70f, 1.00f, 1.00f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.60f, 0.80f, 1.00f, 1.00f);
    colors[ImGuiCol_ResizeGrip] = ImVec4(0.50f, 0.70f, 1.00f, 0.50f);
    colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.60f, 0.80f, 1.00f, 0.75f);
    colors[ImGuiCol_ResizeGripActive] = ImVec4(0.70f, 0.90f, 1.00f, 1.00f);
    colors[ImGuiCol_Separator] = ImVec4(0.50f, 0.70f, 1.00f, 1.00f);

    // Scrollbar
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.10f, 0.12f, 1.00f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.30f, 0.30f, 0.35f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.40f, 0.40f, 0.50f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.45f, 0.45f, 0.55f, 1.00f);

    // Style tweaks
    style.WindowRounding = 5.0f;
    style.FrameRounding = 5.0f;
    style.GrabRounding = 5.0f;
    style.TabRounding = 5.0f;
    style.PopupRounding = 5.0f;
    style.ScrollbarRounding = 5.0f;
    style.WindowPadding = ImVec2(10, 10);
    style.FramePadding = ImVec2(6, 4);
    style.ItemSpacing = ImVec2(8, 6);
    style.PopupBorderSize = 0.f;
}

void Application::SetWinXpTheme()
{
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;

    ImVec4 blancCreme = ImVec4(0.9f, 0.92f, 0.85f, 1.0f);

    ImVec4 gris = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);

    ImVec4 bleuFonce = ImVec4(0.0f, 0.2f, 0.6f, 1.0f);
    ImVec4 bleuGris = ImVec4(0.55f, 0.67f, 0.9f, 1.0f);
    ImVec4 bleuPale = ImVec4(0.55f, 0.67f, 0.9f, 1.0f);
    ImVec4 bleu = ImVec4(0.4f, 0.53f, 0.86f, 1.0f);
    ImVec4 bleu2 = ImVec4(0.19f, 0.42f, 0.77f, 1.0f);

    ImVec4 vert = ImVec4(0.07f, 0.57f, 0.05f, 1.0f);
    ImVec4 vertFonce = ImVec4(0.0f, 0.55f, 0.0f, 1.0f);
    ImVec4 vertPale = ImVec4(0.66f, 0.92, 0.61f, 1.0f);

    ImVec4 noir = ImVec4(0.0f, 0.0f, 0.0f, 1.00f);

    ImVec4 orange = ImVec4(1.0f, 0.6f, 0.2f, 1.00f);

    // (Backgrounds, Titles, Tabs)
    colors[ImGuiCol_WindowBg] = bleu;      // Main window
    colors[ImGuiCol_MenuBarBg] = vertFonce;// Menu bar
    colors[ImGuiCol_PopupBg] = bleuFonce;  // Popup windows
    colors[ImGuiCol_TitleBg] = bleuPale;   // Title bar
    colors[ImGuiCol_TitleBgActive] = bleuFonce;
    colors[ImGuiCol_TitleBgCollapsed] = bleuPale;
    colors[ImGuiCol_TabUnfocused] = vertPale;
    colors[ImGuiCol_TabUnfocusedActive] = bleuFonce;

    // (Text, Highlights)
    colors[ImGuiCol_Text] = blancCreme;  // Main text
    colors[ImGuiCol_TextDisabled] = gris;// Disabled text

    colors[ImGuiCol_CheckMark] = blancCreme;// Checkmark

    // (Tabs, Headers, Sliders, Grips)
    colors[ImGuiCol_Header] = bleu;            // Headers
    colors[ImGuiCol_HeaderHovered] = bleuFonce;// Hovered Headers
    colors[ImGuiCol_HeaderActive] = bleuPale;  // Active Headers
    colors[ImGuiCol_Tab] = bleu;
    colors[ImGuiCol_TabHovered] = bleuFonce;
    colors[ImGuiCol_TabActive] = bleuPale;

    colors[ImGuiCol_SliderGrab] = bleuFonce;
    colors[ImGuiCol_SliderGrabActive] = bleuPale;

    colors[ImGuiCol_ResizeGrip] = vert;
    colors[ImGuiCol_ResizeGripHovered] = bleu2;
    colors[ImGuiCol_ResizeGripActive] = bleuPale;

    colors[ImGuiCol_Separator] = vertFonce;

    // Borders, Scrollbars, Backgrounds
    ImVec4 grayShadow = ImVec4(0.0f, 0.00f, 0.00f, 0.5f);

    colors[ImGuiCol_Border] = noir;
    colors[ImGuiCol_BorderShadow] = grayShadow;

    colors[ImGuiCol_ScrollbarBg] = blancCreme;
    colors[ImGuiCol_ScrollbarGrab] = bleu;
    colors[ImGuiCol_ScrollbarGrabHovered] = bleuPale;
    colors[ImGuiCol_ScrollbarGrabActive] = bleuPale;

    // Buttons & Frames
    colors[ImGuiCol_Button] = bleu2;
    colors[ImGuiCol_ButtonHovered] = bleu;
    colors[ImGuiCol_ButtonActive] = bleuFonce;
    colors[ImGuiCol_FrameBg] = bleu2;
    colors[ImGuiCol_FrameBgHovered] = bleu;
    colors[ImGuiCol_FrameBgActive] = bleuFonce;

    // Style Tweaks
    style.WindowRounding = 10.0f;
    style.FrameRounding = 5.0f;
    style.GrabRounding = 10.0f;
    style.TabRounding = 10.0f;
    style.PopupRounding = 10.0f;
    style.ScrollbarRounding = 1.0f;
    style.WindowPadding = ImVec2(10, 10);
    style.FramePadding = ImVec2(6, 4);
    style.ItemSpacing = ImVec2(8, 6);
    style.PopupBorderSize = 0.f;
}

void Application::SetDarkGlassTheme()
{
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;

    /// Setting up a dark theme base
    colors[ImGuiCol_WindowBg] = ImVec4(0.1f, 0.1f, 0.1f, 0.6f); // Semi-transparent dark background
    colors[ImGuiCol_MenuBarBg] = ImVec4(0.1f, 0.1f, 0.1f, 0.8f);// ADDED
    colors[ImGuiCol_ChildBg] = ImVec4(0.1f, 0.1f, 0.1f, 0.4f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.8f);
    colors[ImGuiCol_Border] = ImVec4(0.8f, 0.8f, 0.8f, 0.2f);

    // Text and frames
    colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.2f, 0.2f, 0.2f, 0.5f);// Semi-transparent for frosted look
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.3f, 0.3f, 0.3f, 0.7f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.3f, 0.3f, 0.3f, 0.9f);

    // Header
    colors[ImGuiCol_Header] = ImVec4(0.3f, 0.3f, 0.3f, 0.7f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.4f, 0.4f, 0.4f, 0.8f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);

    // Buttons
    colors[ImGuiCol_Button] = ImVec4(0.3f, 0.3f, 0.3f, 0.6f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.4f, 0.4f, 0.4f, 0.8f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
    // ------------------------ ADDED ------------------------
    // Frame BG
    colors[ImGuiCol_FrameBg] = ImVec4(0.15f, 0.15f, 0.18f, 1.00f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.22f, 0.22f, 0.27f, 1.00f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.25f, 0.25f, 0.30f, 1.00f);

    // Scrollbar
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.10f, 0.12f, 1.00f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.30f, 0.30f, 0.35f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.40f, 0.40f, 0.50f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.45f, 0.45f, 0.55f, 1.00f);

    // Highlights
    colors[ImGuiCol_CheckMark] = ImVec4(0.50f, 0.70f, 1.00f, 1.00f);
    colors[ImGuiCol_SliderGrab] = ImVec4(0.50f, 0.70f, 1.00f, 1.00f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.60f, 0.80f, 1.00f, 1.00f);
    colors[ImGuiCol_ResizeGrip] = ImVec4(0.50f, 0.70f, 1.00f, 0.50f);
    colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.60f, 0.80f, 1.00f, 0.75f);
    colors[ImGuiCol_ResizeGripActive] = ImVec4(0.70f, 0.90f, 1.00f, 1.00f);
    colors[ImGuiCol_Separator] = ImVec4(0.50f, 0.70f, 1.00f, 1.00f);

    // Tabs
    colors[ImGuiCol_Tab] = ImVec4(0.18f, 0.18f, 0.22f, 0.80f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.35f, 0.35f, 0.50f, 1.00f);
    colors[ImGuiCol_TabActive] = ImVec4(0.25f, 0.25f, 0.38f, 1.00f);
    colors[ImGuiCol_TabUnfocused] = ImVec4(0.13f, 0.13f, 0.17f, 1.00f);
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.20f, 0.20f, 0.25f, 1.00f);

    // Title
    colors[ImGuiCol_TitleBg] = ImVec4(0.12f, 0.12f, 0.15f, 1.00f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.15f, 0.15f, 0.20f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.10f, 0.10f, 0.12f, 1.00f);
    // --------------------------------------------------------
    // Adjust window rounding and padding to enhance the glass look
    style.WindowRounding = 10.0f;
    style.FrameRounding = 5.0f;
    style.WindowPadding = ImVec2(10, 10);
    style.FramePadding = ImVec2(5, 5);
}

void Application::SetLightTheme()
{
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;

    // General window settings
    style.WindowRounding = 5.0f;
    style.FrameRounding = 5.0f;
    style.ScrollbarRounding = 5.0f;
    style.GrabRounding = 5.0f;
    style.TabRounding = 5.0f;
    style.WindowBorderSize = 1.0f;
    style.FrameBorderSize = 1.0f;
    style.PopupBorderSize = 1.0f;
    style.PopupRounding = 5.0f;

    // Setting the colors (Light version)
    colors[ImGuiCol_Text] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
    colors[ImGuiCol_WindowBg] = ImVec4(0.95f, 0.95f, 0.95f, 1.00f);// Light background
    colors[ImGuiCol_ChildBg] = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.98f, 0.98f, 0.98f, 1.00f);
    colors[ImGuiCol_Border] = ImVec4(0.70f, 0.70f, 0.70f, 1.00f);
    colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.85f, 0.85f, 0.85f, 1.00f);// Light frame background
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.80f, 0.80f, 0.80f, 1.00f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.75f, 0.75f, 0.75f, 1.00f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.85f, 0.85f, 0.85f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
    colors[ImGuiCol_MenuBarBg] = ImVec4(0.95f, 0.95f, 0.95f, 1.00f);
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.80f, 0.80f, 0.80f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.75f, 0.75f, 0.75f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.70f, 0.70f, 0.70f, 1.00f);

    // Accent colors with a soft pastel gray-green
    colors[ImGuiCol_CheckMark] = ImVec4(0.55f, 0.65f, 0.55f, 1.00f);// Soft gray-green for check marks
    colors[ImGuiCol_SliderGrab] = ImVec4(0.55f, 0.65f, 0.55f, 1.00f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.60f, 0.70f, 0.60f, 1.00f);
    colors[ImGuiCol_Button] = ImVec4(0.85f, 0.85f, 0.85f, 1.00f);// Light button background
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.80f, 0.80f, 0.80f, 1.00f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.75f, 0.75f, 0.75f, 1.00f);
    colors[ImGuiCol_Header] = ImVec4(0.75f, 0.75f, 0.75f, 1.00f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.70f, 0.70f, 0.70f, 1.00f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.65f, 0.65f, 0.65f, 1.00f);
    colors[ImGuiCol_Separator] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
    colors[ImGuiCol_SeparatorHovered] = ImVec4(0.65f, 0.65f, 0.65f, 1.00f);
    colors[ImGuiCol_SeparatorActive] = ImVec4(0.70f, 0.70f, 0.70f, 1.00f);
    colors[ImGuiCol_ResizeGrip] = ImVec4(0.55f, 0.65f, 0.55f, 1.00f);// Accent color for resize grips
    colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.60f, 0.70f, 0.60f, 1.00f);
    colors[ImGuiCol_ResizeGripActive] = ImVec4(0.65f, 0.75f, 0.65f, 1.00f);
    colors[ImGuiCol_Tab] = ImVec4(0.85f, 0.85f, 0.85f, 1.00f);// Tabs background
    colors[ImGuiCol_TabHovered] = ImVec4(0.80f, 0.80f, 0.80f, 1.00f);
    colors[ImGuiCol_TabActive] = ImVec4(0.75f, 0.75f, 0.75f, 1.00f);
    colors[ImGuiCol_TabUnfocused] = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.75f, 0.75f, 0.75f, 1.00f);
    colors[ImGuiCol_DockingPreview] = ImVec4(0.55f, 0.65f, 0.55f, 1.00f);// Docking preview in gray-green
    colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);

    // Additional styles
    style.FramePadding = ImVec2(8.0f, 4.0f);
    style.ItemSpacing = ImVec2(8.0f, 4.0f);
    style.IndentSpacing = 20.0f;
    style.ScrollbarSize = 16.0f;
}
