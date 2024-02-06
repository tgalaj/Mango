#include "SceneHierarchyPanel.h"

#include "Mango/Core/AssetManager.h"
#include "Mango/Scene/Components.h"

#include <imgui.h>
#include <imgui_internal.h>
#include <misc/cpp/imgui_stdlib.h>
#include <glm/gtc/type_ptr.hpp>

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
        ImGui::Begin("Hierarchy Panel");
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

        drawComponent<ModelRendererComponent>("Model Renderer", entity, [&entity](auto& component)
        {
            const char* renderQueueStrings[] = { "Opaque", "Alpha", "Static Environment Mapping", "Dynamic Environment Mapping"};
            const char* currentRenderQueue   = renderQueueStrings[int(component.getRenderQueue())];

            if (ImGui::BeginCombo("Render Queue", currentRenderQueue))
            {
                for (int i = 0; i < std::size(renderQueueStrings); ++i)
                {
                    bool isSelected = currentRenderQueue == renderQueueStrings[i];
                    if (ImGui::Selectable(renderQueueStrings[i], isSelected))
                    {
                        currentRenderQueue   = renderQueueStrings[i];
                        entity.addOrReplaceComponent<ModelRendererComponent>(component.model, ModelRendererComponent::RenderQueue(i));
                    }

                    if (isSelected)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }

            Model& model = component.model;
            const char* modelTypeStrings[] = { "Model3D", "Cone", "Cube", "Cylinder", "Plane", "Sphere", "Torus", "Quad" };
            const char* currentModelType = modelTypeStrings[int(model.getModelType())];

            if (ImGui::BeginCombo("Type", currentModelType))
            {
                for (int i = 0; i < std::size(modelTypeStrings); ++i)
                {
                    bool isSelected = currentModelType == modelTypeStrings[i];
                    if (ImGui::Selectable(modelTypeStrings[i], isSelected))
                    {
                        currentModelType = modelTypeStrings[i];
                        model.setModelType(Model::ModelType(i));
                    }

                    if (isSelected)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }

            PrimitiveProperties pp                        = model.getPrimitiveProperties();
            bool                updatePrimitiveProperties = false;

            switch (model.getModelType())
            {
                case Model::ModelType::Model3D:
                    ImGui::InputText("##Filename", &model.m_filename);

                    ImGui::SameLine();
                    ImGui::PushItemWidth(-1);

                    if (ImGui::Button("Load"))
                    {
                        model.load(model.m_filename);
                    }
                    ImGui::PopItemWidth();
                    break;

                case mango::Model::ModelType::Cone: 
                    updatePrimitiveProperties |= ImGui::DragFloat("Height", &pp.height, 0.01f, 0.01f, FLT_MAX, "%.2f", ImGuiSliderFlags_AlwaysClamp);
                    updatePrimitiveProperties |= ImGui::DragFloat("Radius", &pp.radius, 0.01f, 0.01f, FLT_MAX, "%.2f", ImGuiSliderFlags_AlwaysClamp);
                    updatePrimitiveProperties |= ImGui::DragInt  ("Slices", &pp.slices, 1,     3,     INT_MAX, "%d",   ImGuiSliderFlags_AlwaysClamp);
                    updatePrimitiveProperties |= ImGui::DragInt  ("Stacks", &pp.stacks, 1,     1,     INT_MAX, "%d",   ImGuiSliderFlags_AlwaysClamp);
                    break;

                case mango::Model::ModelType::Cube:
                    updatePrimitiveProperties |= ImGui::DragFloat("Size", &pp.size, 0.01f, 0.01f, FLT_MAX, "%.2f", ImGuiSliderFlags_AlwaysClamp);
                    break;

                case mango::Model::ModelType::Cylinder:
                    updatePrimitiveProperties |= ImGui::DragFloat("Height", &pp.height, 0.01f, 0.01f, FLT_MAX, "%.2f", ImGuiSliderFlags_AlwaysClamp);
                    updatePrimitiveProperties |= ImGui::DragFloat("Radius", &pp.radius, 0.01f, 0.01f, FLT_MAX, "%.2f", ImGuiSliderFlags_AlwaysClamp);
                    updatePrimitiveProperties |= ImGui::DragInt  ("Slices", &pp.slices, 1,     3,     INT_MAX, "%d",   ImGuiSliderFlags_AlwaysClamp);
                    break;

                case mango::Model::ModelType::Plane:
                    updatePrimitiveProperties |= ImGui::DragFloat("Width",  &pp.width,  0.01f, 0.01f, FLT_MAX, "%.2f", ImGuiSliderFlags_AlwaysClamp);
                    updatePrimitiveProperties |= ImGui::DragFloat("Height", &pp.height, 0.01f, 0.01f, FLT_MAX, "%.2f", ImGuiSliderFlags_AlwaysClamp);
                    updatePrimitiveProperties |= ImGui::DragInt  ("Slices", &pp.slices, 1,     1,     INT_MAX, "%d",   ImGuiSliderFlags_AlwaysClamp);
                    updatePrimitiveProperties |= ImGui::DragInt  ("Stacks", &pp.stacks, 1,     1,     INT_MAX, "%d",   ImGuiSliderFlags_AlwaysClamp);
                    break;

                case mango::Model::ModelType::Sphere:
                    updatePrimitiveProperties |= ImGui::DragFloat("Radius", &pp.radius, 0.01f, 0.01f, FLT_MAX, "%.2f", ImGuiSliderFlags_AlwaysClamp);
                    updatePrimitiveProperties |= ImGui::DragInt  ("Slices", &pp.slices, 1,     3,     INT_MAX, "%d",   ImGuiSliderFlags_AlwaysClamp);
                    break;

                case mango::Model::ModelType::Torus:
                    updatePrimitiveProperties |= ImGui::DragFloat("Inner Radius", &pp.innerRadius, 0.01f, 0.01f, FLT_MAX, "%.2f", ImGuiSliderFlags_AlwaysClamp);
                    updatePrimitiveProperties |= ImGui::DragFloat("Outer Radius", &pp.outerRadius, 0.01f, 0.01f, FLT_MAX, "%.2f", ImGuiSliderFlags_AlwaysClamp);
                    updatePrimitiveProperties |= ImGui::DragInt  ("Slices",       &pp.slices,      1,     3,     INT_MAX, "%d",   ImGuiSliderFlags_AlwaysClamp);
                    updatePrimitiveProperties |= ImGui::DragInt  ("Stacks",       &pp.stacks,      1,     3,     INT_MAX, "%d",   ImGuiSliderFlags_AlwaysClamp);
                    break;

                case mango::Model::ModelType::Quad:
                    updatePrimitiveProperties |= ImGui::DragFloat("Width",  &pp.width,  0.01f, 0.01f, FLT_MAX, "%.2f", ImGuiSliderFlags_AlwaysClamp);
                    updatePrimitiveProperties |= ImGui::DragFloat("Height", &pp.height, 0.01f, 0.01f, FLT_MAX, "%.2f", ImGuiSliderFlags_AlwaysClamp);
                    break;
            }

            if (updatePrimitiveProperties) model.setPrimitiveProperties(pp);

            ImGui::Separator();

            // TODO: this needs to be reworked, it's a temporary solution right now
            // Display material for the base mesh only
            auto& material = model.getMesh().material;

            ImGui::Text("Material");

            // Draw vec3 map
            for (auto& [propertyName, vec3Value] : material.m_vec3Map)
            {
                ImGui::DragFloat3(propertyName.c_str(), glm::value_ptr(vec3Value));
            }
            // Draw float map
            for (auto& [propertyName, floatValue] : material.m_floatMap)
            {
                ImGui::DragFloat(propertyName.c_str(), &floatValue);
            }
            // Draw textures
            for (auto& [textureType, texture] : material.m_textureMap)
            {
                std::string textureName = "##" + Material::m_textureUniformsMap[textureType];
                ImGui::InputText(textureName.c_str(), &texture->getFilename());
                ImGui::SameLine();
                ImGui::PushItemWidth(-1);

                if (ImGui::Button("Load"))
                {
                    if(texture->getFilename().empty()) return;

                    bool isSrgb = (textureType == Material::TextureType::DIFFUSE) || (textureType == Material::TextureType::SPECULAR);
                    texture = AssetManager::createTexture2D(texture->getFilename(), isSrgb);
                }
                ImGui::PopItemWidth();
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
