#pragma once
#include "Mango.h"
#include "EditorCamera.h"
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
        void newScene();
        void openScene();
        void openScene(const std::filesystem::path& path);
        void saveScene();
        void saveSceneAs();

        void moveLights(float dt);

    private:
        std::filesystem::path  m_editorScenePath;
        std::shared_ptr<Scene> m_mainScene;

        bool      m_viewportFocused   = false, 
                  m_viewportHovered   = false;
        glm::vec2 m_viewportSize      = { 0.0f, 0.0f };
        glm::vec2 m_viewportBounds[2] = {{ 0.0f, 0.0f }, { 0.0f, 0.0f }};

        GizmoType m_gizmoType = GizmoType::NONE;
        GizmoMode m_gizmoMode = GizmoMode::LOCAL;

        SceneHierarchyPanel m_sceneHierarchyPanel;

        EditorCamera m_editorCamera;
    };
}