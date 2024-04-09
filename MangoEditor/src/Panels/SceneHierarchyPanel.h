#pragma once

#include "Mango/Core/Base.h"
#include "Mango/Scene/Entity.h"
#include "Mango/Scene/Scene.h"

namespace mango
{
    class SceneHierarchyPanel
    {
    public:
        SceneHierarchyPanel() = default;
        SceneHierarchyPanel(const ref<Scene>& scene);
        
        void setScene(const ref<Scene>& scene);
        void onGui();

    private:
        enum class EcmAction { Create, Delete, None}; // Entity Context Menu Action

    private:
        EcmAction displayEntityContextMenu(bool isClickedEmptySpace);

        template<typename T>
        bool displayAddComponentEntry(const std::string& entryName);

        void drawEntityNode(Entity entity);
        void drawComponents(Entity entity);

    private:
        ref<Scene> m_scene;
        Entity m_selectedEntity;
    };
}