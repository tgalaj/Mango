#include "SceneHierarchyPanel.h"

#include "Mango/Scene/Components.h"

#include <imgui.h>
#include <imgui_internal.h>

namespace mango
{
    SceneHierarchyPanel::SceneHierarchyPanel(const std::shared_ptr<Scene>& scene)
        : m_scene(scene) 
    {
    }

    void SceneHierarchyPanel::setScene(const std::shared_ptr<Scene>& scene)
    {
        m_scene = scene;
    }

    void SceneHierarchyPanel::draw()
    {
        if (ImGui::Begin("Hierarchy Panel"))
        {
            if (m_scene)
            {
                m_scene->m_registry.each([&](auto entityID)
                    {
                        Entity entity{ entityID, m_scene.get() };
                        drawEntityNode(entity);
                    });
            }

            if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && ImGui::IsWindowHovered())
            {
                m_selectedEntity = {};
            }

            ImGui::End();
        }

        if (ImGui::Begin("Properties"))
        {
            if (m_selectedEntity)
            {
                drawComponents(m_selectedEntity);
            }
            ImGui::End();
        }
    }

    Entity SceneHierarchyPanel::getSelectedEntity() const
    {
        return m_selectedEntity;
    }

    void SceneHierarchyPanel::setSelectedEntity(Entity entity)
    {
        m_selectedEntity = entity;
    }

    void SceneHierarchyPanel::drawEntityNode(Entity entity)
    {
        auto& tag = entity.getComponent<TagComponent>().tag;
        
        ImGuiTreeNodeFlags flags  = ((m_selectedEntity == entity) ? ImGuiTreeNodeFlags_Selected : 0);
                           flags |= ImGuiTreeNodeFlags_OpenOnArrow;
                           flags |= ImGuiTreeNodeFlags_OpenOnDoubleClick;
                           flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
        bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tag.c_str());
        
        if (ImGui::IsItemClicked())
        {
            m_selectedEntity = entity;
        }

        if (opened)
        {
            // TODO: display child nodes
            ImGuiTreeNodeFlags flags  = ImGuiTreeNodeFlags_OpenOnArrow;
                               flags |= ImGuiTreeNodeFlags_OpenOnDoubleClick;
                               flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
            bool opened = ImGui::TreeNodeEx((void*)34564574566, flags, tag.c_str());

            if (opened)
            {
                ImGui::TreePop();
            }

            ImGui::TreePop();
        }
    }

    void SceneHierarchyPanel::drawComponents(Entity entity)
    {
        if (entity.hasComponent<TagComponent>())
        {
            auto& tag = entity.getComponent<TagComponent>().tag;

            char buffer[256];
            memset(buffer, 0, sizeof(buffer));
            strcpy_s(buffer, sizeof(buffer), tag.c_str());

            if (ImGui::InputText("Tag", buffer, sizeof(buffer)))
            {
                tag = std::string(buffer);
            }
        }

        if (entity.hasComponent<TransformComponent>())
        {
            if (ImGui::TreeNodeEx((void*)typeid(TransformComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Transform"))
            {
                auto& transform = entity.getComponent<TransformComponent>();

                auto position = transform.getPosition();
                auto rotation = transform.getOrientation();
                auto scale    = transform.getScale();

                if (ImGui::DragFloat3("Position", &position[0], 0.5f))
                {
                    transform.setPosition(position);
                }

                if (ImGui::DragFloat4("Rotation", &rotation[0], 0.5f))
                {
                    transform.setOrientation(rotation);
                }

                if (ImGui::DragFloat3("Scale", &scale[0], 0.5f))
                {
                    transform.setScale(scale);
                }

                ImGui::TreePop();
            }
        }
    }

}
