#include "SceneHierarchyPanel.h"

#include "Mango/Scene/Components.h"

#include <imgui.h>
#include <imgui_internal.h>

#include <type_traits>

namespace mango
{
    SceneHierarchyPanel::SceneHierarchyPanel(const std::shared_ptr<Scene>& scene)
        : m_scene(scene),
          m_selectedEntity{}
    {
    }

    void SceneHierarchyPanel::setScene(const std::shared_ptr<Scene>& scene)
    {
        m_scene          = scene;
        m_selectedEntity = {};
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
                drawComponents(m_selectedEntity);

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

    template<typename ComponentType>
    bool SceneHierarchyPanel::displayAddComponentEntry(const std::string& entryName)
    {
        if (!m_selectedEntity.hasComponent<ComponentType>())
        {
            if (ImGui::MenuItem(entryName.c_str()))
            {
                m_selectedEntity.addComponent<ComponentType>();
                ImGui::CloseCurrentPopup();
            }
            return true;
        }
        return false;
    }

    void SceneHierarchyPanel::drawEntityNode(Entity entity)
    {
        auto& tag = entity.getComponent<TagComponent>().tag;

        ImGuiTreeNodeFlags flags  = ((m_selectedEntity == entity) ? ImGuiTreeNodeFlags_Selected : 0);
                           flags |= ImGuiTreeNodeFlags_OpenOnArrow;
                           flags |= ImGuiTreeNodeFlags_OpenOnDoubleClick;
                           flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
        bool opened     = ImGui::TreeNodeEx((void*)(uint64_t)(entity.getUUID()), flags, tag.c_str());
        bool isSelected = flags & ImGuiTreeNodeFlags_Selected;

        if (ImGui::IsItemClicked())
        {
            m_selectedEntity = entity;
        }

        // Right click on empty space
        bool isEntityDeleted = false;
        if (ImGui::BeginPopupContextItem())
        {
            if (ImGui::MenuItem("Delete Entity"))
            {
                isEntityDeleted = true;
            }
            ImGui::EndPopup();
        }
        
        if (isSelected && ImGui::IsItemFocused() && !isEntityDeleted && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Delete)))
        {
            isEntityDeleted = true;
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

        if (isEntityDeleted)
        {
            m_scene->destroyEntity(entity);

            if (m_selectedEntity == entity)
            {
                m_selectedEntity = {};
            }
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
            ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();

            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
            float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;

            bool opened = ImGui::TreeNodeEx((void*)typeid(ComponentType).hash_code(), treeNodeFlags, name.c_str());
            ImGui::PopStyleVar();

            if (!std::is_same_v<ComponentType, TransformComponent>)
            {
                ImGui::SameLine(contentRegionAvailable.x - (lineHeight + 3.0f) * 0.5f);

                if (ImGui::Button("...", ImVec2{lineHeight + 3.0f, lineHeight}))
                {
                    ImGui::OpenPopup("ComponentSettings");
                }
            }

            float removeComponent = false;
            if (ImGui::BeginPopup("ComponentSettings"))
            {
                if(ImGui::MenuItem("Remove component"))
                    removeComponent = true;

                ImGui::EndPopup();
            }

            if (opened)
            {
                auto& component = entity.getComponent<ComponentType>();
                uiFunction(component);
                ImGui::TreePop();
            }

            if (removeComponent)
            {
                entity.removeComponent<ComponentType>();
            }

            ImGui::Spacing();
        }
    }

    static bool customDragFloat3(const std::string& label, glm::vec3& values, float defaultValue = 0.0f, float columnWidth = 100.0f)
    {
        ImGuiIO& io = ImGui::GetIO();
        auto boldFont = io.Fonts->Fonts[0];

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
        ImGui::PushFont(boldFont);

        if (ImGui::Button("X", buttonSize))
        {
            values.x = defaultValue;
            ret |= true;
        }

        ImGui::PopFont();
        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        ret |= ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
        ImGui::PopItemWidth();
        ImGui::SameLine();

        ImGui::PushStyleColor(ImGuiCol_Button,        { 0.2f, 0.7f, 0.2f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0.3f, 0.8f, 0.3f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonActive,  { 0.2f, 0.7f, 0.2f, 1.0f });
        ImGui::PushFont(boldFont);

        if (ImGui::Button("Y", buttonSize))
        {
            values.y = defaultValue;
            ret |= true;
        }

        ImGui::PopFont();
        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        ret |= ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
        ImGui::PopItemWidth();
        ImGui::SameLine();

        ImGui::PushStyleColor(ImGuiCol_Button,        { 0.1f, 0.25f, 0.8f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0.2f, 0.35f, 0.9f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonActive,  { 0.1f, 0.25f, 0.8f, 1.0f });
        ImGui::PushFont(boldFont);

        if (ImGui::Button("Z", buttonSize))
        {
            values.z = defaultValue;
            ret |= true;
        }

        ImGui::PopFont();
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

            if (ImGui::InputText("##Tag", buffer, sizeof(buffer)))
            {
                tag = std::string(buffer);
            }
        }

        ImGui::SameLine();
        ImGui::PushItemWidth(-1);

        if (ImGui::Button("Add Component"))
            ImGui::OpenPopup("AddComponent");

        if (ImGui::BeginPopup("AddComponent"))
        {
            bool shouldClosePopup = false;

            // TODO: add missing components
            shouldClosePopup |= !displayAddComponentEntry<CameraComponent>("Camera");
            shouldClosePopup |= !displayAddComponentEntry<DirectionalLightComponent>("Directional Light");
            shouldClosePopup |= !displayAddComponentEntry<PointLightComponent>("Point Light");
            shouldClosePopup |= !displayAddComponentEntry<SpotLightComponent>("Spot Light");
            shouldClosePopup |= !displayAddComponentEntry<ModelRendererComponent>("Model Renderer");
            shouldClosePopup |= !displayAddComponentEntry<RigidBody3DComponent>("Rigidbody 3D");
            shouldClosePopup |= !displayAddComponentEntry<BoxCollider3DComponent>("Box Collider 3D");
            shouldClosePopup |= !displayAddComponentEntry<SphereColliderComponent>("Sphere Collider");

            //if (shouldClosePopup)
            //    ImGui::CloseCurrentPopup();

            ImGui::EndPopup();
        }

        ImGui::PopItemWidth();

        drawComponent<TransformComponent>("Transform", entity, [](auto& component)
        {
            auto position = component.getPosition();
            auto rotation = glm::degrees(component.getRotation());
            auto scale    = component.getScale();

            if (customDragFloat3("Position", position))
            {
                component.setPosition(position);
            }
            
            if (customDragFloat3("Rotation", rotation))
            {
                component.setRotation(rotation);
            }

            if (customDragFloat3("Scale", scale, 1.0f))
            {
                component.setScale(scale);
            }
        });

        drawComponent<DirectionalLightComponent>("Directional Light", entity, [](auto& component)
        {
            ImGui::ColorEdit3("Color", &component.color[0]);
            
            if (ImGui::DragFloat("Intensity", &component.intensity, 0.01f, 0.0f, 0.0f, "%.2f"));

            float size = component.getSize();
            if (ImGui::DragFloat("Size", &size))
            {
                component.setSize(size);
            }
            
            bool castsShadows = component.getCastsShadows();
            if (ImGui::Checkbox("Casts shadows", &castsShadows))
            {
                component.setCastsShadows(castsShadows);
            }
        });

        drawComponent<PointLightComponent>("Point Light", entity, [](auto& component)
        {
            ImGui::ColorEdit3("Color", &component.color[0]);
            
            if (ImGui::DragFloat("Intensity", &component.intensity, 0.01f, 0.0f, 0.0f, "%.2f"));

            ImGui::Text("Attenuation");
            Attenuation attenuation = component.getAttenuation();

            if (ImGui::DragFloat("Constant", &attenuation.constant))
            {
                component.setAttenuation(attenuation.constant, attenuation.linear, attenuation.quadratic);
            }

            if (ImGui::DragFloat("Linear", &attenuation.linear))
            {
                component.setAttenuation(attenuation.constant, attenuation.linear, attenuation.quadratic);
            }

            if (ImGui::DragFloat("Quadratic", &attenuation.quadratic))
            {
                component.setAttenuation(attenuation.constant, attenuation.linear, attenuation.quadratic);
            }
            
            float shadowNearPlane = component.getShadowNearPlane();
            if (ImGui::DragFloat("Shadow Near Plane", &shadowNearPlane))
            {
                component.setShadowNearPlane(shadowNearPlane);
            }

            float shadowFarPlane = component.getShadowFarPlane();
            if (ImGui::DragFloat("Shadow Far Plane", &shadowFarPlane))
            {
                component.setShadowFarPlane(shadowFarPlane);
            }

            bool castsShadows = component.getCastsShadows();
            if (ImGui::Checkbox("Casts shadows", &castsShadows))
            {
                component.setCastsShadows(castsShadows);
            }
        });

        drawComponent<SpotLightComponent>("Spot Light", entity, [](auto& component)
        {
            ImGui::ColorEdit3("Color", &component.color[0]);
            
            if (ImGui::DragFloat("Intensity", &component.intensity, 0.01f, 0.0f, 0.0f, "%.2f"));

            float cutoffAngle = glm::degrees(component.getCutOffAngle());
            if (ImGui::DragFloat("Cut-off Angle", &cutoffAngle))
            {
                component.setCutOffAngle(cutoffAngle);
            }

            ImGui::Text("Attenuation");
            Attenuation attenuation = component.getAttenuation();

            if (ImGui::DragFloat("Constant", &attenuation.constant))
            {
                component.setAttenuation(attenuation.constant, attenuation.linear, attenuation.quadratic);
            }

            if (ImGui::DragFloat("Linear", &attenuation.linear))
            {
                component.setAttenuation(attenuation.constant, attenuation.linear, attenuation.quadratic);
            }

            if (ImGui::DragFloat("Quadratic", &attenuation.quadratic))
            {
                component.setAttenuation(attenuation.constant, attenuation.linear, attenuation.quadratic);
            }
            
            float shadowNearPlane = component.getShadowNearPlane();
            if (ImGui::DragFloat("Shadow Near Plane", &shadowNearPlane))
            {
                component.setShadowNearPlane(shadowNearPlane);
            }

            float shadowFarPlane = component.getShadowFarPlane();
            if (ImGui::DragFloat("Shadow Far Plane", &shadowFarPlane))
            {
                component.setShadowFarPlane(shadowFarPlane);
            }

            bool castsShadows = component.getCastsShadows();
            if (ImGui::Checkbox("Casts shadows", &castsShadows))
            {
                component.setCastsShadows(castsShadows);
            }
        });

        drawComponent<CameraComponent>("Camera", entity, [](auto& component)
        {
            bool isPrimary = component.isPrimary();
            
            ImGui::BeginDisabled();
            ImGui::Checkbox("Primary", &isPrimary);
            ImGui::EndDisabled();

            ImGui::SameLine();
            if (ImGui::Button("Set as primary"))
            {
                component.setPrimary();
            }

            const char* projectionTypeStrings[] = { "Perspective", "Orthographic" };
            const char* currentProjectionTypeString = projectionTypeStrings[int(component.getProjectionType())];

            if (ImGui::BeginCombo("Projection", currentProjectionTypeString))
            {
                for (int i = 0; i < std::size(projectionTypeStrings); ++i)
                {
                    bool isSelected = currentProjectionTypeString == projectionTypeStrings[i];
                    if (ImGui::Selectable(projectionTypeStrings[i], isSelected))
                    {
                        currentProjectionTypeString = projectionTypeStrings[i];
                        component.setProjectionType(CameraComponent::ProjectionType(i));
                    }

                    if (isSelected)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }

            if (component.getProjectionType() == CameraComponent::ProjectionType::Perspective)
            {
                float verticalFov = glm::degrees(component.getPerspectiveVerticalFieldOfView());
                if (ImGui::DragFloat("Vertical FOV", &verticalFov))
                {
                    component.setPerspectiveVerticalFieldOfView(verticalFov);
                }

                float nearClip = component.getPerspectiveNearClip();
                if (ImGui::DragFloat("Near Clip", &nearClip))
                {
                    component.setPerspectiveNearClip(nearClip);
                }

                float farClip = component.getPerspectiveFarClip();
                if (ImGui::DragFloat("Far Clip", &farClip))
                {
                    component.setPerspectiveFarClip(farClip);
                }
            }

            if (component.getProjectionType() == CameraComponent::ProjectionType::Orthographic)
            {
                float orthoSize = component.getOrthographicSize();
                if (ImGui::DragFloat("Size", &orthoSize))
                {
                    component.setOrthographicSize(orthoSize);
                }

                float nearClip = component.getOrthographicNearClip();
                if (ImGui::DragFloat("Near Clip", &nearClip))
                {
                    component.setOrthographicNearClip(nearClip);
                }

                float farClip = component.getOrthographicFarClip();
                if (ImGui::DragFloat("Far Clip", &farClip))
                {
                    component.setOrthographicFarClip(farClip);
                }
            }

        });

        drawComponent<RigidBody3DComponent>("Rigidbody 3D", entity, [](auto& component)
        {
            const char* motionTypeStrings[] = { "Static", "Kinematic", "Dynamic"};
            const char* currentMotionType   = motionTypeStrings[int(component.motionType)];

            if (ImGui::BeginCombo("Motion Type", currentMotionType))
            {
                for (int i = 0; i < std::size(motionTypeStrings); ++i)
                {
                    bool isSelected = currentMotionType == motionTypeStrings[i];
                    if (ImGui::Selectable(motionTypeStrings[i], isSelected))
                    {
                        currentMotionType = motionTypeStrings[i];
                        component.motionType = RigidBody3DComponent::MotionType(i);
                    }

                    if (isSelected)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }

            ImGui::DragFloat("Friction",        &component.friction,       0.01f, 0.0f, 1.0f);
            ImGui::DragFloat("Restitution",     &component.restitution,    0.01f, 0.0f, 1.0f);
            ImGui::DragFloat("Linear Damping",  &component.linearDamping,  0.01f, 0.0f, 1.0f);
            ImGui::DragFloat("Angular Damping", &component.angularDamping, 0.01f, 0.0f, 1.0f);

            ImGui::Checkbox("Activated",       &component.isInitiallyActivated);
        });

        drawComponent<BoxCollider3DComponent>("Box Collider 3D", entity, [](auto& component)
        {
            ImGui::DragFloat3("Offset", &component.offset[0]);
            ImGui::DragFloat3("Extent", &component.halfExtent[0]);
        });

        drawComponent<SphereColliderComponent>("Sphere Collider", entity, [](auto& component)
        {
            ImGui::DragFloat3("Offset", &component.offset[0]);
            ImGui::DragFloat ("Radius", &component.radius);
        });
    }

}
