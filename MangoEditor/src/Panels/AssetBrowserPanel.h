#pragma once

#include "Mango/Core/Base.h"
#include "Mango/Rendering/Texture.h"

#include <filesystem>

namespace mango
{
    class AssetBrowserPanel
    {
    public:
        AssetBrowserPanel();

        void onGui();

        float thumbnailPadding = 6.0f;
        float thumbnailSize    = 96.0f;
        float backButtonSize   = 32.0f;

    private:
        std::vector<std::string> m_pathStack;
        std::filesystem::path m_basePath;
        std::filesystem::path m_currentPath;

        ref<Texture> m_folderIcon;
        ref<Texture> m_fileIcon;
        ref<Texture> m_returnIcon;
    };
}