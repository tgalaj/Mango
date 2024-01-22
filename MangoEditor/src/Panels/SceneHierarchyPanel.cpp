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

    template<typename ComponentType, typename UIFunction>
    static void drawComponent(const std::string& name, Entity entity, UIFunction uiFunction)
    {
        const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen    | 
                                                 ImGuiTreeNodeFlags_Framed         |
                                                 ImGuiTreeNodeFlags_FramePadding   |
                                                 ImGuiTreeNodeFlags_SpanAvailWidth |
                                                 ImGuiTreeNodeFlags_AllowItemOverlap;

        if (entity.hasComponent<ComponentType>())
        {
            bool opened = ImGui::TreeNodeEx((void*)typeid(ComponentType).hash_code(), treeNodeFlags, name.c_str());

            if (opened)
            {
                auto& component = entity.getComponent<ComponentType>();
                uiFunction(component);
                ImGui::TreePop();
            }
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

        drawComponent<TransformComponent>("Transform", entity, [](auto& transform)
        {
            auto position = transform.getPosition();
            auto rotation = glm::degrees(transform.getRotation());
            auto scale    = transform.getScale();

            if (ImGui::DragFloat3("Position", &position[0], 0.5f))
            {
                transform.setPosition(position);
            }

            if (ImGui::DragFloat3("Rotation", &rotation[0], 0.5f))
            {
                transform.setRotation(rotation);
            }

            if (ImGui::DragFloat3("Scale", &scale[0], 0.5f))
            {
                transform.setScale(scale);
            }
        });

        drawComponent<CameraComponent>("Camera", entity, [](auto& cameraComponent)
        {
            bool isPrimary = cameraComponent.isPrimary();
            
            ImGui::BeginDisabled();
            ImGui::Checkbox("Primary", &isPrimary);
            ImGui::EndDisabled();

            ImGui::SameLine();
            if (ImGui::Button("Set as primary"))
            {
                cameraComponent.setPrimary();
            }

            const char* projectionTypeStrings[] = { "Perspective", "Orthographic" };
            const char* currentProjectionTypeString = projectionTypeStrings[int(cameraComponent.getProjectionType())];

            if (ImGui::BeginCombo("Projection", currentProjectionTypeString))
            {
                for (int i = 0; i < std::size(projectionTypeStrings); ++i)
                {
                    bool isSelected = currentProjectionTypeString == projectionTypeStrings[i];
                    if (ImGui::Selectable(projectionTypeStrings[i], isSelected))
                    {
                        currentProjectionTypeString = projectionTypeStrings[i];
                        cameraComponent.setProjectionType(CameraComponent::ProjectionType(i));
                    }

                    if (isSelected)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }

            if (cameraComponent.getProjectionType() == CameraComponent::ProjectionType::Perspective)
            {
                float verticalFov = glm::degrees(cameraComponent.getPerspectiveVerticalFieldOfView());
                if (ImGui::DragFloat("Vertical FOV", &verticalFov))
                {
                    cameraComponent.setPerspectiveVerticalFieldOfView(verticalFov);
                }

                float nearClip = cameraComponent.getPerspectiveNearClip();
                if (ImGui::DragFloat("Near Clip", &nearClip))
                {
                    cameraComponent.setPerspectiveNearClip(nearClip);
                }

                float farClip = cameraComponent.getPerspectiveFarClip();
                if (ImGui::DragFloat("Far Clip", &farClip))
                {
                    cameraComponent.setPerspectiveFarClip(farClip);
                }
            }

            if (cameraComponent.getProjectionType() == CameraComponent::ProjectionType::Orthographic)
            {
                float orthoSize = cameraComponent.getOrthographicSize();
                if (ImGui::DragFloat("Size", &orthoSize))
                {
                    cameraComponent.setOrthographicSize(orthoSize);
                }

                float nearClip = cameraComponent.getOrthographicNearClip();
                if (ImGui::DragFloat("Near Clip", &nearClip))
                {
                    cameraComponent.setOrthographicNearClip(nearClip);
                }

                float farClip = cameraComponent.getOrthographicFarClip();
                if (ImGui::DragFloat("Far Clip", &farClip))
                {
                    cameraComponent.setOrthographicFarClip(farClip);
                }
            }

        });
    }

}
