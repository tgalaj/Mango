#include "AssetBrowserPanel.h"

#include "Mango/Core/VFI.h"
#include "Mango/Project/Project.h"

#include <imgui.h>

namespace mango
{

    AssetBrowserPanel::AssetBrowserPanel()
    {        
        m_basePath    = Project::getAssetDirectory();
        m_currentPath = m_basePath;

        m_pathStack.reserve(10);
        m_pathStack.push_back(m_currentPath.filename().string());

        m_folderIcon = createRef<Texture>();
        m_folderIcon->createTexture2d("icons/folder.png", false, 8);

        m_fileIcon = createRef<Texture>();
        m_fileIcon->createTexture2d("icons/file.png", false, 8);

        m_returnIcon = createRef<Texture>();
        m_returnIcon->createTexture2d("icons/return.png", false, 8);
    }

    void AssetBrowserPanel::onGui()
    {
        ImGui::Begin("Assets Browser Panel");

        #ifdef _WIN32
        if (ImGui::BeginPopupContextWindow(0, ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems))
        {
            if (ImGui::MenuItem("Open Folder in File Explorer"))
            {
                auto command = "explorer " + m_currentPath.make_preferred().string();
                system(command.c_str());
            }
            ImGui::EndPopup();
        }
        #endif

        ImGui::PushStyleColor(ImGuiCol_Button, { 0, 0, 0, 0 });

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
        ImGui::BeginDisabled(m_currentPath == m_basePath);
        {
            if (ImGui::ImageButton("backArrow", (ImTextureID)m_returnIcon->getRendererID(), { backButtonSize, backButtonSize }) ||
                ImGui::IsMouseClicked(ImGuiMouseButton_Middle + 1))
            {
                m_currentPath = m_currentPath.parent_path();
                m_pathStack.pop_back();
            }
        }
        ImGui::EndDisabled();
        ImGui::PopStyleVar();

        for (uint32_t i = 0; i < m_pathStack.size(); ++i)
        {
            auto& folderName = m_pathStack[i];
            if (i > 0)
            {
                ImGui::SameLine();

                auto cursorPos = ImGui::GetCursorPos();
                auto fontSize  = ImGui::GetFontSize();

                ImGui::SetCursorPos(ImVec2(cursorPos.x, cursorPos.y + backButtonSize * 0.5f - fontSize * 0.75f));
                ImGui::Text("/");
            }

            ImGui::SameLine();
            if (ImGui::Button(folderName.c_str(), { 0, backButtonSize }))
            {
                for (uint32_t c = m_pathStack.size() - 1; c > i; --c)
                {
                    m_currentPath = m_currentPath.parent_path();
                }
                    
                m_pathStack.erase(m_pathStack.begin() + i + 1, m_pathStack.end());
            }
        }

        ImGui::PopStyleColor();

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

                ImGui::PushStyleColor(ImGuiCol_Button, { 0, 0, 0, 0});
                
                ImGui::ImageButton(filenameString.c_str(),
                                   directoryIterator.is_directory() ?
                                   (ImTextureID)m_folderIcon->getRendererID() : 
                                   (ImTextureID)m_fileIcon->getRendererID(),
                                   { thumbnailSize, thumbnailSize });

                if (ImGui::BeginDragDropSource())
                {
                    std::filesystem::path relativePath = std::filesystem::relative(path, m_basePath);
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
                        m_pathStack.push_back(filenameString);
                    }
                    else
                    {
                        #ifdef _WIN32
                        system(("start " + path.string() + " &").c_str());
                        #endif

                        // Didn't test on Linux and MacOS, might work...
                        #ifdef __linux__
                        system(("xdg-open " + path.string() + " &").c_str());
                        #endif

                        #ifdef __APPLE__
                        system(("open " + path.string() + " &").c_str());
                        #endif
                    }
                }

                ImGui::TextWrapped(filenameString.c_str());
                ImGui::Spacing();
            }
            ImGui::EndTable();
        }
        ImGui::End();
    }
}