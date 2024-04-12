#include "SceneHierarchyPanel.h"

#include "DragDropPayloadTypes.h"
#include "IconsMaterialDesignIcons.h"

#include "Mango/Core/AssetManager.h"
#include "Mango/Core/VFI.h"
#include "Mango/ImGui/ImGuiUtils.h"
#include "Mango/Project/Project.h"
#include "Mango/Scene/Components.h"
#include "Mango/Scene/SelectionManager.h"
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
        m_scene = scene;
        SelectionManager::resetEntitySelection();
    }

    void SceneHierarchyPanel::onGui()
    {
        ImGui::Begin("Scene Hierarchy", nullptr);
        { 
            m_selectedEntity = SelectionManager::getSelectedEntity();

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
                SelectionManager::resetEntitySelection();
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

    SceneHierarchyPanel::EcmAction SceneHierarchyPanel::displayEntityContextMenu(bool isClickedEmptySpace)
    {
        EcmAction action = EcmAction::None;

        if (ImGui::MenuItem("Create Empty"))
        {
            auto newEntity = m_scene->createEntity("Empty Entity");

            if (m_selectedEntity)
            {
                m_selectedEntity.addChild(newEntity);
            }
        }
        if (ImGui::BeginMenu("3D Object"))
        {
            static const char* objectStringList[] = { "Cube", "Sphere", "Sphere UV", "Capsule", "Cylinder", "Cone", "Plane", "Quad", "Torus", "Torus Knot" };

            for (auto& objectString : objectStringList)
            {
                if (ImGui::MenuItem(objectString))
                {
                    auto newEntity = m_scene->createEntity(objectString);
                    auto& smc = newEntity.addComponent<StaticMeshComponent>();

                    auto mesh = AssetManager::getMesh(objectString);

                    smc.mesh      = mesh;
                    smc.materials = mesh->getMaterials();

                    if (m_selectedEntity)
                    {
                        m_selectedEntity.addChild(newEntity);
                    }
                }
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Light"))
        {
            if (ImGui::MenuItem("Directional Light"))
            {
                auto newEntity = m_scene->createEntity("Directional Light");
                newEntity.addComponent<DirectionalLightComponent>();

                if (m_selectedEntity)
                {
                    m_selectedEntity.addChild(newEntity);
                }
            }
            if (ImGui::MenuItem("Point Light"))
            {
                auto newEntity = m_scene->createEntity("Point Light");
                newEntity.addComponent<PointLightComponent>();

                if (m_selectedEntity)
                {
                    m_selectedEntity.addChild(newEntity);
                }
            }
            if (ImGui::MenuItem("Spot Light"))
            {
                auto newEntity = m_scene->createEntity("Spot Light");
                newEntity.addComponent<SpotLightComponent>();

                if (m_selectedEntity)
                {
                    m_selectedEntity.addChild(newEntity);
                }
            }
            ImGui::EndMenu();
        }

        if (ImGui::MenuItem("Camera"))
        {
            auto newEntity = m_scene->createEntity("Camera");
            newEntity.addComponent<CameraComponent>();

            if (m_selectedEntity)
            {
                m_selectedEntity.addChild(newEntity);
            }
        }

        if (ImGui::MenuItem("Delete", nullptr, nullptr, !isClickedEmptySpace))
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
            SelectionManager::selectEntity(entity);
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
                SelectionManager::resetEntitySelection();
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
            displayAddComponentEntry<CapsuleColliderComponent>("Capsule Collider 3D");
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
                        SelectionManager::selectMaterial(component.materials[selectedMaterialIndex]);
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
            ImGui::Utils::TableDragFloat3("Offset", &component.offset[0],     0.01f);
            ImGui::Utils::TableDragFloat3("Extent", &component.halfExtent[0], 0.01f);
        });

        drawComponent<CapsuleColliderComponent>("CAPSULE COLLIDER 3D", entity, [](auto& component)
        {
            ImGui::Utils::TableDragFloat3("Offset",      &component.offset[0],  0.01f);
            ImGui::Utils::TableDragFloat ("Half height", &component.halfHeight, 0.01f);
            ImGui::Utils::TableDragFloat ("Radius",      &component.radius,     0.01f);
        });

        drawComponent<SphereColliderComponent>("SPHERE COLLIDER", entity, [](auto& component)
        {
            ImGui::Utils::TableDragFloat3("Offset", &component.offset[0], 0.01f);
            ImGui::Utils::TableDragFloat ("Radius", &component.radius,    0.01f);
        });
    }

}
