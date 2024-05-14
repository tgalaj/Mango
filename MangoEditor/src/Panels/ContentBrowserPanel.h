#pragma once

#include "Mango/Core/Base.h"
#include "Mango/Rendering/Texture.h"

#include <filesystem>
#include <map>
#include <set>

namespace mango
{
    class ContentBrowserPanel
    {
    public:
        ContentBrowserPanel();

        void onGui();

        float thumbnailPadding = 6.0f;
        float thumbnailSize    = 96.0f;
        float backButtonSize   = 32.0f;

    private:
        using RenameEntry = std::pair<const std::filesystem::path, bool>;

        bool contentBrowserButton(const char* label, void* textureID, bool rename = false);

        void cmdShowInExplorer(std::filesystem::path path);
        void cmdOpenExternally(std::filesystem::path path);
        void cmdDelete(std::filesystem::path path);
        void refreshAssetTree();

    private:
        std::vector<std::string> m_pathStack;
        std::filesystem::path m_baseDirectory;
        std::filesystem::path m_currentDirectory;
        std::filesystem::path m_currentRenameEntry;

        ref<Texture> m_directoryIcon;
        ref<Texture> m_fileIcon;

        struct TreeNode
        {
            std::map<std::filesystem::path, uint32_t> children;
            std::filesystem::path                     path;

            AssetHandle handle = 0;
            uint32_t    parent = (uint32_t)-1;


            TreeNode(const std::filesystem::path& path, AssetHandle handle)
                : path(path), handle(handle) {}
        };

        std::vector<TreeNode> m_treeNodes;

        std::map<std::filesystem::path, std::vector<std::filesystem::path>> m_assetTree;

        enum class Mode
        {
            Asset = 0, FileSystem = 1
        };

        Mode m_currentMode = Mode::FileSystem;
    };
}