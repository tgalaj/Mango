#include "SceneHierarchyPanel.h"
#include "DragDropPayloadTypes.h"
#include "IconsMaterialDesignIcons.h"

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
                for (auto [entityID] : m_scene->m_registry.storage<entt::entity>().each())
                { 
                    Entity entity{ entityID, m_scene.get() };

                    if (!entity.hasParent())
                    {
                        drawEntityNode(entity);
                    }
                }
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
                           flags |= entity.getChildren().empty() ? ImGuiTreeNodeFlags_Leaf : 0;
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
            for (auto [child, transform] : entity.getChildren())
            {
                drawEntityNode(child);
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

        if (ImGui::Button("ADD " ICON_MDI_PLUS_THICK))
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

        drawComponent<TransformComponent>("Transform", entity, [entity](auto& component)
        {
            bool hasParent = component.hasParent();
            ImGui::Checkbox("Has parent", &hasParent);

            if (hasParent)
            {
                if(ImGui::Button("Detach")) component.getParent()->removeChild(entity, component);
            }

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

            static int32_t       selectedMaterialIndex = -1;
            static bool          isMaterialEditorOpen  = false;
            static ref<Material> materialToEdit        = nullptr;

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

                    ImGui::SameLine();
                    if(ImGui::Button(ICON_MDI_DOTS_VERTICAL)) // Clear, Edit menu
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
            ImGui::Indent();

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
                                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(MG_DRAG_PAYLOAD_CONTENT_BROWSER_ITEM))
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
