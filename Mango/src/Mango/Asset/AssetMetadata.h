#pragma once

#include "Asset.h"

#include <filesystem>

namespace mango
{
    // It only exists in the Editor
    struct AssetMetadata
    {
        std::filesystem::path filepath          = "";
        AssetType             type              = AssetType::None;
        bool                  isMemoryOnlyAsset = false;

        bool isValid() const { return type != AssetType::None && !isMemoryOnlyAsset; }
    };
}