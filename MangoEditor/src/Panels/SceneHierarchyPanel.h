#pragma once

#include "Mango/Scene/Entity.h"
#include "Mango/Scene/Scene.h"

namespace mango
{
    class SceneHierarchyPanel
    {
    public:
        SceneHierarchyPanel() = default;
        SceneHierarchyPanel(const std::shared_ptr<Scene>& scene);
        
        void setScene(const std::shared_ptr<Scene>& scene);
        void draw();

        Entity getSelectedEntity() const;
        void setSelectedEntity(Entity entity);

    private:
        template<typename T>
        bool displayAddComponentEntry(const std::string& entryName);

        void drawEntityNode(Entity entity);
        void drawComponents(Entity entity);

    private:
        std::shared_ptr<Scene> m_scene;
        Entity m_selectedEntity;
    };
}