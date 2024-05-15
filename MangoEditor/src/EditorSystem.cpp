#define GLFW_INCLUDE_NONE 

#include "EditorSystem.h"
#include "DragDropPayloadTypes.h"
#include "IconsMaterialDesignIcons.h"

#include "Mango/ImGui/ImGuiUtils.h"
#include "Mango/Math/Math.h"
#include "Mango/Project/ProjectSerializer.h"
#include "Mango/Scene/SceneSerializer.h"
#include "Mango/Scene/SelectionManager.h"
#include "Mango/Systems/PhysicsSystem.h"

#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>
#include <ImGuizmo.h>
#include <portable-file-dialogs.h>

namespace
{
    ImGuizmo::OPERATION getImGuizmoOp(mango::GizmoType type)
    {
        switch (type)
        {
            case mango::GizmoType::TRANSLATE: return ImGuizmo::OPERATION::TRANSLATE;
            case mango::GizmoType::ROTATE:    return ImGuizmo::OPERATION::ROTATE;
            case mango::GizmoType::SCALE:     return ImGuizmo::OPERATION::SCALE;
            case mango::GizmoType::TRANSFORM: return ImGuizmo::OPERATION::UNIVERSAL;
        }

        MG_ASSERT_FAIL("Unknown GizmoType!");
        return ImGuizmo::OPERATION::TRANSLATE;
    }

    ImGuizmo::MODE getImGuizmoMode(mango::GizmoMode mode)
    {
        switch (mode)
        {
            case mango::GizmoMode::LOCAL: return ImGuizmo::MODE::LOCAL;
            case mango::GizmoMode::WORLD: return ImGuizmo::MODE::WORLD;
        }

        MG_ASSERT_FAIL("Unknown GizmoMode!");
        return ImGuizmo::MODE::LOCAL;
    }
}

namespace mango
{
    EditorSystem::EditorSystem()
        : System("EditorSystem")
    {
        // Set CVars
        CVarFloat CVarCameraRotationSpeed("editor.camera.rotationSpeed", "rotation speed of the editor camera", 0.2f);
        CVarFloat CVarCameraMoveSpeed    ("editor.camera.moveSpeed",     "movement speed of the editor camera", 10.0f);

        // Listen to the events
        Services::eventBus()->subscribe<RequestSceneLoadEvent>(MG_BIND_EVENT(EditorSystem::onReceiveSceneLoadEvent));
    }

    void EditorSystem::onInit()
    {
        auto appConfig = Services::application()->getConfig();
        if (!appConfig.projectPath.empty())
        {
            if (!openProject(appConfig.projectPath))
            {
                Services::application()->close();
            }
        }
        else
        {
            m_isMangoHubOpen = true;
        }

        // Setup font
        ImGuiSystem::addFont("Inter16", "fonts/inter/Inter-Regular.ttf", 16.0f, true);
        
        static const ImWchar iconRanges[3] = { ICON_MIN_MDI, ICON_MAX_MDI, 0 };
        ImGuiSystem::mergeFont("fonts/materialdesignicons-7.4.47/" FONT_ICON_FILE_NAME_MDI, 18.0f, iconRanges, 1.0f / 3.0f);

        ImGuiSystem::addFont("InterBold16",  "fonts/inter/Inter-Bold.ttf",    16.0f); 
        ImGuiSystem::addFont("Inter24",      "fonts/inter/Inter-Regular.ttf", 24.0f);

        // Load icons
        m_playIcon = createRef<Texture>();
        m_playIcon->createTexture2d("icons/play.png", false, 8);

        m_playPressedIcon = createRef<Texture>();
        m_playPressedIcon->createTexture2d("icons/playPressed.png", false, 8);

        m_simulateIcon = createRef<Texture>();
        m_simulateIcon->createTexture2d("icons/simulate.png", false, 8);

        m_simulatePressedIcon = createRef<Texture>();
        m_simulatePressedIcon->createTexture2d("icons/simulatePressed.png", false, 8);

        m_pauseIcon = createRef<Texture>();
        m_pauseIcon->createTexture2d("icons/pause.png", false, 8);

        m_pausePressedIcon = createRef<Texture>();
        m_pausePressedIcon->createTexture2d("icons/pausePressed.png", false, 8);

        m_stepIcon = createRef<Texture>();
        m_stepIcon->createTexture2d("icons/step.png", false, 8);

        m_editorCamera.setPerspective(glm::radians(45.0f), Services::application()->getWindow()->getAspectRatio(), 0.1f, 1000.0f);
        m_editorCamera.setPosition(glm::vec3(0.0f, 4.0f, 30.0f));

        Services::renderer()->setOutputToOffscreenTexture(true);
        Services::application()->getWindow()->setVSync(false);
        Services::application()->getImGuiSystem()->setDefaultIniSettingsFile("imgui.ini");

        Services::eventBus()->subscribe<GamepadConnectedEvent>([](const GamepadConnectedEvent& event)
        {
            if (event.isConnected)
            {
                MG_CORE_INFO("Conencted gamepad:\n  ID: {}\n  Name: {}", int(event.gid), Input::getGamepadName(event.gid));
            }
            else
            {
                MG_CORE_ERROR("Disconencted gamepad:\n  ID: {}", int(event.gid));
            }
        });

        if (Project::getActive())
        {
            /*auto skybox = createRef<Skybox>("skyboxes/stormydays/",
                                              "stormydays_lf.tga",
                                              "stormydays_rt.tga",
                                              "stormydays_up.tga", 
                                              "stormydays_dn.tga", 
                                              "stormydays_ft.tga", 
                                              "stormydays_bk.tga" );*/
            auto skybox = createRef<Skybox>("skyboxes/cold/",
                                            "right.jpg",
                                            "left.jpg",
                                            "top.jpg", 
                                            "bottom.jpg", 
                                            "front.jpg", 
                                            "back.jpg" );
            Services::renderer()->setSkybox(skybox);
        }
    }

    void EditorSystem::onDestroy()
    {
    }

    void EditorSystem::onUpdate(float dt)
    {
        if (SceneState::Edit == m_sceneState || SceneState::Simulate == m_sceneState)
        {
            if (!m_editorCamera.isUsing())
            {
                bool ctrl  = Input::getKey(KeyCode::LeftControl) || Input::getKey(KeyCode::RightControl);
                bool shift = Input::getKey(KeyCode::LeftShift)   || Input::getKey(KeyCode::RightShift);

                if (Input::getKeyDown(KeyCode::N))
                {
                    if (ctrl)
                    {
                        newScene();
                        return;
                    }
                }

                if (Input::getKeyDown(KeyCode::O))
                {
                    if (ctrl)
                    {
                        openScene();
                        return;
                    }
                }

                if (Input::getKeyDown(KeyCode::S))
                {
                    if (ctrl)
                    {
                        if (shift)
                        {
                            saveSceneAs();
                            return;
                        }
                
                        saveScene();
                        return;
                    }
                }

                if (Input::getKeyDown(KeyCode::D))
                {
                    if (ctrl)
                    {
                        onDuplicateEntity();
                        return;
                    }
                }

                if (Input::getKeyDown(KeyCode::W))
                {
                    m_gizmoType = GizmoType::TRANSLATE;
                }

                if (Input::getKeyDown(KeyCode::E))
                {
                    m_gizmoType = GizmoType::ROTATE;
                }

                if (Input::getKeyDown(KeyCode::R))
                {
                    m_gizmoType = GizmoType::SCALE;
                }

                if (Input::getKeyDown(KeyCode::Q))
                {
                    m_gizmoType = GizmoType::NONE;
                }

                if (Input::getKeyDown(KeyCode::T))
                {
                    m_gizmoMode = (m_gizmoMode == GizmoMode::LOCAL) ? GizmoMode::WORLD : GizmoMode::LOCAL;
                }

                if (Input::getKeyDown(KeyCode::Escape))
                {
                    SelectionManager::resetEntitySelection();
                }
            }

            m_editorCamera.onUpdate(dt);
        }
        else if (SceneState::Play == m_sceneState)
        {
            static bool shouldMoveLights = false;
            if (Input::getKeyDown(KeyCode::Space) || Input::getGamepadButtonDown(GamepadID::PAD_1, GamepadButton::X))
            {
                shouldMoveLights = !shouldMoveLights;
            }

            if (shouldMoveLights)
            {
                moveLights(dt);
            }
        }
    }
    
    void EditorSystem::onGui()
    {
        if (m_isMangoHubOpen) onGuiMangoHub();

        bool isActiveProject = Project::getActive() != nullptr;

        // Resize the main FBO based not on the window, but based on the Viewport panel !!
        // Resize
        if (glm::uvec2 mainFramebufferSize = Services::renderer()->getMainFramebufferSize();
            m_viewportSize.x > 0.0f && m_viewportSize.y > 0.0f && // zero sized framebuffer is invalid
            (mainFramebufferSize.x != m_viewportSize.x || mainFramebufferSize.y != m_viewportSize.y))
        {
            Services::renderer()->resize(m_viewportSize.x, m_viewportSize.y);
        }

        static bool dockspaceOpen = true;
        static ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_None;

        // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
        // because it would be confusing to have two docking targets within each others.
        ImGuiWindowFlags windowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

        // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
        // and handle the pass-thru hole, so we ask Begin() to not render a background.
        if (dockspaceFlags & ImGuiDockNodeFlags_PassthruCentralNode)
            windowFlags |= ImGuiWindowFlags_NoBackground;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("Mango Editor", &dockspaceOpen, windowFlags);
        ImGui::PopStyleVar(3);

        // Submit the DockSpace
        ImGuiIO&    io                    = ImGui::GetIO();
        ImGuiStyle& style                 = ImGui::GetStyle();
        float       minImGuiWindowSizeX   = style.WindowMinSize.x;
                    style.WindowMinSize.x = 370.0f;

        ImGuiID dockspaceID = ImGui::GetID("MyDockSpace");
        if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
        {
            ImGui::DockSpace(dockspaceID, ImVec2(0.0f, 0.0f), dockspaceFlags);
        }
        style.WindowMinSize.x = minImGuiWindowSizeX;

        /** Menu Bar */
        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("New Scene", "Ctrl+N", nullptr, isActiveProject))
                {
                    newScene();
                }

                if (ImGui::MenuItem("Open Scene", "Ctrl+O", nullptr, isActiveProject))
                {
                    openScene();
                }

                ImGui::Separator();

                if (ImGui::MenuItem("Save Scene", "Ctrl+S", nullptr, isActiveProject))
                {
                    saveScene();
                }

                if (ImGui::MenuItem("Save Scene As...", "Ctrl+Shift+S", nullptr, isActiveProject))
                {
                    saveSceneAs();
                }

                ImGui::Separator();

                if (ImGui::MenuItem("New Project..."))
                {
                    newProject();
                }

                if (ImGui::MenuItem("Open Project..."))
                {
                    m_isMangoHubOpen = true;
                }

                if (ImGui::MenuItem("Save Project", nullptr, nullptr, isActiveProject))
                {
                    saveProject();
                }

                ImGui::Separator();

                if (ImGui::MenuItem("Exit")) Services::application()->close();
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

        if (isActiveProject)
        {
            ImGui::Begin("CVars Editor");
            {
                CVarSystem::get()->drawImguiEditor();
            }
            ImGui::End(); // CVars Editor

            onGuiViewport();
            onOverlayPlayPauseSim();
            onOverlayGizmoManipulators();

            m_sceneHierarchyPanel.onGui();
            m_assetsBrowserPanel->onGui();
            m_materialEditorPanel.onGui();

            onGuiStats();
            onGuiRenderingSettings();
            onGuiProjectSettings();
        }

        ImGui::End(); // Mango Editor
    }

    void EditorSystem::newProject()
    {
        m_isMangoHubOpen = true;
    }

    bool EditorSystem::openProject(const std::filesystem::path& path)
    {
        if (Project::load(path))
        {
            // Populate VFI search path
            VFI::addToSearchPath(Project::getActiveProjectDirectory());
            VFI::addToSearchPath(Project::getActiveAssetDirectory());

            auto searchpath = VFI::getSearchPath();
            for (auto& p : searchpath)
            {
                MG_TRACE(p);
            }

            // Unload previous resources
            AssetManagerOld::unload();

            // Open start scene
            openScene(Project::getActive()->getConfig().startScene);
            m_assetsBrowserPanel = createScope<ContentBrowserPanel>();

            return true;
        }

        return false;
    }

    bool EditorSystem::openProject()
    {
        auto file = pfd::open_file("", "", { "Mango Project (*" MG_PROJECT_EXTENSION ")", "*" MG_PROJECT_EXTENSION }, false);

        if (file.result().empty()) return false;
    
        return openProject(file.result()[0]);
    }

    void EditorSystem::saveProject()
    {
        auto projectName      = Project::getActive()->getConfig().name + MG_PROJECT_EXTENSION;
        auto projectDirectory = Project::getActiveProjectDirectory();

        Project::saveActive(projectDirectory / projectName);
    }

    void EditorSystem::newScene()
    {
        m_editorScene = Services::sceneManager()->createScene("New Scene");
        m_activeScene = m_editorScene;
        Services::sceneManager()->setActiveScene(m_activeScene);
        m_sceneHierarchyPanel.setScene(m_activeScene);

        m_editorScenePath = std::filesystem::path();
    }

    void EditorSystem::openScene()
    {
        auto file = pfd::open_file("", Project::getActiveProjectDirectory().string(), {"Mango Scene (*" MG_SCENE_EXTENSION ")", "*" MG_SCENE_EXTENSION}, false);
        
        if (!file.result().empty())
        {
            // Check relative path; if the scene file is not in a child directory of project path
            // don't load this scene file.
            auto relativeFilepath = std::filesystem::relative(file.result()[0], Project::getActiveAssetDirectory());

            if (!relativeFilepath.empty())
            {
                openScene(relativeFilepath);
            }
        }
    }

    void EditorSystem::openScene(const std::filesystem::path& path)
    {
        if (SceneState::Edit != m_sceneState)
        {
            auto choice = pfd::message("Warning!", 
                                       "Are you sure you want to open a new scene while the current scene is playing?", 
                                       pfd::choice::ok_cancel, 
                                       pfd::icon::warning);
            switch (choice.result())
            {
                case pfd::button::ok:
                    onSceneStop();
                    break;
                case pfd::button::cancel:
                    return;
            }
        }

        if (path.extension().string() != MG_SCENE_EXTENSION)
        {
            MG_WARN("Could not load {0} - not a scene file", path.filename().string());
            return;
        }

        auto pathString = path.string();

        m_editorScenePath = VFI::getFilepath(pathString).make_preferred();
        m_editorScene     = SceneSerializer::deserialize(m_editorScenePath);
        m_activeScene     = m_editorScene;

        Services::sceneManager()->setActiveScene(m_activeScene);
        m_sceneHierarchyPanel.setScene(m_activeScene);
    }

    void EditorSystem::saveScene()
    {
        if (!m_editorScenePath.empty())
        {
            SceneSerializer::serialize(m_activeScene, m_editorScenePath);
        }
        else
        {
            saveSceneAs();
        }
    }

    void EditorSystem::saveSceneAs()
    {
        auto file = pfd::save_file("", "." /*TODO: project path here*/, { "Mango Scene (*" MG_SCENE_EXTENSION ")", "*" MG_SCENE_EXTENSION }, true);
        if (!file.result().empty())
        {
            m_editorScenePath = file.result();

            if (!m_editorScenePath.has_extension())
            {
                m_editorScenePath += ".mango";
            }

            SceneSerializer::serialize(m_activeScene, m_editorScenePath);
        }
    }

    void EditorSystem::onScenePlay()
    {
        if (SceneState::Simulate == m_sceneState) onSceneStop();

        m_sceneState = SceneState::Play;

        m_activeScene = Scene::copy(m_editorScene);
        m_sceneHierarchyPanel.setScene(m_activeScene);
        Services::sceneManager()->setActiveScene(m_activeScene);
        Services::application()->getPhysicsSystem()->start();
    }

    void EditorSystem::onSceneSimulate()
    {
        if (SceneState::Play == m_sceneState) onSceneStop();

        m_sceneState = SceneState::Simulate;

        m_activeScene = Scene::copy(m_editorScene);
        m_sceneHierarchyPanel.setScene(m_activeScene);
        Services::sceneManager()->setActiveScene(m_activeScene);
        Services::application()->getPhysicsSystem()->start();
    }

    void EditorSystem::onSceneStop()
    {
        m_sceneState = SceneState::Edit;
        m_activeScene = m_editorScene;
        m_sceneHierarchyPanel.setScene(m_activeScene);
        Services::sceneManager()->setActiveScene(m_activeScene);
        Services::application()->getPhysicsSystem()->stop();
    }

    void EditorSystem::onGuiViewport()
    {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
        ImGui::Begin("Viewport");
        {
            auto node = ImGui::GetWindowDockNode();
            node->LocalFlags |= ImGuiDockNodeFlags_NoWindowMenuButton | 
                                ImGuiDockNodeFlags_NoCloseButton      | 
                                ImGuiDockNodeFlags_NoUndocking        | 
                                ImGuiDockNodeFlags_NoTabBar;

            if (SceneState::Edit == m_sceneState || SceneState::Simulate == m_sceneState)
            {
                Services::renderer()->setRenderingMode(RenderingMode::EDITOR, &m_editorCamera, m_editorCamera.getPosition());
            }
            else if (SceneState::Play == m_sceneState)
            {
                Services::renderer()->setRenderingMode(RenderingMode::GAME);
            }
            
            auto viewportMinRegion  = ImGui::GetWindowContentRegionMin();
            auto viewportMaxRegion  = ImGui::GetWindowContentRegionMax();
            auto viewportOffset     = ImGui::GetWindowPos();

            m_viewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
            m_viewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };

            m_viewportFocused = ImGui::IsWindowFocused();
            m_viewportHovered = ImGui::IsWindowHovered();

            if (m_viewportHovered)
            {
                ImGui::SetNextFrameWantCaptureKeyboard(false);
                ImGui::SetNextFrameWantCaptureMouse(false);
            }

            ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
                   m_viewportSize    = { viewportPanelSize.x, viewportPanelSize.y };
                   
            uint32_t outputTextureID = Services::renderer()->getOutputOffscreenTextureID();

            ImGui::Image((ImTextureID)outputTextureID, viewportPanelSize, { 0, 1 }, { 1, 0 });

            /** Drag drop target */
            if (ImGui::BeginDragDropTarget())
            {
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(MG_DRAG_PAYLOAD_CB_ITEM))
                {
                    const auto* path = (const wchar_t*)payload->Data;
                    openScene(path);
                }
                ImGui::EndDragDropTarget();
            }

            if (SceneState::Edit == m_sceneState || SceneState::Simulate == m_sceneState)
            {
                /** ImGuizmo */
                Entity selectedEntity = SelectionManager::getSelectedEntity();
                if (selectedEntity && m_gizmoType != GizmoType::NONE)
                {
                    ImGuizmo::SetOrthographic(false);
                    ImGuizmo::SetDrawlist();

                    ImGuizmo::SetRect(m_viewportBounds[0].x, m_viewportBounds[0].y, m_viewportBounds[1].x - m_viewportBounds[0].x, m_viewportBounds[1].y - m_viewportBounds[0].y);

                    // Camera
                    const glm::mat4& cameraView       = m_editorCamera.getView();
                    const glm::mat4& cameraProjection = m_editorCamera.getProjection();

                    // Entity transform
                    auto& tc = selectedEntity.getComponent<TransformComponent>();
                    glm::mat4 transform = tc.getWorldMatrix();

                    // Snapping
                    bool snap = Input::getKey(KeyCode::LeftControl);
                    float snapValue = 0.5f; // Snap to 0.5m for translation and scale

                    // Snap to 45 degrees for rotation
                    if (m_gizmoType == GizmoType::ROTATE)
                    {
                        snapValue = 45.0f;
                    }
                    glm::vec3 snapValues = glm::vec3(snapValue);

                    ImGuizmo::Manipulate(glm::value_ptr(cameraView), 
                                         glm::value_ptr(cameraProjection), 
                                         getImGuizmoOp(m_gizmoType), 
                                         getImGuizmoMode(m_gizmoMode), 
                                         glm::value_ptr(transform),
                                         nullptr /*deltaMatrix*/,
                                         snap ? glm::value_ptr(snapValues) : nullptr);

                    if (ImGuizmo::IsUsing())
                    {
                        transform = glm::inverse(tc.getParentWorldMatrix()) * transform;
                        glm::vec3 translation, rotation, scale;
                        math::decompose(transform,
                                        translation,
                                        rotation,
                                        scale);

                        // Calculate delta rotation, to always add 
                        // small portion of the rotation to avoid the gimbal lock
                        glm::vec3 currentRotation = tc.getLocalRotation();
                        glm::vec3 deltaRotation   = rotation - currentRotation;

                        tc.setLocalPosition(translation);
                        tc.setLocalRotation(currentRotation + deltaRotation);
                        tc.setLocalScale(scale);
                    }
                }

                // Mouse Entity picking
                if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !ImGuizmo::IsOver() && m_viewportHovered)
                {
                    auto mousePos  = Input::getMousePosition();
                         mousePos -= m_viewportBounds[0];

                    auto viewportSize = m_viewportBounds[1] - m_viewportBounds[0];
                         mousePos.y   = viewportSize.y - mousePos.y;

                    if (mousePos.x >= 0 && mousePos.y >= 0 && mousePos.x < (int)viewportSize.x && mousePos.y < (int)viewportSize.y)
                    {
                        int    selectedID     = Services::renderer()->getSelectedEntityID(mousePos.x, mousePos.y);
                        Entity selectedEntity = (selectedID == -1) ? Entity() : Entity((entt::entity)selectedID, m_activeScene.get());

                        SelectionManager::selectEntity(selectedEntity);
                    }
                }
            }
        }
        ImGui::End(); // Viewport
        ImGui::PopStyleVar();
    }

    void EditorSystem::onOverlayPlayPauseSim()
    {
        ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDecoration       |
                                       ImGuiWindowFlags_NoDocking          |
                                       ImGuiWindowFlags_AlwaysAutoResize   |
                                       ImGuiWindowFlags_NoSavedSettings    |
                                       ImGuiWindowFlags_NoFocusOnAppearing |
                                       ImGuiWindowFlags_NoNav              |
                                       ImGuiWindowFlags_NoMove;

        // Center window
        float  paddingY          = m_viewportSize.y * 0.005f;
        ImVec2 viewportTopCenter = { m_viewportBounds[0].x + m_viewportSize.x * 0.5f, m_viewportBounds[0].y + paddingY };
        ImGui::SetNextWindowPos(viewportTopCenter, ImGuiCond_Always, ImVec2(0.5f, 0.0f));
        ImGui::SetNextWindowBgAlpha(0.75f); // Transparent background

        constexpr ImVec2 iconSize   (40.0f, 40.0f);
        constexpr ImVec2 itemSpacing(0.0f,  5.0f);

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding,    ImVec2(1.0f, 1.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding,   5.0f);
        if (ImGui::Begin("Play Pause Sim Overlay", nullptr, windowFlags))
        {
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing,  itemSpacing);
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));

                  auto& colors        = ImGui::GetStyle().Colors;
            const auto& buttonHovered = colors[ImGuiCol_ButtonHovered];
            const auto& buttonActive  = colors[ImGuiCol_ButtonActive];

            ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4(0, 0, 0, 0));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(buttonHovered.x, buttonHovered.y, buttonHovered.z, 0.5f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ImVec4(buttonActive.x,  buttonActive.y,  buttonActive.z,  0.5f));

            auto playIcon = (SceneState::Play == m_sceneState) ? m_playPressedIcon : m_playIcon;
            if (ImGui::ImageButton("playButton", (ImTextureID)playIcon->getRendererID(), iconSize))
            {
                if (SceneState::Play == m_sceneState)
                {
                    onSceneStop();
                }
                else
                {
                    onScenePlay();
                }
            }

            ImGui::SameLine();
            auto simulateIcon = (SceneState::Simulate == m_sceneState) ? m_simulatePressedIcon : m_simulateIcon;
            if (ImGui::ImageButton("simulateButton", (ImTextureID)simulateIcon->getRendererID(), iconSize))
            {
                if (SceneState::Simulate == m_sceneState)
                {
                    onSceneStop();
                }
                else
                {
                    onSceneSimulate();
                }
            }

            bool isPaused = Services::application()->isPaused();
            auto pauseIcon = isPaused ? m_pausePressedIcon : m_pauseIcon;

            ImGui::SameLine();
            if (ImGui::ImageButton("pauseButton", (ImTextureID)pauseIcon->getRendererID(), iconSize))
            {
                Services::application()->setPaused(!isPaused);
            }

            ImGui::BeginDisabled(!isPaused || SceneState::Edit == m_sceneState);
            {
                ImGui::SameLine();
                if (ImGui::ImageButton("stepButton", (ImTextureID)m_stepIcon->getRendererID(), iconSize))
                {
                    Services::application()->step();
                }
            }
            ImGui::EndDisabled();

            ImGui::PopStyleColor(3);
            ImGui::PopStyleVar(2);
        }
        ImGui::PopStyleVar(3);
        ImGui::End();
    }

    void EditorSystem::onOverlayGizmoManipulators()
    {
        ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDecoration       |
                                       ImGuiWindowFlags_NoDocking          |
                                       ImGuiWindowFlags_AlwaysAutoResize   |
                                       ImGuiWindowFlags_NoSavedSettings    |
                                       ImGuiWindowFlags_NoFocusOnAppearing |
                                       ImGuiWindowFlags_NoNav              |
                                       ImGuiWindowFlags_NoMove;

        // Center window
        float  paddingX           = m_viewportSize.x * 0.01f;
        float  paddingY           = m_viewportSize.y * 0.005f;
        ImVec2 viewportLeftCenter = { m_viewportBounds[0].x + paddingX, m_viewportBounds[0].y + paddingY };
        ImGui::SetNextWindowPos(viewportLeftCenter, ImGuiCond_Always, ImVec2(0.0f, 0.0f));
        ImGui::SetNextWindowBgAlpha(0.75f); // Transparent background

        constexpr ImVec2 iconSize         (40.0f, 40.0f);
        constexpr ImVec2 itemSpacing      (-5.0f,  5.0f);
        constexpr ImVec4 selectedTextColor(0.9569, 0.7333, 0.2667, 1.0);
        
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding,    ImVec2(1.0f, 1.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding,   5.0f);
        if (ImGui::Begin("Gizmo Manipulators Overlay", nullptr, windowFlags))
        {
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing,  itemSpacing);
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));

                  auto& colors        = ImGui::GetStyle().Colors;
            const auto& buttonHovered = colors[ImGuiCol_ButtonHovered];
            const auto& buttonActive  = colors[ImGuiCol_ButtonActive];

            ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4(0, 0, 0, 0));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(buttonHovered.x, buttonHovered.y, buttonHovered.z, 0.5f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ImVec4(buttonActive.x,  buttonActive.y,  buttonActive.z,  0.5f));

            if (GizmoType::NONE == m_gizmoType) ImGui::PushStyleColor(ImGuiCol_Text, selectedTextColor);
            if (ImGui::Button(ICON_MDI_CURSOR_DEFAULT, iconSize))
            {
                m_gizmoType = GizmoType::NONE;
            }
            else if (GizmoType::NONE == m_gizmoType) ImGui::PopStyleColor();

            ImGui::SameLine();
            if (GizmoType::TRANSLATE == m_gizmoType) ImGui::PushStyleColor(ImGuiCol_Text, selectedTextColor);
            if (ImGui::Button(ICON_MDI_CURSOR_MOVE, iconSize))
            {
                m_gizmoType = GizmoType::TRANSLATE;
            }
            else if (GizmoType::TRANSLATE == m_gizmoType) ImGui::PopStyleColor();

            ImGui::SameLine();
            if (GizmoType::ROTATE == m_gizmoType) ImGui::PushStyleColor(ImGuiCol_Text, selectedTextColor);
            if (ImGui::Button(ICON_MDI_AUTORENEW, iconSize))
            {
                m_gizmoType = GizmoType::ROTATE;
            }
            else if (GizmoType::ROTATE == m_gizmoType) ImGui::PopStyleColor();

            ImGui::SameLine();
            if (GizmoType::SCALE == m_gizmoType) ImGui::PushStyleColor(ImGuiCol_Text, selectedTextColor);
            if (ImGui::Button(ICON_MDI_RESIZE, iconSize))
            {
                m_gizmoType = GizmoType::SCALE;
            }
            else if (GizmoType::SCALE == m_gizmoType) ImGui::PopStyleColor();

            ImGui::SameLine();
            if (GizmoType::TRANSFORM == m_gizmoType) ImGui::PushStyleColor(ImGuiCol_Text, selectedTextColor);
            if (ImGui::Button(ICON_MDI_ROTATE_ORBIT, iconSize))
            {
                m_gizmoType = GizmoType::TRANSFORM;
            }
            else if (GizmoType::TRANSFORM == m_gizmoType) ImGui::PopStyleColor();

            ImGui::PopStyleColor(3);
            ImGui::PopStyleVar(2);
        }
        ImGui::PopStyleVar(3);
        ImGui::End();
    }

    void EditorSystem::onGuiStats()
    {
        ImGui::Begin("Stats");
        {
            auto stats = Services::renderer()->getStatistics();
            ImGui::Text("%s\nDriver: %s\nGLSL Version: %s\n\n", stats.rendererName.c_str(),
                        stats.driverVersion.c_str(),
                        stats.glslVersion.c_str());
            ImGui::Text("Frame Rate: %.3f ms/frame (%.1f FPS)", Services::application()->getFramerate(), 1000.0f / Services::application()->getFramerate());
        }
        ImGui::End(); // Stats
    }

    void EditorSystem::onGuiRenderingSettings()
    {
        ImGui::Begin("Rendering Settings");
        {
            ImGui::Checkbox("Visualize Light",     &RenderingSystem::s_VisualizeLight);
            ImGui::Checkbox("Visualize Camera",    &RenderingSystem::s_VisualizeCamera);
            ImGui::Checkbox("Visualize Colliders", &RenderingSystem::s_VisualizePhysicsColliders);

            // Shading mode
            const  char* drawModeItems[]      = { "Shaded", "Wireframe", "Shaded Wireframe" };
            static auto  drawModeCurrentIndex = (int)RenderingSystem::s_ShadingMode;

            if (ImGui::BeginCombo("Draw Mode", drawModeItems[drawModeCurrentIndex]))
            {
                for (int n = 0; n < std::size(drawModeItems); ++n)
                {
                    const bool isSelected = (drawModeCurrentIndex == n);
                    if (ImGui::Selectable(drawModeItems[n], isSelected))
                    {
                        drawModeCurrentIndex = n;
                        RenderingSystem::s_ShadingMode = (ShadingMode)drawModeCurrentIndex;
                    }

                    // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                    if (isSelected) ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }

            // Debug Views
            auto debugViews                                       = Services::renderer()->getDebugViewsMap();
            auto [currentDebugViewName, currentDebugViewTexture ] = Services::renderer()->getCurrentDebugView();

            if (ImGui::BeginCombo("Debug View", currentDebugViewName.c_str()))
            {
                for (auto [name, texture] : debugViews)
                {
                    const bool isSelected = (currentDebugViewName == name);
                    if (ImGui::Selectable(name.c_str(), isSelected))
                    {
                        Services::renderer()->setCurrentDebugView(name);
                    }

                    // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                    if (isSelected) ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }

            // JFA Outline
            ImGui::Checkbox  ("Outline Use Single Axis Method", &Services::renderer()->outlineUseSeparableAxisMethod);
            ImGui::ColorEdit3("Outline Color",                  &Services::renderer()->outlineColor[0]);
            ImGui::DragFloat ("Outline Width",                  &Services::renderer()->outlineWidth, 1.0f, 1.0f, 500.0f);
        }
        ImGui::End();
    }

    // NOTE(TG) Probably should this be a separate panel
    void EditorSystem::onGuiProjectSettings()
    {
        static ImGuiWindowFlags flags = 0;
        ImGui::Begin("Project Settings", nullptr, flags);
        {
            auto& config = Project::getActive()->getConfig();

            if (ImGui::BeginTable("ProjectSettings", 2))
            {
                ImGui::TableSetupColumn("x", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("y", ImGuiTableColumnFlags_WidthStretch);

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("Name");
                
                ImGui::TableSetColumnIndex(1);
                ImGui::BeginDisabled();
                {
                    ImGui::InputText("##ProjectName", config.name.data(), config.name.size());
                }
                ImGui::EndDisabled();

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("Asset Directory");

                ImGui::TableSetColumnIndex(1);
                ImGui::BeginDisabled();
                {
                    ImGui::InputText("##AssetDirectory", &config.assetDirectory.string()[0], config.assetDirectory.string().size());
                }
                ImGui::EndDisabled();

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("Startup Scene");

                ImGui::TableSetColumnIndex(1);
                std::string startSceneName = std::filesystem::path(config.startScene).string();
                static char startSceneNameBuf[128];
                startSceneName.copy(startSceneNameBuf, sizeof(startSceneNameBuf));

                // NOTE(TG) What about the dropdown that lists all available scenes in a project?
                if (ImGui::InputText("##StartupScene", startSceneNameBuf, sizeof(startSceneNameBuf)))
                {
                    flags |= ImGuiWindowFlags_UnsavedDocument;
                    config.startScene = std::string(startSceneNameBuf);
                }

                ImGui::EndTable();
            }

            ImGui::Spacing();
            ImGui::BeginDisabled(!(flags & ImGuiWindowFlags_UnsavedDocument));
            {
                if (ImGui::Utils::ButtonAligned("Save Project Settings"))
                {
                    saveProject();
                    flags = 0;
                }
            }
            ImGui::EndDisabled();
        }
        ImGui::End();
    }

    // NOTE(TG) Probably should be a separate panel
    void EditorSystem::onGuiMangoHub()
    {
        float dpiScale = Services::application()->getWindow()->getDpiScale().x;
        ImGui::SetNextWindowSize({ 900 * dpiScale, 350 * dpiScale });

        auto flags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse;
        if (ImGui::Begin("Primitive Mango Hub", &m_isMangoHubOpen, flags))
        {
            ImGuiStyle& style          = ImGui::GetStyle();
            ImGuiIO&    io             = ImGui::GetIO();
            auto        boldFont       = ImGuiSystem::getFont("InterBold16");
            auto        bigRegularFont = ImGuiSystem::getFont("Inter24");
            
            MG_ASSERT(boldFont);
            MG_ASSERT(bigRegularFont);

            ImVec4 errorMessageColor   = ImVec4(0.8392, 0.2706, 0.2039, 1.0);
            ImVec4 warningMessageColor = ImVec4(0.9490, 0.8863, 0.3882, 1.0);

            bool isError = false;

            if (ImGui::BeginTable("ProjectSettings", 2, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingStretchSame))
            {
                ImGui::TableSetupColumn("x", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableSetupColumn("y", ImGuiTableColumnFlags_WidthStretch);

                // New project
                ImGui::TableNextColumn();
                ImGui::PushFont(bigRegularFont);
                ImGui::Text("New Project");
                ImGui::PopFont();
                ImGui::Dummy({ 0, bigRegularFont->FontSize });

                ImGui::Text("Project Name:");
                ImGui::PushItemWidth(-1);
                static char newProjectName[256];
                ImGui::InputText("##NewProjectName", newProjectName, sizeof(newProjectName));
                ImGui::PopItemWidth();

                ImGui::Text("Path:");

                auto        browseLabel      = "Browse";
                float       browseButtonSize = ImGui::CalcTextSize(browseLabel).x + style.FramePadding.x * 2.0f * 2.0f; // additional times 2 due to same line and padding between widgets
                float       availableWidth   = ImGui::GetContentRegionAvail().x;

                static char newProjectPath[256];
                ImGui::PushItemWidth(availableWidth - browseButtonSize);
                ImGui::InputText("##NewProjectPath", newProjectPath, sizeof(newProjectPath));
                ImGui::PopItemWidth();
                
                ImGui::SameLine();
                
                ImGui::PushID("newProjectBrowseButton");
                if (ImGui::Button(browseLabel))
                {
                    auto folder = pfd::select_folder("Select New Project Directory", "");

                    if (!folder.result().empty())
                    {
                        folder.result().copy(newProjectPath, sizeof(newProjectPath));
                    }
                }
                ImGui::PopID();

                ImGui::Spacing();
                if (std::string(newProjectName).empty())
                {
                    const char* msg = "Project name can't be empty";
                    ImGui::Utils::AlignWidget(msg);
                    ImGui::PushFont(boldFont);
                    ImGui::TextColored(errorMessageColor, msg);
                    ImGui::PopFont();
                    isError = true;
                }
                else if (!std::filesystem::exists(newProjectPath))
                {
                    const char* msg = "The specified path doesn't exist";
                    ImGui::Utils::AlignWidget(msg);
                    ImGui::PushFont(boldFont);
                    ImGui::TextColored(errorMessageColor, msg);
                    ImGui::PopFont();
                    isError = true;
                }
                else if (!std::filesystem::is_directory(newProjectPath))
                {
                    const char* msg = "The specified path is not a directory";
                    ImGui::Utils::AlignWidget(msg);
                    ImGui::PushFont(boldFont);
                    ImGui::TextColored(errorMessageColor, msg);
                    ImGui::PopFont();
                    isError = true;
                }
                else if (std::filesystem::exists(std::filesystem::path(newProjectPath) / newProjectName))
                {
                    const char* msg = "The project with the same name\nalready exists in this directory";
                    ImGui::Utils::AlignWidget(msg);
                    ImGui::PushFont(boldFont);
                    ImGui::TextColored(errorMessageColor, msg);
                    ImGui::PopFont();
                    isError = true;
                }
                else
                {
                    ImGui::Text("");
                }

                ImGui::Dummy({ 0, io.FontDefault->FontSize });
                ImGui::BeginDisabled(isError);
                if (ImGui::Utils::ButtonAligned("Create & Open", 0.5f))
                {
                    auto newProject = Project::createNew(newProjectName, newProjectPath);
                    auto projectFilepath = Project::getActive()->getConfig().name + MG_PROJECT_EXTENSION;
                    if (openProject(std::filesystem::path(newProjectPath) / std::filesystem::path(newProjectName) / projectFilepath))
                    {
                        m_isMangoHubOpen = false;
                    }

                    memset(newProjectName, 0, sizeof(newProjectName));
                    memset(newProjectPath, 0, sizeof(newProjectPath));
                }
                ImGui::EndDisabled();

                // Open project
                isError = false;

                ImGui::TableNextColumn();
                ImGui::PushFont(bigRegularFont);
                ImGui::Text("Open Project");
                ImGui::PopFont();
                ImGui::Dummy({ 0, bigRegularFont->FontSize });

                ImGui::Text("Path:");

                availableWidth = ImGui::GetContentRegionAvail().x;

                static char openProjectPath[256];
                ImGui::PushItemWidth(availableWidth - browseButtonSize);
                ImGui::InputText("##OpenProjectPath", openProjectPath, sizeof(openProjectPath));
                ImGui::PopItemWidth();

                ImGui::SameLine();

                ImGui::PushID("openProjectBrowseButton");
                if (ImGui::Button(browseLabel))
                {
                    auto file = pfd::open_file("", "", { "Mango Project (*" MG_PROJECT_EXTENSION ")", "*" MG_PROJECT_EXTENSION }, false);

                    if (!file.result().empty())
                    {
                        file.result()[0].copy(openProjectPath, sizeof(openProjectPath));
                    }
                }
                ImGui::PopID();

                ImGui::Spacing();
                if (!std::filesystem::exists(openProjectPath))
                {
                    const char* msg = "The specified project doesn't exist";
                    ImGui::Utils::AlignWidget(msg);
                    ImGui::PushFont(boldFont);
                    ImGui::TextColored(errorMessageColor, msg);
                    ImGui::PopFont();
                    isError = true;
                }
                else if (std::filesystem::is_directory(openProjectPath) || 
                         std::filesystem::path(openProjectPath).extension() != MG_PROJECT_EXTENSION)
                {
                    const char* msg = "The specified path is not a Mango project file";
                    ImGui::Utils::AlignWidget(msg);
                    ImGui::PushFont(boldFont);
                    ImGui::TextColored(errorMessageColor, msg);
                    ImGui::PopFont();
                    isError = true;
                }
                else
                {
                    ImGui::Text("");
                }

                ImGui::Dummy({ 0, io.FontDefault->FontSize });
                ImGui::BeginDisabled(isError);
                if (ImGui::Utils::ButtonAligned("Open Project", 0.5f))
                {
                    if (openProject(openProjectPath))
                    {
                        m_isMangoHubOpen = false;
                    }
                    memset(openProjectPath, 0, sizeof(openProjectPath));
                }
                ImGui::EndDisabled();

                // Fill the window with empty space to the bottom
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Dummy({ 0, ImGui::GetContentRegionAvail().y });

                ImGui::EndTable();
            }
        }
        ImGui::End();
    }

    void EditorSystem::beginDockingToolbar(const char* name, ImGuiAxis toolbarAxis, const ImVec2& iconSize)
    {
        // 1. We request auto-sizing on one axis
        // Note however this will only affect the toolbar when NOT docked.
        ImVec2 requestedSize = (toolbarAxis == ImGuiAxis_X) ? ImVec2(-1.0f, 0.0f) : ImVec2(0.0f, -1.0f);
        ImGui::SetNextWindowSize(requestedSize);

        // 2. Specific docking options for toolbars.
        // Currently they add some constraint we ideally wouldn't want, but this is simplifying our first implementation
        ImGuiWindowClass window_class;
        window_class.DockingAllowUnclassed     = true;
        window_class.DockNodeFlagsOverrideSet |= ImGuiDockNodeFlags_NoCloseButton;
        window_class.DockNodeFlagsOverrideSet |= ImGuiDockNodeFlags_HiddenTabBar; // ImGuiDockNodeFlags_NoTabBar // FIXME: Will need a working Undock widget for _NoTabBar to work
        window_class.DockNodeFlagsOverrideSet |= ImGuiDockNodeFlags_NoDockingSplit;
        window_class.DockNodeFlagsOverrideSet |= ImGuiDockNodeFlags_NoDockingOverMe;
        window_class.DockNodeFlagsOverrideSet |= ImGuiDockNodeFlags_NoDockingOverOther;
        if (toolbarAxis == ImGuiAxis_X)
            window_class.DockNodeFlagsOverrideSet |= ImGuiDockNodeFlags_NoResizeY;
        else
            window_class.DockNodeFlagsOverrideSet |= ImGuiDockNodeFlags_NoResizeX;

        ImGui::SetNextWindowClass(&window_class);

        // 3. Begin into the window
        ImGui::Begin(name, NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar);

        // 4. Overwrite node size
        ImGuiDockNode* node = ImGui::GetWindowDockNode();
        if (node != NULL)
        {
            // Overwrite size of the node
                  ImGuiStyle& style                    = ImGui::GetStyle();
            const ImGuiAxis   toolbarAxisPerp          = (ImGuiAxis)(toolbarAxis ^ 1);
            const float       TOOLBAR_SIZE_WHEN_DOCKED = style.WindowPadding[toolbarAxisPerp] * 2.0f + iconSize[toolbarAxisPerp];

            node->WantLockSizeOnce      = true;
            node->Size[toolbarAxisPerp] = node->SizeRef[toolbarAxisPerp] = TOOLBAR_SIZE_WHEN_DOCKED;

            if (node->ParentNode && node->ParentNode->SplitAxis != ImGuiAxis_None)
                toolbarAxis = (ImGuiAxis)(node->ParentNode->SplitAxis ^ 1);
        }
    }

    void EditorSystem::endDockingToolbar()
    {
        ImGui::End();
    }

    void EditorSystem::onDuplicateEntity()
    {
        if (SceneState::Edit != m_sceneState)
        {
            return;
        }

        Entity selectedEntity = SelectionManager::getSelectedEntity();
        if (selectedEntity)
        {
            Entity newEntity = m_editorScene->duplicateEntity(selectedEntity);
            SelectionManager::selectEntity(newEntity);
        }
    }

    void EditorSystem::moveLights(float dt)
    {    
        static float acc = 0.0f;
        acc += dt / 6.0f;
    
        {
            auto view = m_activeScene->getEntitiesWithComponent<TransformComponent, PointLightComponent>();
            for (auto entity : view)
            {
                auto [transform, pointLight] = view.get(entity);

                glm::vec3 delta = transform.getLocalPosition() - glm::vec3(0.0f);

                float r = 8.0f * glm::abs(glm::sin(acc));
                float currentAngle = atan2(delta.z, delta.x);

                auto position = transform.getLocalPosition();
                position.x = r * glm::cos(currentAngle + dt);
                position.y = r * (glm::cos(2.0f * (currentAngle + dt)) * 0.5f + 0.5f) * 0.5f;
                position.z = r * glm::sin(currentAngle + dt);

                transform.setLocalPosition(position);
            }
        }
    
        {
            auto view = m_activeScene->getEntitiesWithComponent<TransformComponent, SpotLightComponent>();
            for (auto entity : view)
            {
                auto [transform, spotLight] = view.get(entity);

                glm::quat previousOrientation = transform.getLocalOrientation();

                transform.setLocalRotation(0.0f, 16.667f * dt, 0.0f);
                transform.setLocalRotation(previousOrientation * transform.getLocalOrientation());
            }
        }
    }

    void EditorSystem::onReceiveSceneLoadEvent(const RequestSceneLoadEvent& event)
    {
        openScene(event.scenePath);
    }

}