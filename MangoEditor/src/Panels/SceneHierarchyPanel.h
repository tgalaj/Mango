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

        Entity getSelectedEntity() const;
        void setSelectedEntity(Entity entity);

    private:
        template<typename T>
        bool displayAddComponentEntry(const std::string& entryName);

        void drawEntityNode(Entity entity);
        void drawComponents(Entity entity);

    private:
        ref<Scene> m_scene;
        Entity m_selectedEntity;
    };
}