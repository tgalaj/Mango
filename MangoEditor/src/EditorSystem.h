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
        void moveLights(float dt);

    private:
        SceneHierarchyPanel m_sceneHierarchyPanel;

        std::shared_ptr<Scene> m_mainScene;
        std::shared_ptr<FreeCameraController> m_freeCameraController;

        Entity m_camera1;
        Entity m_camera2;
    };
}