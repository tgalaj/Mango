#pragma once

#include <filesystem>

namespace mango
{
    class AssetsBrowserPanel
    {
    public:
        AssetsBrowserPanel();

        void onGui();

    private:
        std::filesystem::path m_basePath;
        std::filesystem::path m_currentPath;
    };
}