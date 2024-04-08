#include "SceneHierarchyPanel.h"
#include "DragDropPayloadTypes.h"
#include "IconsMaterialDesignIcons.h"

#include "Mango/Core/AssetManager.h"
#include "Mango/Core/VFI.h"
#include "Mango/ImGui/ImGuiUtils.h"
#include "Mango/Project/Project.h"
#include "Mango/Scene/Components.h"
#include "Mango/Systems/ImGuiSystem.h"

#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>
#include <strutil.h>

#include <format>
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
        ImGui::Begin("Scene Hierarchy", nullptr);
        { 
            auto* window = ImGui::GetCurrentWindow();
            if(ImGui::BeginDragDropTargetCustom(window->InnerRect, window->ID))
            {
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(MG_DRAG_PAYLOAD_SHP_ENTITY))
                {
                    auto* dragEntity = (Entity*)payload->Data;

                    if (dragEntity->hasParent())
                    {
                        dragEntity->getParent().removeChild(*dragEntity);
                    }
                }
                ImGui::EndDragDropTarget();
            }

            if (m_scene)
            {
                for (auto [entityID] : m_scene->m_registry.storage<entt::entity>().each())
                { 
                    Entity entity{ entityID, m_scene.get() };

                    if (!entity.hasParent())
                    {
                        drawEntityNode(entity);
                    }
                }
            }

            if ((ImGui::IsMouseClicked(ImGuiMouseButton_Left) || ImGui::IsMouseDown(ImGuiMouseButton_Right)) && ImGui::IsWindowHovered() && !ImGui::IsAnyItemHovered())
            {
                m_selectedEntity = {};
            }

            // Right click on empty space
            if (ImGui::BeginPopupContextWindow(0, ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems))
            {
                displayEntityContextMenu(true);
                
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

    SceneHierarchyPanel::EcmAction SceneHierarchyPanel::displayEntityContextMenu(bool isClickedEmptySpace)
    {
        EcmAction action = EcmAction::None;

        if (ImGui::MenuItem("Create Empty Entity"))
        {
            auto newEntity = m_scene->createEntity("Empty Entity");

            if (m_selectedEntity)
            {
                m_selectedEntity.addChild(newEntity);
            }
        }
        if (ImGui::MenuItem("Delete Entity", nullptr, nullptr, !isClickedEmptySpace))
        {
            action = EcmAction::Delete;
        }

        return action;
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
                           flags |= entity.getChildren().empty() ? ImGuiTreeNodeFlags_Leaf : 0;
        bool opened     = ImGui::TreeNodeEx((void*)(uint64_t)(entity.getUUID()), flags, name.c_str());
        bool isSelected = flags & ImGuiTreeNodeFlags_Selected;

        if (ImGui::BeginDragDropSource())
        {
            ImGui::SetDragDropPayload(MG_DRAG_PAYLOAD_SHP_ENTITY, &entity, sizeof(Entity));
            ImGui::Text(name.c_str());
            ImGui::EndDragDropSource();
        }

        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(MG_DRAG_PAYLOAD_SHP_ENTITY))
            {
                auto* dragEntity = (Entity*)payload->Data;

                // Check if dragged Entity is dropped onto its child
                if (!(*dragEntity).hasChild(entity))
                {
                    if (dragEntity->hasParent())
                    {
                        dragEntity->getParent().removeChild(*dragEntity);
                    }

                    entity.addChild(*dragEntity);
                }
            }
            ImGui::EndDragDropTarget();
        }

        // Mark the entity as selected:
        // - on mouse click
        // - when tree is traversed with the keyboard arrow keys (isHovered && isFocused)
        if ((ImGui::IsMouseDown(ImGuiMouseButton_Left) || ImGui::IsMouseDown(ImGuiMouseButton_Right)) && ImGui::IsItemHovered())
        {
            m_selectedEntity = entity;
            MG_CORE_TRACE("Selected entity ID: {}, name: {}", entity.getUUID(), entity.getName());
        }

        // Right click on empty space
        bool      isEntityDeleted = false;
        EcmAction ecmAction       = EcmAction::None;
        if (ImGui::BeginPopupContextItem())
        {
            ecmAction       = displayEntityContextMenu(false);
            isEntityDeleted = (EcmAction::Delete == ecmAction);

            ImGui::EndPopup();
        }
        
        if (isSelected && ImGui::IsItemFocused() && !isEntityDeleted && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Delete)))
        {
            isEntityDeleted = true;
        }

        if (opened)
        {
            for (auto childEntity : entity.getChildren())
            {
                drawEntityNode(childEntity);
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

                if (ImGui::Button(ICON_MDI_COG, ImVec2{lineHeight + 3.0f, lineHeight}))
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
                float columnWidth = ImGui::GetContentRegionAvail().x * 0.5f;
                auto& component   = entity.getComponent<ComponentType>();

                if (!std::is_same_v<ComponentType, TransformComponent>)
                {
                    if (ImGui::BeginTable(typeid(ComponentType).name(), 2, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_BordersInnerV))
                    {
                        ImGui::TableSetupColumn("x", ImGuiTableColumnFlags_WidthFixed, columnWidth);
                        ImGui::TableSetupColumn("y", ImGuiTableColumnFlags_WidthStretch);

                        uiFunction(component);
                        
                        ImGui::EndTable();
                    }
                }
                else
                {
                    uiFunction(component);
                }
                ImGui::TreePop();
            }

            if (removeComponent)
            {
                entity.removeComponent<ComponentType>();
            }

            ImGui::Spacing();
        }
    }

    void SceneHierarchyPanel::drawComponents(Entity entity)
    {
        if (entity.hasComponent<TagComponent>())
        {
            static bool editMode = false;

            auto& name = entity.getComponent<TagComponent>().name;

            ImGui::BeginDisabled(editMode);
            if (ImGui::Button(ICON_MDI_LEAD_PENCIL))
            {
                editMode = true;
            }
            ImGui::EndDisabled();

            ImGui::SameLine(0.0f, ImGui::GetFontSize() * 0.3f);
            if (editMode)
            {
                static char buffer[256];
                memset(buffer, 0, sizeof(buffer));
                strcpy_s(buffer, sizeof(buffer), name.c_str());

                ImGui::SetKeyboardFocusHere(0);
                if (ImGui::InputText("##edit_name", buffer, sizeof(buffer), ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
                {
                    name     = buffer;
                    editMode = false;
                }

                if (!ImGui::IsItemHovered())
                {
                    if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) || ImGui::IsMouseClicked(ImGuiMouseButton_Middle) || ImGui::IsMouseClicked(ImGuiMouseButton_Right))
                    {
                        editMode = false;
                    }
                }
            }
            else
            {
                ImGui::Text(name.c_str());
            }
        }

        ImGui::SameLine();
        ImGui::PushItemWidth(-1);

        if (ImGui::Utils::ButtonAligned("ADD " ICON_MDI_PLUS_THICK, 1.0f))
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

        drawComponent<TransformComponent>("TRANSFORM", entity, [](auto& component)
        {
            float columnWidth = ImGui::GetTextLineHeight() * 4.0f;

            auto position = component.getPosition();
            auto rotation = glm::degrees(component.getRotation());
            auto scale    = component.getScale();

            if (ImGui::Utils::TableCustomDragFloat3("Position", position, 0.0f, columnWidth))
            {
                component.setPosition(position);
            }
            
            if (ImGui::Utils::TableCustomDragFloat3("Rotation", rotation, 0.0f, columnWidth))
            {
                component.setRotation(glm::radians(rotation));
            }

            if (ImGui::Utils::TableCustomDragFloat3("Scale", scale, 1.0f, columnWidth))
            {
                component.setScale(scale);
            }
        });

        drawComponent<DirectionalLightComponent>("DIRECTIONAL LIGHT", entity, [](auto& component)
        {
            ImGui::Utils::TableColorEdit3("Color",     &component.color[0]);
            ImGui::Utils::TableDragFloat ("Intensity", &component.intensity, 0.1f, 0.0f, FLT_MAX, "%.2f", ImGuiSliderFlags_AlwaysClamp);

            float size = component.getSize();
            if (ImGui::Utils::TableDragFloat("Size", &size))
            {
                component.setSize(size);
            }
            
            bool castsShadows = component.getCastsShadows();
            if (ImGui::Utils::TableCheckbox("Casts shadows", &castsShadows))
            {
                component.setCastsShadows(castsShadows);
            }
        });

        drawComponent<PointLightComponent>("POINT LIGHT", entity, [](auto& component)
        {
            ImGui::Utils::TableColorEdit3("Color",     &component.color[0]);
            ImGui::Utils::TableDragFloat ("Intensity", &component.intensity, 0.1f, 0.0f, FLT_MAX, "%.2f", ImGuiSliderFlags_AlwaysClamp);

            const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen    |
                                                     ImGuiTreeNodeFlags_Framed         |
                                                     ImGuiTreeNodeFlags_FramePadding   |
                                                     ImGuiTreeNodeFlags_SpanFullWidth |
                                                     ImGuiTreeNodeFlags_SpanAllColumns |
                                                     ImGuiTreeNodeFlags_AllowOverlap;
            ImGui::TableNextColumn();
            if (ImGui::TreeNodeEx("Attenuation", treeNodeFlags))
            {
                ImGui::TableNextColumn();
                Attenuation attenuation = component.getAttenuation();

                if (ImGui::Utils::TableDragFloat("Constant", &attenuation.constant, 0.01f, 0.0f, FLT_MAX, "%.2f", ImGuiSliderFlags_AlwaysClamp))
                {
                    component.setAttenuation(attenuation.constant, attenuation.linear, attenuation.quadratic);
                }

                if (ImGui::Utils::TableDragFloat("Linear", &attenuation.linear, 0.01f, 0.0f, FLT_MAX, "%.2f", ImGuiSliderFlags_AlwaysClamp))
                {
                    component.setAttenuation(attenuation.constant, attenuation.linear, attenuation.quadratic);
                }

                if (ImGui::Utils::TableDragFloat("Quadratic", &attenuation.quadratic, 0.01f, 0.0f, FLT_MAX, "%.2f", ImGuiSliderFlags_AlwaysClamp))
                {
                    component.setAttenuation(attenuation.constant, attenuation.linear, attenuation.quadratic);
                }

                ImGui::TreePop();
            }
            else
            {
                ImGui::TableNextColumn();
            }
            
            float shadowNearPlane = component.getShadowNearPlane();
            if (ImGui::Utils::TableDragFloat("Shadow Near Plane", &shadowNearPlane))
            {
                component.setShadowNearPlane(shadowNearPlane);
            }

            float shadowFarPlane = component.getShadowFarPlane();
            if (ImGui::Utils::TableDragFloat("Shadow Far Plane", &shadowFarPlane))
            {
                component.setShadowFarPlane(shadowFarPlane);
            }

            bool castsShadows = component.getCastsShadows();
            if (ImGui::Utils::TableCheckbox("Casts shadows", &castsShadows))
            {
                component.setCastsShadows(castsShadows);
            }
        });

        drawComponent<SpotLightComponent>("SPOT LIGHT", entity, [](auto& component)
        {
            ImGui::Utils::TableColorEdit3("Color",     &component.color[0]);
            ImGui::Utils::TableDragFloat ("Intensity", &component.intensity, 0.1f, 0.0f, FLT_MAX, "%.2f", ImGuiSliderFlags_AlwaysClamp);

            float cutoffAngle = glm::degrees(component.getCutOffAngle());
            if (ImGui::Utils::TableDragFloat("Cut-off Angle", &cutoffAngle, 0.01f, 0.0f, 89.99999f, "%.2f", ImGuiSliderFlags_AlwaysClamp))
            {
                component.setCutOffAngle(cutoffAngle);
            }

            const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen    |
                                                     ImGuiTreeNodeFlags_Framed         |
                                                     ImGuiTreeNodeFlags_FramePadding   |
                                                     ImGuiTreeNodeFlags_SpanAvailWidth |
                                                     ImGuiTreeNodeFlags_SpanAllColumns |
                                                     ImGuiTreeNodeFlags_AllowItemOverlap;
            ImGui::TableNextColumn();
            if (ImGui::TreeNodeEx("Attenuation", treeNodeFlags))
            {
                ImGui::TableNextColumn();
                Attenuation attenuation = component.getAttenuation();

                if (ImGui::Utils::TableDragFloat("Constant", &attenuation.constant, 0.01f, 0.0f, FLT_MAX, "%.2f", ImGuiSliderFlags_AlwaysClamp))
                {
                    component.setAttenuation(attenuation.constant, attenuation.linear, attenuation.quadratic);
                }

                if (ImGui::Utils::TableDragFloat("Linear", &attenuation.linear, 0.01f, 0.0f, FLT_MAX, "%.2f", ImGuiSliderFlags_AlwaysClamp))
                {
                    component.setAttenuation(attenuation.constant, attenuation.linear, attenuation.quadratic);
                }

                if (ImGui::Utils::TableDragFloat("Quadratic", &attenuation.quadratic, 0.01f, 0.0f, FLT_MAX, "%.2f", ImGuiSliderFlags_AlwaysClamp))
                {
                    component.setAttenuation(attenuation.constant, attenuation.linear, attenuation.quadratic);
                }

                ImGui::TreePop();
            }
            else
            {
                ImGui::TableNextColumn();
            }
            
            float shadowNearPlane = component.getShadowNearPlane();
            if (ImGui::Utils::TableDragFloat("Shadow Near Plane", &shadowNearPlane))
            {
                component.setShadowNearPlane(shadowNearPlane);
            }

            float shadowFarPlane = component.getShadowFarPlane();
            if (ImGui::Utils::TableDragFloat("Shadow Far Plane", &shadowFarPlane))
            {
                component.setShadowFarPlane(shadowFarPlane);
            }

            bool castsShadows = component.getCastsShadows();
            if (ImGui::Utils::TableCheckbox("Casts shadows", &castsShadows))
            {
                component.setCastsShadows(castsShadows);
            }
        });

        drawComponent<CameraComponent>("CAMERA", entity, [](auto& component)
        {            
            auto& camera = component.camera;

            ImGui::Utils::TableCheckbox("Primary", &component.isPrimary);

            const char* projectionTypeStrings[] = { "Perspective", "Orthographic" };
            const char* currentProjectionTypeString = projectionTypeStrings[int(camera.getProjectionType())];

            if (ImGui::Utils::TableBeginCombo("Projection", currentProjectionTypeString))
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
                if (ImGui::Utils::TableDragFloat("Vertical FOV", &verticalFov, 0.01f, 0.0f, 89.9999f, "%.2f", ImGuiSliderFlags_AlwaysClamp))
                {
                    camera.setPerspectiveVerticalFieldOfView(glm::radians(verticalFov));
                }

                float nearClip = camera.getPerspectiveNearClip();
                if (ImGui::Utils::TableDragFloat("Near Clip", &nearClip))
                {
                    camera.setPerspectiveNearClip(nearClip);
                }

                float farClip = camera.getPerspectiveFarClip();
                if (ImGui::Utils::TableDragFloat("Far Clip", &farClip))
                {
                    camera.setPerspectiveFarClip(farClip);
                }
            }

            if (camera.getProjectionType() == Camera::ProjectionType::Orthographic)
            {
                float orthoSize = camera.getOrthographicSize();
                if (ImGui::Utils::TableDragFloat("Size", &orthoSize))
                {
                    camera.setOrthographicSize(orthoSize);
                }

                float nearClip = camera.getOrthographicNearClip();
                if (ImGui::Utils::TableDragFloat("Near Clip", &nearClip))
                {
                    camera.setOrthographicNearClip(nearClip);
                }

                float farClip = camera.getOrthographicFarClip();
                if (ImGui::Utils::TableDragFloat("Far Clip", &farClip))
                {
                    camera.setOrthographicFarClip(farClip);
                }
            }

        });

        drawComponent<StaticMeshComponent>("STATIC MESH", entity, [](auto& component)
        {
            auto&       mesh  = component.mesh;
            std::string meshLabel = mesh ? mesh->getName() : "NULL";

            static std::string searchPattern = "";

            if (ImGui::Utils::TableButton("Mesh", meshLabel.c_str(), {-1, 0}))
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
                                                     ImGuiTreeNodeFlags_SpanAllColumns |
                                                     ImGuiTreeNodeFlags_AllowItemOverlap;

            static int32_t       selectedMaterialIndex = -1;
            static bool          isMaterialEditorOpen  = false;
            static ref<Material> materialToEdit        = nullptr;

            ImGui::TableNextColumn();
            if (ImGui::TreeNodeEx("Materials", treeNodeFlags))
            {
                ImGui::TableNextColumn();
                for (uint32_t i = 0; i < component.materials.size(); ++i)
                {
                    std::string materialLabel = component.materials[i] ? component.materials[i]->name : "NULL";

                    ImGui::PushID(i);
                    if (ImGui::Utils::TableButton(std::format("[Material {}]", i).c_str(), materialLabel.c_str()))
                    {
                        ImGui::OpenPopup("material_select_popup");
                        searchPattern = "";
                        selectedMaterialIndex = i;
                    }

                    ImGui::SameLine();
                    if (ImGui::Utils::ButtonAligned(ICON_MDI_DOTS_VERTICAL, 1.0f)) // Clear, Edit menu
                    {
                        ImGui::OpenPopup("material_options");
                        selectedMaterialIndex = i;
                    }
                    ImGui::PopID();
                }

                ImGui::PushID(selectedMaterialIndex);
                if (ImGui::BeginPopup("material_options"))
                {
                    if (ImGui::Selectable("Edit"))
                    {
                        isMaterialEditorOpen = true;
                        materialToEdit       = component.materials[selectedMaterialIndex];
                    }
                    
                    auto originalMaterial = mesh->getMaterials()[selectedMaterialIndex];

                    ImGui::BeginDisabled(component.materials[selectedMaterialIndex] == originalMaterial);
                    if (ImGui::Selectable("Clear"))
                    {
                        component.materials[selectedMaterialIndex] = originalMaterial;
                    }
                    ImGui::EndDisabled();
                    ImGui::EndPopup();
                }

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
            else
            {
                ImGui::TableNextColumn();
            }

            auto materialTextureTypeToString = [](Material::TextureType type) -> std::string
            {
                switch (type)
                {
                    case Material::TextureType::DIFFUSE:      return "Diffuse";
                    case Material::TextureType::SPECULAR:     return "Specular";
                    case Material::TextureType::NORMAL:       return "Normal";
                    case Material::TextureType::EMISSION:     return "Emission";
                    case Material::TextureType::DISPLACEMENT: return "Displacement";
                }
                MG_CORE_ASSERT_MSG(false, "Unknown texture type");
                return {};
            };
            
            // TODO(TG): clean this up - move this to a separate MaterialEditorPanel class
            if (isMaterialEditorOpen)
            {
                ImGui::Begin("Material Editor", &isMaterialEditorOpen);

                ImGui::Text("Material");
                ImGui::SameLine();

                if (ImGui::Button(materialToEdit->name.c_str(), { ImGui::GetContentRegionAvail().x, 0 }))
                {
                    ImGui::OpenPopup("select_material_to_edit_popup");
                }

                if (ImGui::BeginPopup("select_material_to_edit_popup"))
                {
                    ImGui::InputText("##search_pattern", &searchPattern, ImGuiInputTextFlags_EscapeClearsAll);

                    auto availWidth = ImGui::GetContentRegionAvail().x;
                    if (ImGui::Button("CLEAR", ImVec2(availWidth, ImGui::GetFrameHeight()))) searchPattern = "";

                    ImGui::BeginChild("##item_list", ImVec2(availWidth, ImGui::GetFrameHeight() * 7.0f));

                    // 1. get list of all loaded materials from AssetManager and list them
                    // 2. if item selected -> change the material to edit
                    int32_t id = 0;
                    for (auto [name, material] : AssetManager::getMaterialList())
                    {
                        auto rx = std::regex(searchPattern, std::regex_constants::icase);
                        if (std::regex_search(name, rx))
                        {
                            if (ImGui::Selectable(name.c_str()))
                            {
                                materialToEdit = material;
                                ImGui::CloseCurrentPopup();
                            }
                        }
                    }

                    ImGui::EndChild();
                    ImGui::EndPopup();
                }

                ImGui::Text("Shader:   %s", "tbd");
                ImGui::Separator();

                if (ImGui::BeginTable("Textures", 2, ImGuiTableFlags_SizingStretchSame))
                {
                    // Render Queue
                    ImGui::TableNextColumn();
                    ImGui::Text("Render Queue");

                    ImGui::TableNextColumn();
                    ImGui::SetNextItemWidth(-1);
                    const  char* renderQueueItems[]      = {"Opaque", "Transparent", "Env. Mapped Static", "Env. Mapped Dynamic"};
                    static auto  renderQueueCurrentIndex = (int)materialToEdit->getRenderQueue();

                    if (ImGui::BeginCombo("##render_queue", renderQueueItems[renderQueueCurrentIndex]))
                    {
                        for (int n = 0; n < std::size(renderQueueItems); ++n)
                        {
                            const bool isSelected = (renderQueueCurrentIndex == n);
                            if (ImGui::Selectable(renderQueueItems[n], isSelected))
                            {
                                renderQueueCurrentIndex = n;
                                materialToEdit->setRenderQueue((Material::RenderQueue)renderQueueCurrentIndex);
                            }

                            // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                            if (isSelected) ImGui::SetItemDefaultFocus();
                        }
                        ImGui::EndCombo();
                    }

                    // Blend Mode
                    ImGui::TableNextColumn();
                    ImGui::Text("Blend Mode");

                    ImGui::TableNextColumn();
                    const  char* blendModeItems[] = { "None", "Alpha" };
                    static auto  blendModeCurrentIndex = (int)materialToEdit->getBlendMode();

                    if (ImGui::BeginCombo("##blend_mode", blendModeItems[blendModeCurrentIndex]))
                    {
                        for (int n = 0; n < std::size(blendModeItems); ++n)
                        {
                            const bool isSelected = (blendModeCurrentIndex == n);
                            if (ImGui::Selectable(blendModeItems[n], isSelected))
                            {
                                blendModeCurrentIndex = n;
                                materialToEdit->setBlendMode((Material::BlendMode)blendModeCurrentIndex);
                            }

                            // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                            if (isSelected) ImGui::SetItemDefaultFocus();
                        }
                        ImGui::EndCombo();
                    }

                    ImGui::EndTable();
                }

                float previewSize = ImGui::GetFontSize() * 5.0f;
                if (ImGui::CollapsingHeader("Textures", ImGuiTreeNodeFlags_DefaultOpen))
                {
                    if (ImGui::BeginTable("Textures", 2, ImGuiTableFlags_SizingFixedFit))
                    {
                        auto availWidth = ImGui::GetContentRegionAvail().x;
                        ImGui::TableSetupColumn("x", ImGuiTableColumnFlags_WidthStretch);
                        ImGui::TableSetupColumn("y", ImGuiTableColumnFlags_WidthFixed, previewSize);

                        const char* defaultTextureNames[] = 
                        {
                            "DefaultDiffuse",
                            "DefaultSpecular",
                            "DefaultNormal",
                            "DefaultEmission",
                            "DefaultDisplacement"
                        };
                        for (auto &[type, texture] : materialToEdit->getTextureMap())
                        {
                            ImGui::TableNextColumn();
                            ImGui::Text("%s", materialTextureTypeToString(type).c_str());

                            ImGui::TableNextColumn();
                            ImVec2 cursorPos = ImGui::GetCursorPos();

                            ImGui::Image((ImTextureID)texture->getRendererID(), { previewSize, previewSize }, { 0, 1 }, { 1, 0 });

                            if (ImGui::BeginDragDropTarget())
                            {
                                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(MG_DRAG_PAYLOAD_CB_ITEM))
                                {
                                    const auto* path = (const wchar_t*)payload->Data;

                                    bool isSrgb = (type == Material::TextureType::DIFFUSE) || (type == Material::TextureType::EMISSION);
                                    texture = AssetManager::createTexture2D(std::filesystem::path(path).string(), isSrgb);
                                }
                                ImGui::EndDragDropTarget();
                            }

                            if (ImGui::BeginItemTooltip())
                            {
                                ImGui::Text("Path: %s", texture->getFilename().c_str());
                                ImGui::Image((ImTextureID)texture->getRendererID(), { 512, 512 }, { 0, 1 }, { 1, 0 });
                                ImGui::EndTooltip();
                            }

                            auto defaultTextureName = defaultTextureNames[(int)type];
                            if (texture->getFilename() != defaultTextureName)
                            {
                                ImGui::SetCursorPos(cursorPos);
                                ImGui::PushID(defaultTextureName);

                                if (ImGui::Button("X"))
                                {
                                    texture = AssetManager::getTexture2D(defaultTextureName);
                                }

                                ImGui::PopID();
                            }
                        }
                        ImGui::EndTable();
                    }
                }

                if (ImGui::CollapsingHeader("Floats", ImGuiTreeNodeFlags_DefaultOpen))
                {
                    if (ImGui::BeginTable("Floats", 2, ImGuiTableFlags_SizingStretchSame))
                    {
                        for (auto& [name, value] : materialToEdit->getFloatMap())
                        {
                            ImGui::TableNextColumn();
                            ImGui::Text(name.c_str());

                            ImGui::TableNextColumn();
                            ImGui::SetNextItemWidth(-1);
                            ImGui::PushID(name.c_str());
                            ImGui::DragFloat("##", &value);
                            ImGui::PopID();
                        }
                        ImGui::EndTable();
                    }
                }

                if (ImGui::CollapsingHeader("Vec3s", ImGuiTreeNodeFlags_DefaultOpen))
                {
                    if (ImGui::BeginTable("Vec3s", 2, ImGuiTableFlags_SizingStretchSame))
                    {
                        for (auto& [name, value] : materialToEdit->getVec3Map())
                        {
                            ImGui::TableNextColumn();
                            ImGui::Text(name.c_str());

                            ImGui::TableNextColumn();
                            ImGui::SetNextItemWidth(-1);
                            ImGui::PushID(name.c_str());
                            ImGui::DragFloat3("##", &value[0]);
                            ImGui::PopID();
                        }
                        ImGui::EndTable();
                    }
                }

                if (ImGui::CollapsingHeader("Bools", ImGuiTreeNodeFlags_DefaultOpen))
                {
                    if (ImGui::BeginTable("Bools", 2, ImGuiTableFlags_SizingStretchSame))
                    {
                        for (auto& [name, value] : materialToEdit->getBoolMap())
                        {
                            ImGui::TableNextColumn();
                            ImGui::Text(name.c_str());

                            ImGui::TableNextColumn();
                            ImGui::SetNextItemWidth(-1);
                            ImGui::PushID(name.c_str());
                            ImGui::Checkbox("##", &value);
                            ImGui::PopID();
                        }
                        ImGui::EndTable();
                    }
                }

                ImGui::End();
            }
        });

        drawComponent<RigidBody3DComponent>("RIGIDBODY 3D", entity, [](auto& component)
        {
            const char* motionTypeStrings[] = { "Static", "Kinematic", "Dynamic"};
            const char* currentMotionType   = motionTypeStrings[int(component.motionType)];

            if (ImGui::Utils::TableBeginCombo("Motion Type", currentMotionType))
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
            
            ImGui::Utils::TableDragFloat("Friction",        &component.friction,       0.01f, 0.0f, 1.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
            ImGui::Utils::TableDragFloat("Restitution",     &component.restitution,    0.01f, 0.0f, 1.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
            ImGui::Utils::TableDragFloat("Linear Damping",  &component.linearDamping,  0.01f, 0.0f, 1.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
            ImGui::Utils::TableDragFloat("Angular Damping", &component.angularDamping, 0.01f, 0.0f, 1.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
            ImGui::Utils::TableCheckbox ("Activated",       &component.isInitiallyActivated);
        });

        drawComponent<BoxCollider3DComponent>("BOX COLLIDER 3D", entity, [](auto& component)
        {
            ImGui::Utils::TableDragFloat3("Offset", &component.offset[0]);
            ImGui::Utils::TableDragFloat3("Extent", &component.halfExtent[0]);
        });

        drawComponent<SphereColliderComponent>("SPHERE COLLIDER", entity, [](auto& component)
        {
            ImGui::Utils::TableDragFloat3("Offset", &component.offset[0]);
            ImGui::Utils::TableDragFloat ("Radius", &component.radius);
        });
    }

}
