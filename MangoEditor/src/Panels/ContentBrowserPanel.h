#pragma once

#include "Mango/Core/Base.h"
#include "Mango/Rendering/Texture.h"

#include <filesystem>

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
        bool contentBrowserButton(const char* label, void* textureID, bool rename = false);

        using RenameEntry = std::pair<const std::filesystem::path, bool>;

        void cmdShowInExplorer(std::filesystem::path path);
        void cmdOpenExternally(std::filesystem::path path);
        void cmdDelete(std::filesystem::path path);

    private:
        std::vector<std::string> m_pathStack;
        std::filesystem::path m_basePath;
        std::filesystem::path m_currentPath;
        std::filesystem::path m_currentRenameEntry;

        ref<Texture> m_folderIcon;
        ref<Texture> m_fileIcon;
    };
}