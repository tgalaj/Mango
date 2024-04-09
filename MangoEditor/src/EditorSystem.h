#pragma once
#include "Mango.h"
#include "EditorCamera.h"
#include "Panels/ContentBrowserPanel.h"
#include "Panels/MaterialEditorPanel.h"
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
        ref<Scene> m_activeScene;
        ref<Scene> m_editorScene;

        ref<Texture> m_playIcon;
        ref<Texture> m_playPressedIcon;
        ref<Texture> m_simulateIcon;
        ref<Texture> m_simulatePressedIcon;
        ref<Texture> m_pauseIcon;
        ref<Texture> m_pausePressedIcon;
        ref<Texture> m_stepIcon;

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

        scope<ContentBrowserPanel> m_assetsBrowserPanel;
        MaterialEditorPanel m_materialEditorPanel;
        SceneHierarchyPanel m_sceneHierarchyPanel;

        EditorCamera m_editorCamera;

        bool m_isMangoHubOpen = false;
    };
}