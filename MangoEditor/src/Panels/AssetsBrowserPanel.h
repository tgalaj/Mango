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

    private:
        std::filesystem::path m_basePath;
        std::filesystem::path m_currentPath;

        std::shared_ptr<Texture> m_folderTexture;
        std::shared_ptr<Texture> m_fileTexture;
    };
}