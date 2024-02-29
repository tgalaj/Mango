#include "SceneHierarchyPanel.h"

#include "Mango/Core/AssetManager.h"
#include "Mango/Core/VFI.h"
#include "Mango/Project/Project.h"
#include "Mango/Scene/Components.h"
#include "Mango/Systems/ImGuiSystem.h"

#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>
#include <imgui_internal.h>
#include <misc/cpp/imgui_stdlib.h>
#include <strutil.h>

#include <type_traits>

namespace mango
{
    SceneHierarchyPanel::SceneHierarchyPanel(const ref<Scene>& scene)
        : m_scene(scene)
    {
    }

    void SceneHierarchyPanel::setScene(const ref<Scene>& scene)
    {
        m_scene          = scene;
        m_selectedEntity = {};
    }

    void SceneHierarchyPanel::onGui()
    {
        ImGui::Begin("Scene Hierarchy");
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
        }
        ImGui::End();

        ImGui::Begin("Properties");
        {
            if (m_selectedEntity) drawComponents(m_selectedEntity);
        }
        ImGui::End();
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
        auto& name = entity.getComponent<TagComponent>().name;

        ImGuiTreeNodeFlags flags  = ((m_selectedEntity == entity) ? ImGuiTreeNodeFlags_Selected : 0);
                           flags |= ImGuiTreeNodeFlags_OpenOnArrow;
                           flags |= ImGuiTreeNodeFlags_OpenOnDoubleClick;
                           flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
        bool opened     = ImGui::TreeNodeEx((void*)(uint64_t)(entity.getUUID()), flags, name.c_str());
        bool isSelected = flags & ImGuiTreeNodeFlags_Selected;

        // Mark the entity as selected:
        // - on mouse click
        // - when tree is traversed with the keyboard arrow keys (isHovered && isFocused)
        if (ImGui::IsItemClicked() || (ImGui::IsItemHovered() && ImGui::IsItemFocused()))
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
            bool opened = ImGui::TreeNodeEx((void*)34564574566, flags, name.c_str());

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

    static bool customDragFloat3(const std::string& label, glm::vec3& values, float defaultValue = 0.0f, float columnWidth = 80.0f)
    {
        ImGuiIO& io       = ImGui::GetIO();
        auto     boldFont = ImGuiSystem::getFont("InterBold16");

        MG_ASSERT(boldFont);

        bool ret = false;

        ImGui::PushID(label.c_str());

        // TODO: write func. to draw each component using Tables API (each component creates it's own table)
        if (ImGui::BeginTable("CustomDragFloat3", 2, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_Borders))
        {
            ImGui::TableSetupColumn("x", ImGuiTableColumnFlags_WidthFixed, columnWidth);
            ImGui::TableSetupColumn("y", ImGuiTableColumnFlags_WidthStretch);

            ImGui::TableNextColumn();
            ImGui::Text(label.c_str());

            ImGui::TableNextColumn();

            ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth() * 1.1f);
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
            ImGui::EndTable();
        }

        ImGui::PopID();

        return ret;
    }

    void SceneHierarchyPanel::drawComponents(Entity entity)
    {
        if (entity.hasComponent<TagComponent>())
        {
            auto& name = entity.getComponent<TagComponent>().name;

            char buffer[256];
            memset(buffer, 0, sizeof(buffer));
            strcpy_s(buffer, sizeof(buffer), name.c_str());

            if (ImGui::InputText("##Name", buffer, sizeof(buffer)))
            {
                name = std::string(buffer);
            }
        }

        ImGui::SameLine();
        ImGui::PushItemWidth(-1);

        if (ImGui::Button("Add Component"))
            ImGui::OpenPopup("AddComponent");

        if (ImGui::BeginPopup("AddComponent"))
        {
            displayAddComponentEntry<CameraComponent>("Camera");
            displayAddComponentEntry<DirectionalLightComponent>("Directional Light");
            displayAddComponentEntry<PointLightComponent>("Point Light");
            displayAddComponentEntry<SpotLightComponent>("Spot Light");
            displayAddComponentEntry<StaticMeshComponent>("Static Mesh");
            displayAddComponentEntry<RigidBody3DComponent>("Rigidbody 3D");
            displayAddComponentEntry<BoxCollider3DComponent>("Box Collider 3D");
            displayAddComponentEntry<SphereColliderComponent>("Sphere Collider");

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
                component.setRotation(glm::radians(rotation));
            }

            if (customDragFloat3("Scale", scale, 1.0f))
            {
                component.setScale(scale);
            }
        });

        drawComponent<DirectionalLightComponent>("Directional Light", entity, [](auto& component)
        {
            ImGui::ColorEdit3("Color",     &component.color[0]);
            ImGui::DragFloat ("Intensity", &component.intensity, 0.1f, 0.0f, FLT_MAX, "%.2f", ImGuiSliderFlags_AlwaysClamp);

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
            ImGui::ColorEdit3("Color",     &component.color[0]);
            ImGui::DragFloat ("Intensity", &component.intensity, 0.1f, 0.0f, FLT_MAX, "%.2f", ImGuiSliderFlags_AlwaysClamp);

            ImGui::Text("Attenuation");
            Attenuation attenuation = component.getAttenuation();

            if (ImGui::DragFloat("Constant", &attenuation.constant, 0.01f, 0.0f, FLT_MAX, "%.2f", ImGuiSliderFlags_AlwaysClamp))
            {
                component.setAttenuation(attenuation.constant, attenuation.linear, attenuation.quadratic);
            }

            if (ImGui::DragFloat("Linear", &attenuation.linear, 0.01f, 0.0f, FLT_MAX, "%.2f", ImGuiSliderFlags_AlwaysClamp))
            {
                component.setAttenuation(attenuation.constant, attenuation.linear, attenuation.quadratic);
            }

            if (ImGui::DragFloat("Quadratic", &attenuation.quadratic, 0.01f, 0.0f, FLT_MAX, "%.2f", ImGuiSliderFlags_AlwaysClamp))
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
            ImGui::ColorEdit3("Color",     &component.color[0]);
            ImGui::DragFloat ("Intensity", &component.intensity, 0.1f, 0.0f, FLT_MAX, "%.2f", ImGuiSliderFlags_AlwaysClamp);

            float cutoffAngle = glm::degrees(component.getCutOffAngle());
            if (ImGui::DragFloat("Cut-off Angle", &cutoffAngle, 0.01f, 0.0f, 89.99999f, "%.2f", ImGuiSliderFlags_AlwaysClamp))
            {
                component.setCutOffAngle(cutoffAngle);
            }

            ImGui::Text("Attenuation");
            Attenuation attenuation = component.getAttenuation();

            if (ImGui::DragFloat("Constant", &attenuation.constant, 0.01f, 0.0f, FLT_MAX, "%.2f", ImGuiSliderFlags_AlwaysClamp))
            {
                component.setAttenuation(attenuation.constant, attenuation.linear, attenuation.quadratic);
            }

            if (ImGui::DragFloat("Linear", &attenuation.linear, 0.01f, 0.0f, FLT_MAX, "%.2f", ImGuiSliderFlags_AlwaysClamp))
            {
                component.setAttenuation(attenuation.constant, attenuation.linear, attenuation.quadratic);
            }

            if (ImGui::DragFloat("Quadratic", &attenuation.quadratic, 0.01f, 0.0f, FLT_MAX, "%.2f", ImGuiSliderFlags_AlwaysClamp))
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
            auto& camera = component.camera;
            ImGui::Checkbox("Primary", &component.isPrimary);

            const char* projectionTypeStrings[] = { "Perspective", "Orthographic" };
            const char* currentProjectionTypeString = projectionTypeStrings[int(camera.getProjectionType())];

            if (ImGui::BeginCombo("Projection", currentProjectionTypeString))
            {
                for (int i = 0; i < std::size(projectionTypeStrings); ++i)
                {
                    bool isSelected = currentProjectionTypeString == projectionTypeStrings[i];
                    if (ImGui::Selectable(projectionTypeStrings[i], isSelected))
                    {
                        currentProjectionTypeString = projectionTypeStrings[i];
                        camera.setProjectionType(Camera::ProjectionType(i));
                    }

                    if (isSelected)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }

            if (camera.getProjectionType() == Camera::ProjectionType::Perspective)
            {
                float verticalFov = glm::degrees(camera.getPerspectiveVerticalFieldOfView());
                if (ImGui::DragFloat("Vertical FOV", &verticalFov, 0.01f, 0.0f, 89.9999f, "%.2f", ImGuiSliderFlags_AlwaysClamp))
                {
                    camera.setPerspectiveVerticalFieldOfView(glm::radians(verticalFov));
                }

                float nearClip = camera.getPerspectiveNearClip();
                if (ImGui::DragFloat("Near Clip", &nearClip))
                {
                    camera.setPerspectiveNearClip(nearClip);
                }

                float farClip = camera.getPerspectiveFarClip();
                if (ImGui::DragFloat("Far Clip", &farClip))
                {
                    camera.setPerspectiveFarClip(farClip);
                }
            }

            if (camera.getProjectionType() == Camera::ProjectionType::Orthographic)
            {
                float orthoSize = camera.getOrthographicSize();
                if (ImGui::DragFloat("Size", &orthoSize))
                {
                    camera.setOrthographicSize(orthoSize);
                }

                float nearClip = camera.getOrthographicNearClip();
                if (ImGui::DragFloat("Near Clip", &nearClip))
                {
                    camera.setOrthographicNearClip(nearClip);
                }

                float farClip = camera.getOrthographicFarClip();
                if (ImGui::DragFloat("Far Clip", &farClip))
                {
                    camera.setOrthographicFarClip(farClip);
                }
            }

        });

        drawComponent<StaticMeshComponent>("STATIC MESH", entity, [&entity](auto& component)
        {
            auto&       mesh  = component.mesh;
            std::string meshLabel = mesh ? mesh->getName() : "NULL";

            ImGui::Text("Mesh");
            ImGui::SameLine();
            
            static std::string searchPattern = "";
            if (ImGui::Button(meshLabel.c_str()))
            {
                ImGui::OpenPopup("mesh_select_popup");
                searchPattern = "";
            }

            auto popupLambda = [](const char* popupName, auto& component, std::function<void(void)> func)
            {
                if (ImGui::BeginPopup(popupName))
                {
                    ImGui::InputText("##search_pattern", &searchPattern, ImGuiInputTextFlags_EscapeClearsAll);

                    auto availWidth = ImGui::GetContentRegionAvail().x;
                    if (ImGui::Button("CLEAR", ImVec2(availWidth, ImGui::GetFrameHeight()))) searchPattern = "";

                    ImGui::BeginChild("##item_list", ImVec2(availWidth, ImGui::GetFrameHeight() * 7.0f));
                    
                    func();

                    ImGui::EndChild();
                    ImGui::EndPopup();
                }
            };

            popupLambda("mesh_select_popup", component, [&component]()
            {
                // 1. get list of all loaded meshes from AssetManager and list them
                // 2. if item selected -> change the mesh for the component.mesh
                for (auto [name, staticMesh] : AssetManager::getStaticMeshList())
                {
                    auto rx = std::regex(searchPattern, std::regex_constants::icase);
                    if (std::regex_search(name, rx))
                    {
                        if (ImGui::Selectable(name.c_str()))
                        {
                            component.mesh      = staticMesh;
                            component.materials = staticMesh->getMaterials();
                            ImGui::CloseCurrentPopup();
                        }
                    }
                }
            });

            const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen    |
                                                     ImGuiTreeNodeFlags_Framed         |
                                                     ImGuiTreeNodeFlags_FramePadding   |
                                                     ImGuiTreeNodeFlags_SpanAvailWidth |
                                                     ImGuiTreeNodeFlags_AllowItemOverlap;

            static int32_t selectedMaterialIndex = -1;

            ImGui::Unindent();
            if (ImGui::TreeNodeEx("Materials", treeNodeFlags))
            {
                for (uint32_t i = 0; i < component.materials.size(); ++i)
                {
                    std::string materialLabel = component.materials[i] ? component.materials[i]->name : "NULL";

                    ImGui::Text("[Material %d]", i);
                    ImGui::SameLine();
                    
                    ImGui::PushID(i);
                    if (ImGui::Button(materialLabel.c_str()))
                    {
                        ImGui::OpenPopup("material_select_popup");
                        searchPattern = "";
                        selectedMaterialIndex = i;
                    }
                    ImGui::PopID();
                }

                ImGui::PushID(selectedMaterialIndex);
                popupLambda("material_select_popup", component, [&component]()
                {
                    // 1. get list of all loaded materials from AssetManager and list them
                    // 2. if item selected -> change the material for the component
                    int32_t id = 0;
                    for (auto [name, material] : AssetManager::getMaterialList())
                    {
                        auto rx = std::regex(searchPattern, std::regex_constants::icase);
                        if (std::regex_search(name, rx))
                        {
                            if (ImGui::Selectable(name.c_str()))
                            {
                                component.materials[selectedMaterialIndex] = material;
                                selectedMaterialIndex = -1;
                                ImGui::CloseCurrentPopup();
                            }
                        }
                    }
                });
                ImGui::PopID();
                ImGui::TreePop();
            }
            ImGui::Indent();
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

            ImGui::DragFloat("Friction",        &component.friction,       0.01f, 0.0f, 1.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
            ImGui::DragFloat("Restitution",     &component.restitution,    0.01f, 0.0f, 1.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
            ImGui::DragFloat("Linear Damping",  &component.linearDamping,  0.01f, 0.0f, 1.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
            ImGui::DragFloat("Angular Damping", &component.angularDamping, 0.01f, 0.0f, 1.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp);

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
