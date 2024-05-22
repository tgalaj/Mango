#pragma once

#include "Asset.h"

#include <filesystem>

namespace mango
{
    // It only exists in the Editor
    struct AssetMetadata
    {
        std::filesystem::path filepath       = "";
        AssetHandle           handle         = 0;
        AssetType             type           = AssetType::None;
        bool                  isMemoryAsset  = false;

        bool isValid() const { return handle != 0 && !isMemoryAsset; }
    };
}