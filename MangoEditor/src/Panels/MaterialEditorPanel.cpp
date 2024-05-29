#include "MaterialEditorPanel.h"
#include "DragDropPayloadTypes.h"
#include "Mango/Scene/SelectionManager.h"

#include "Mango/Asset/AssetManager.h"
#include "Mango/Asset/Manager/EditorAssetManager.h"
#include "Mango/Core/Assertions.h"
#include "Mango/Core/AssetManager.h"
#include "Mango/Project/Project.h"
#include "Mango/Systems/ImGuiSystem.h"

#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>
#include <strutil.h>


namespace mango
{
    namespace
    {
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
    }

    void MaterialEditorPanel::onGui()
    {
        if (!isOpen)
        {
            SelectionManager::resetMaterialSelection();
            isOpen = true;
        }

        auto& materialToEdit = SelectionManager::getSelectedMaterial();

        if (materialToEdit)
        {
            static std::string searchPattern  = "";

            ImGui::Begin("Material Editor", &isOpen);

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
                for (auto [name, material] : AssetManagerOld::getMaterialList())
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

                                //bool isSrgb = (type == Material::TextureType::DIFFUSE) || (type == Material::TextureType::EMISSION);
                                AssetHandle assetHandle = Project::getActive()->getEditorAssetManager()->importAsset(path);
                                            texture     = AssetManager::getAsset<Texture>(assetHandle);
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
                                texture = AssetManagerOld::getTexture2D(defaultTextureName);
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
    }
}
