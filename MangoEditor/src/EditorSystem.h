#pragma once
#include "Mango.h"
#include "Panels/SceneHierarchyPanel.h"

namespace mango
{
    class EditorSystem : public System
    {
    public:
        EditorSystem();
        ~EditorSystem() = default;

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

        SceneHierarchyPanel   m_sceneHierarchyPanel;

        std::shared_ptr<FreeCameraController> m_freeCameraController;
    };
}