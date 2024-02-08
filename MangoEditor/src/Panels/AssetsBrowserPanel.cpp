#include "AssetsBrowserPanel.h"

#include "Mango/Core/VFI.h"

#include <imgui.h>

namespace mango
{

    AssetsBrowserPanel::AssetsBrowserPanel()
    {
        auto searchPath = VFI::getSearchPath();
        
        m_basePath    = searchPath[searchPath.size() - 1];
        m_currentPath = m_basePath;
    }

    void AssetsBrowserPanel::onGui()
    {
        ImGui::Begin("Assets Browser Panel");

        if (m_currentPath != m_basePath)
        {
            if (ImGui::Button("<-"))
            {
                m_currentPath = m_currentPath.parent_path();
            }
        }

        for (auto& directoryIterator : std::filesystem::directory_iterator(m_currentPath))
        {
            auto directoryPath = directoryIterator.path();
            auto relativePath  = std::filesystem::relative(directoryPath, m_basePath / "..");

            if (directoryIterator.is_directory())
            {
                auto dirname = relativePath.filename();
                if (ImGui::Button(dirname.string().c_str()))
                {
                    m_currentPath /= dirname;
                }
            }
            else
            {
                auto filename = relativePath.filename();
                if (ImGui::Button(filename.string().c_str()))
                {
                    
                }
            }
        }

        ImGui::End();
    }
}