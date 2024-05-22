#include "ContentBrowserPanel.h"
#include "DragDropPayloadTypes.h"
#include "EditorEvents.h"
#include "IconsMaterialDesignIcons.h"

#include "Mango/Asset/Loader/TextureLoader.h"
#include "Mango/Asset/Manager/EditorAssetManager.h"
#include "Mango/Core/Services.h"
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
        m_baseDirectory    = Project::getActiveAssetDirectory();
        m_currentDirectory = m_baseDirectory;
        
        m_treeNodes.emplace_back(TreeNode(".", 0));

        m_pathStack.reserve(10);
        m_pathStack.push_back(m_currentDirectory.filename().string());

        m_directoryIcon = TextureLoader::loadTexture2D("icons/folder.png");//createRef<Texture>();
        //m_directoryIcon->createTexture2d("icons/folder.png", false, 8);

        m_fileIcon = TextureLoader::loadTexture2D("icons/file.png");//createRef<Texture>();
        //m_fileIcon->createTexture2d("icons/file.png", false, 8);

        refreshAssetTree();
    }

    void ContentBrowserPanel::onGui()
    {
        ImGui::Begin("Content Browser");

        ImGui::PushStyleColor(ImGuiCol_Button, { 0, 0, 0, 0 });

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));

        // Asset view / File System view switcher
        {
            const char* label = m_currentMode == Mode::Asset ? "Asset" : "File";
            if (ImGui::Button(label))
            {
                m_currentMode = m_currentMode == Mode::Asset ? Mode::FileSystem : Mode::Asset;
            }
        }

        // Go to prev directory button
        ImGui::BeginDisabled(m_currentDirectory == m_baseDirectory);
        {
            ImGui::SameLine();

            if (ImGui::Button(ICON_MDI_ARROW_LEFT_BOLD, { backButtonSize, backButtonSize }) ||
                ImGui::IsMouseClicked(ImGuiMouseButton_Middle + 1))
            {
                m_currentDirectory = m_currentDirectory.parent_path();
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
                    m_currentDirectory = m_currentDirectory.parent_path();
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
            if (Mode::Asset == m_currentMode)
            {
                TreeNode* node = &m_treeNodes[0];

                auto currentDir = std::filesystem::relative(m_currentDirectory, Project::getActiveAssetDirectory());
                for (const auto& p : currentDir)
                {
                    // if only one level
                    if (node->path == currentDir)
                    {
                        break;
                    }

                    if (node->children.contains(p))
                    {
                        node = &m_treeNodes[node->children[p]];
                        continue;
                    }

                    // can't find the path
                    MG_CORE_ASSERT(false);
                }

                for (const auto& [item, treeNodeIndex] : node->children)
                {
                    ImGui::TableNextColumn();
                    
                    bool        isDirectory = std::filesystem::is_directory(Project::getActiveAssetDirectory() / item);
                    std::string itemString  = item.generic_string();

                    ImGui::PushStyleColor(ImGuiCol_Button, { 0, 0, 0, 0 });
                    contentBrowserButton(itemString.c_str(),
                                         isDirectory ?
                                         (ImTextureID)m_directoryIcon->getRendererID() :
                                         (ImTextureID)m_fileIcon->getRendererID(),
                                         false);
                    
                    if (ImGui::BeginPopupContextItem())
                    {
                        if (ImGui::MenuItem("Delete"))
                        {
                            MG_CORE_ASSERT(false, "Not implemented");
                        }
                        ImGui::EndPopup();
                    }

                    if (ImGui::BeginDragDropSource())
                    {
                        AssetHandle handle = m_treeNodes[treeNodeIndex].handle;
                        ImGui::SetDragDropPayload(MG_DRAG_PAYLOAD_CB_ITEM, &handle, sizeof(AssetHandle));
                        ImGui::EndDragDropSource();
                    }

                    ImGui::PopStyleColor();
                    
                    if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
                    {
                        if (isDirectory)
                        {
                            m_currentDirectory /= item.filename();
                        }
                    }

                }
            }
            else
            {
                for (auto& directoryIterator : std::filesystem::directory_iterator(m_currentDirectory))
                {
                    ImGui::TableNextColumn();
                    const auto& path           = directoryIterator.path();
                    std::string filenameString = path.filename().string();
                    bool isDirectory           = directoryIterator.is_directory();

                    ImGui::PushStyleColor(ImGuiCol_Button, { 0, 0, 0, 0});
                    contentBrowserButton(filenameString.c_str(), 
                                         isDirectory ?
                                         (ImTextureID)m_directoryIcon->getRendererID() : 
                                         (ImTextureID)m_fileIcon->getRendererID(),
                                         path == m_currentRenameEntry);

                    std::filesystem::path relativePath = std::filesystem::relative(path, Project::getActiveAssetDirectory());
                    if (ImGui::BeginDragDropSource())
                    {
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
                            m_currentDirectory /= filenameString;
                            m_pathStack.push_back(filenameString);
                        }
                        else
                        {
                            std::filesystem::path relativePath = std::filesystem::relative(path, m_baseDirectory);

                            Services::eventBus()->emit(RequestSceneLoadEvent(relativePath));
                        }
                    }
                    else if (ImGui::IsMouseClicked(ImGuiMouseButton_Right) && ImGui::IsItemHovered())
                    {
                        ImGui::OpenPopup(filenameString.c_str());
                    }

                    if (ImGui::BeginPopup(filenameString.c_str()))
                    {
                        itemPopupOpened = true;
                        
                        if (ImGui::MenuItem("Import"))
                        {
                            Project::getActive()->getEditorAssetManager()->importAsset(relativePath);
                            refreshAssetTree();
                        }

                        if (ImGui::MenuItem("Open"))
                        {
                            std::filesystem::path relativePath = std::filesystem::relative(path, m_baseDirectory);

                            Services::eventBus()->emit(RequestSceneLoadEvent(relativePath));
                        }

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
                            auto pathToOpen = m_currentDirectory / filenameString;
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
            }
            ImGui::EndTable();
        }

        #ifdef _WIN32
        if (!itemPopupOpened && ImGui::BeginPopupContextWindow(0, ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems))
        {
            if (ImGui::MenuItem("Show in Explorer"))
            {
                cmdShowInExplorer(m_currentDirectory);
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

        return true;
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

    void ContentBrowserPanel::refreshAssetTree()
    {
        const auto& assetRegistry = Project::getActive()->getEditorAssetManager()->getAssetRegistry();

        m_treeNodes.reserve(assetRegistry.size()); // Reserve space in advance to avoid reallocations

        for (const auto& [handle, metadata] : assetRegistry)
        {
            uint32_t currentNodeIndex = 0;

            for (const auto& p : metadata.filepath)
            {
                auto& currentNode = m_treeNodes[currentNodeIndex];
                auto it = currentNode.children.find(p.generic_string());

                if (it != currentNode.children.end())
                {
                    currentNodeIndex = it->second;
                }
                else
                {
                    // add node
                    TreeNode newNode(p, handle);

                    newNode.parent          = currentNodeIndex;
                    currentNodeIndex        = m_treeNodes.size();
                    currentNode.children[p] = currentNodeIndex;

                    m_treeNodes.push_back(std::move(newNode));
                }
            }
        }
    }
}
