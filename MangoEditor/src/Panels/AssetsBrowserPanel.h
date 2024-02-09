#pragma once

#include "Mango/Rendering/Texture.h"

#include <filesystem>

namespace mango
{
    class AssetsBrowserPanel
    {
    public:
        AssetsBrowserPanel();

        void onGui();

        float thumbnailPadding = 6.0f;
        float thumbnailSize    = 96.0f;
        float backButtonSize   = 32.0f;

    private:
        std::vector<std::string> m_pathStack;
        std::filesystem::path m_basePath;
        std::filesystem::path m_currentPath;

        std::shared_ptr<Texture> m_folderTexture;
        std::shared_ptr<Texture> m_fileTexture;
        std::shared_ptr<Texture> m_leftArrow;
    };
}