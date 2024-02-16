#pragma once
#include "Mango.h"
#include "EditorCamera.h"
#include "Panels/AssetBrowserPanel.h"
#include "Panels/SceneHierarchyPanel.h"

namespace mango
{
    enum class GizmoType
    {
        TRANSLATE,
        ROTATE,
        SCALE,
        NONE
    };

    enum class GizmoMode
    {
        LOCAL,
        WORLD
    };

    class EditorSystem : public System
    {
    public:
        EditorSystem();
        virtual ~EditorSystem() = default;

        void onInit() override;
        void onDestroy() override;
        void onUpdate(float dt) override;
        void onGui() override;

    private:
        void newProject();
        bool openProject();
        bool openProject(const std::filesystem::path& path);
        void saveProject();

        void newScene();
        void openScene();
        void openScene(const std::filesystem::path& path);
        void saveScene();
        void saveSceneAs();

        void onScenePlay();
        void onSceneSimulate();
        void onSceneStop();

        void onGuiViewport();
        void onGuiToolbar();
        void onGuiStats();
        void onGuiRenderingSettings();
        void onGuiProjectSettings();
        void onGuiMangoHub();

        void beginDockingToolbar(const char* name, ImGuiAxis toolbarAxis, const ImVec2& iconSize);
        void endDockingToolbar();

        void onDuplicateEntity();

        void moveLights(float dt);

    private:
        std::filesystem::path  m_editorScenePath;
        std::shared_ptr<Scene> m_activeScene;
        std::shared_ptr<Scene> m_editorScene;

        std::shared_ptr<Texture> m_playIcon;
        std::shared_ptr<Texture> m_playPressedIcon;
        std::shared_ptr<Texture> m_simulateIcon;
        std::shared_ptr<Texture> m_simulatePressedIcon;
        std::shared_ptr<Texture> m_pauseIcon;
        std::shared_ptr<Texture> m_pausePressedIcon;
        std::shared_ptr<Texture> m_stepIcon;

        bool      m_viewportFocused   = false, 
                  m_viewportHovered   = false;
        glm::vec2 m_viewportSize      = { 0.0f, 0.0f };
        glm::vec2 m_viewportBounds[2] = {{ 0.0f, 0.0f }, { 0.0f, 0.0f }};

        enum class SceneState
        {
            Edit = 0, Play = 1, Simulate = 2
        };
        SceneState m_sceneState = SceneState::Edit;

        GizmoType m_gizmoType = GizmoType::NONE;
        GizmoMode m_gizmoMode = GizmoMode::LOCAL;

        std::unique_ptr<AssetBrowserPanel> m_assetsBrowserPanel;
        SceneHierarchyPanel m_sceneHierarchyPanel;

        EditorCamera m_editorCamera;

        bool m_isMangoHubOpen = false;
    };
}