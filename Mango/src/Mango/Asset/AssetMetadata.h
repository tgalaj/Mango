#pragma once

#include "Asset.h"

#include <filesystem>

namespace mango
{
    // It only exists in the Editor
    struct AssetMetadata
    {
        AssetHandle           handle            = 0;
        AssetType             type              = AssetType::None;
        std::filesystem::path filepath          = "";

        bool                  isMemoryOnlyAsset = false;

        bool isValid() const { return handle != 0 && !isMemoryOnlyAsset; }
    };
}