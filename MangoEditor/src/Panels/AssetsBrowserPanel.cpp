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

        m_pathStack.reserve(10);
        m_pathStack.push_back(m_currentPath.filename().string());

        m_folderTexture = std::make_shared<Texture>();
        m_folderTexture->createTexture2d("icons/folder.png", false, 8);

        m_fileTexture = std::make_shared<Texture>();
        m_fileTexture->createTexture2d("icons/file.png", false, 8);

        m_leftArrow = std::make_shared<Texture>();
        m_leftArrow->createTexture2d("icons/left_arrow.png", false, 4);
    }

    void AssetsBrowserPanel::onGui()
    {
        ImGui::Begin("Assets Browser Panel");

        ImGui::PushStyleColor(ImGuiCol_Button, { 0, 0, 0, 0 });
        ImGui::BeginDisabled(m_currentPath == m_basePath);
        {
            if (ImGui::ImageButton("backArrow", (ImTextureID)m_leftArrow->getRendererID(), {backButtonSize, backButtonSize}) ||
                ImGui::IsMouseClicked(ImGuiMouseButton_Middle + 1))
            {
                m_currentPath = m_currentPath.parent_path();
                m_pathStack.pop_back();
            }
        }
        ImGui::EndDisabled();

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