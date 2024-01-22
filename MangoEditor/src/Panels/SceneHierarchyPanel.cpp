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

            // Right click on empty space
            if (ImGui::BeginPopupContextWindow(0, ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems))
            {
                if (ImGui::MenuItem("Create Empty Entity"))
                {
                    m_scene->createEntity("Empty Entity");
                }
                ImGui::EndPopup();
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

    static bool customDragFloat3(const std::string& label, glm::vec3& values, float defaultValue = 0.0f, float columnWidth = 100.0f)
    {
        bool ret = false;

        ImGui::PushID(label.c_str());

        ImGui::Columns(2);
        ImGui::SetColumnWidth(0, columnWidth);
        ImGui::Text(label.c_str());
        ImGui::NextColumn();

        ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

        float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
        ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

        ImGui::PushStyleColor(ImGuiCol_Button,        { 0.8f, 0.1f, 0.15f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0.9f, 0.2f, 0.25f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonActive,  { 0.8f, 0.1f, 0.15f, 1.0f });
        if (ImGui::Button("X", buttonSize)) 
        {
            values.x = defaultValue;
            ret |= true;
        }
        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        ret |= ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
        ImGui::PopItemWidth();
        ImGui::SameLine();

        ImGui::PushStyleColor(ImGuiCol_Button,        { 0.2f, 0.7f, 0.2f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0.3f, 0.8f, 0.3f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonActive,  { 0.2f, 0.7f, 0.2f, 1.0f });
        if (ImGui::Button("Y", buttonSize))
        {
            values.y = defaultValue;
            ret |= true;
        }
        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        ret |= ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
        ImGui::PopItemWidth();
        ImGui::SameLine();

        ImGui::PushStyleColor(ImGuiCol_Button,        { 0.1f, 0.25f, 0.8f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0.2f, 0.35f, 0.9f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonActive,  { 0.1f, 0.25f, 0.8f, 1.0f });
        if (ImGui::Button("Z", buttonSize))
        {
            values.z = defaultValue;
            ret |= true;
        }
        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        ret |= ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
        ImGui::PopItemWidth();

        ImGui::PopStyleVar();
        ImGui::Columns(1);

        ImGui::PopID();

        return ret;
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

            if (customDragFloat3("Position", position))
            {
                transform.setPosition(position);
            }
            
            if (customDragFloat3("Rotation", rotation))
            {
                transform.setRotation(rotation);
            }

            if (customDragFloat3("Scale", scale, 1.0f))
            {
                transform.setScale(scale);
            }

            ImGui::Spacing();
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
