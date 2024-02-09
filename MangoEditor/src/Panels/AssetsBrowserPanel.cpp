#include "AssetsBrowserPanel.h"

#include "Mango/Core/VFI.h"

#include <imgui.h>

namespace mango
{

    AssetsBrowserPanel::AssetsBrowserPanel()
    {
        auto searchPath = VFI::getSearchPath();
        
        m_basePath    = searchPath[searchPath.size() - 2];
        m_currentPath = m_basePath;

        m_folderTexture = std::make_shared<Texture>();
        m_folderTexture->createTexture2d("icons/folder.png", false, 8);

        m_fileTexture = std::make_shared<Texture>();
        m_fileTexture->createTexture2d("icons/file.png", false, 8);
    }

    void AssetsBrowserPanel::onGui()
    {
        ImGui::Begin("Assets Browser Panel");

        if (m_currentPath != m_basePath)
        {
            if (ImGui::Button("<-"))
            {
                m_currentPath = m_currentPath.parent_path();
            }
        }

        if (ImGui::IsWindowHovered() && (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) || ImGui::IsKeyDown(ImGuiKey_LeftCtrl)))
        {
            thumbnailSize += ImGui::GetIO().MouseWheel * 2.0f;
            thumbnailSize  = std::clamp(thumbnailSize, 32.0f, 256.0f);
        }

        float cellSize     = thumbnailSize + thumbnailPadding;
        float panelWidth   = ImGui::GetContentRegionAvail().x;
        auto  columnsCount = (int)(panelWidth / cellSize);

        if (ImGui::BeginTable("Thumbnails", columnsCount))
        {
            for (auto& directoryIterator : std::filesystem::directory_iterator(m_currentPath))
            {
                ImGui::TableNextColumn();
                const auto& path           = directoryIterator.path();
                std::string filenameString = path.filename().string();

                ImGui::PushID(filenameString.c_str());
                ImGui::PushStyleColor(ImGuiCol_Button, { 0, 0, 0, 0});
                
                ImGui::ImageButton(directoryIterator.is_directory() ?
                                   (ImTextureID)m_folderTexture->getRendererID() : 
                                   (ImTextureID)m_fileTexture->getRendererID(),
                                   { thumbnailSize, thumbnailSize });

                if (ImGui::BeginDragDropSource())
                {
                    std::filesystem::path relativePath(path);
                    const wchar_t* itemPath = relativePath.c_str();

                    // TODO: consider moving payload type to a shared file (make a define or static const)
                    ImGui::SetDragDropPayload("MG_ASSETS_BROWSER_ITEM", itemPath, (wcslen(itemPath) + 1) * sizeof(wchar_t), ImGuiCond_Once);
                    ImGui::EndDragDropSource();
                }

                ImGui::PopStyleColor();

                if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
                {
                    if (directoryIterator.is_directory())
                    {
                        m_currentPath /= filenameString;
                    }
                    else
                    {
                        system(("start " + path.string() + " &").c_str());
                    }
                }

                ImGui::TextWrapped(filenameString.c_str());
                ImGui::Spacing();

                ImGui::PopID();
            }
            ImGui::EndTable();
        }

        // TODO: status bar
        ImGui::End();
    }
}