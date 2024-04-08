#include "ContentBrowserPanel.h"
#include "DragDropPayloadTypes.h"
#include "IconsMaterialDesignIcons.h"

#include "Mango/Core/VFI.h"
#include "Mango/Project/Project.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS_IMPLEMENTED
#include <imgui.h>
#include <imgui_internal.h>

namespace mango
{

    ContentBrowserPanel::ContentBrowserPanel()
    {        
        m_basePath    = Project::getAssetDirectory();
        m_currentPath = m_basePath;

        m_pathStack.reserve(10);
        m_pathStack.push_back(m_currentPath.filename().string());

        m_folderIcon = createRef<Texture>();
        m_folderIcon->createTexture2d("icons/folder.png", false, 8);

        m_fileIcon = createRef<Texture>();
        m_fileIcon->createTexture2d("icons/file.png", false, 8);
    }

    void ContentBrowserPanel::onGui()
    {
        ImGui::Begin("Content Browser");

        ImGui::PushStyleColor(ImGuiCol_Button, { 0, 0, 0, 0 });

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
        ImGui::BeginDisabled(m_currentPath == m_basePath);
        {
            if (ImGui::Button(ICON_MDI_ARROW_LEFT_BOLD, { backButtonSize, backButtonSize }) ||
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
        auto  columnsCount = glm::max((int)(panelWidth / cellSize), 1);

        bool itemPopupOpened = false;
        if (ImGui::BeginTable("Thumbnails", columnsCount))
        {
            for (auto& directoryIterator : std::filesystem::directory_iterator(m_currentPath))
            {
                ImGui::TableNextColumn();
                const auto& path           = directoryIterator.path();
                std::string filenameString = path.filename().string();
                bool isDirectory           = directoryIterator.is_directory();

                ImGui::PushStyleColor(ImGuiCol_Button, { 0, 0, 0, 0});
                contentBrowserButton(filenameString.c_str(), 
                                     isDirectory ?
                                     (ImTextureID)m_folderIcon->getRendererID() : 
                                     (ImTextureID)m_fileIcon->getRendererID(),
                                     path == m_currentRenameEntry);

                if (ImGui::BeginDragDropSource())
                {
                    std::filesystem::path relativePath = std::filesystem::relative(path, m_basePath);
                    const wchar_t* itemPath = relativePath.c_str();

                    ImGui::SetDragDropPayload(MG_DRAG_PAYLOAD_CB_ITEM, itemPath, (wcslen(itemPath) + 1) * sizeof(wchar_t), ImGuiCond_Once);
                    ImGui::Text(relativePath.filename().string().c_str());
                    ImGui::EndDragDropSource();
                }

                ImGui::PopStyleColor();

                if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
                {
                    if (isDirectory)
                    {
                        m_currentPath /= filenameString;
                        m_pathStack.push_back(filenameString);
                    }
                    else
                    {
                        cmdOpenExternally(path);
                    }
                }
                else if (ImGui::IsMouseClicked(ImGuiMouseButton_Right) && ImGui::IsItemHovered())
                {
                    ImGui::OpenPopup(filenameString.c_str());
                }

                if (ImGui::BeginPopup(filenameString.c_str()))
                {
                    itemPopupOpened = true;
                    
                    if (ImGui::MenuItem("Rename"))
                    {
                        m_currentRenameEntry = path;
                    }

                    if (ImGui::MenuItem("Delete"))
                    {
                        cmdDelete(path);
                    }

                    ImGui::Separator();
                    if (ImGui::MenuItem("Show In Explorer"))
                    {
                        auto pathToOpen = m_currentPath / filenameString;
                        cmdShowInExplorer(isDirectory ? pathToOpen : pathToOpen.parent_path());
                    }
                    if (!isDirectory)
                    {
                        if (ImGui::MenuItem("Open Externally"))
                        {
                            cmdOpenExternally(path);
                        }
                    }

                    ImGui::EndPopup();
                }
            }
            ImGui::EndTable();
        }

        #ifdef _WIN32
        if (!itemPopupOpened && ImGui::BeginPopupContextWindow(0, ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems))
        {
            if (ImGui::MenuItem("Show in Explorer"))
            {
                cmdShowInExplorer(m_currentPath);
            }
            ImGui::EndPopup();
        }
        #endif

        ImGui::End();
    }

    bool ContentBrowserPanel::contentBrowserButton(const char* label, void* textureID, bool rename)
    {
              ImGuiStyle& style            = ImGui::GetStyle();
        const ImVec2      padding          = style.FramePadding;
        const ImVec2      wrappedLabelSize = ImGui::CalcTextSize(label, 0, false, thumbnailSize);

        ImGui::SetNextItemAllowOverlap();
        ImGui::InvisibleButton(label, { thumbnailSize, thumbnailSize + wrappedLabelSize.y });

        if (!ImGui::IsItemVisible()) // Skip rendering as ImDrawList elements are not clipped.
            return false;

        const ImVec2 p0      = ImGui::GetItemRectMin();
        const ImVec2 p1      = ImGui::GetItemRectMax();
        const ImVec2 p0image = { p0.x, p0.y - padding.y };
        const ImVec2 p1image = { p1.x, p1.y - wrappedLabelSize.y - padding.y } ;

        float textAlignedPosX = p0.x + glm::max((p1.x - p0.x) - wrappedLabelSize.x, 0.0f) * 0.25f;

        const ImVec2      textPos  = ImVec2(textAlignedPosX, p1image.y);
              ImDrawList* drawList = ImGui::GetWindowDrawList();

              bool held    = ImGui::IsItemActive();
              bool hovered = ImGui::IsItemHovered();
              ImGuiID id   = ImGui::GetItemID();
        const ImU32 col    = ImGui::GetColorU32((held && hovered) ? ImGuiCol_ButtonActive : hovered ? ImGuiCol_ButtonHovered : ImGuiCol_Button);

        ImGui::RenderNavHighlight(ImRect(p0, p1), id);
        ImGui::RenderFrame(p0, p1, col, true, glm::clamp((float)glm::min(padding.x, padding.y), 0.0f, style.FrameRounding));
        
        drawList->AddImage(textureID, p0image, p1image, { 0, 1 }, { 1, 0 });

        if (!rename) 
            drawList->AddText(ImGui::GetDefaultFont(), ImGui::GetFontSize(), textPos, IM_COL32_WHITE, label, 0, thumbnailSize);
        else
        {   
            static char buffer[256];
            memset(buffer, 0, sizeof(buffer));
            strcpy_s(buffer, sizeof(buffer), label);

            ImGui::SetNextItemWidth(thumbnailSize);
            ImGui::SetKeyboardFocusHere(0);
            ImGui::SetCursorScreenPos(textPos - padding);
            if (ImGui::InputText("##rename", buffer, sizeof(buffer), ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
            {
                std::string newName(buffer);

                std::filesystem::rename(m_currentRenameEntry, m_currentRenameEntry.parent_path() / newName);
                m_currentRenameEntry = "";
            }

            if (!ImGui::IsItemHovered() && (ImGui::IsMouseClicked(ImGuiMouseButton_Left) || ImGui::IsMouseClicked(ImGuiMouseButton_Middle) || ImGui::IsMouseClicked(ImGuiMouseButton_Right)))
            {
                m_currentRenameEntry = "";
            }
        }
    }

    void ContentBrowserPanel::cmdShowInExplorer(std::filesystem::path path)
    {
        auto command = "explorer " + path.make_preferred().string();
        system(command.c_str());
    }

    void ContentBrowserPanel::cmdOpenExternally(std::filesystem::path path)
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

    void ContentBrowserPanel::cmdDelete(std::filesystem::path path)
    {
        std::filesystem::remove_all(path);
    }

}