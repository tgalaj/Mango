#pragma once

#include "Asset.h"

#include <filesystem>

namespace mango
{
    struct AssetMetadata
    {
        AssetType             type     = AssetType::None;
        std::filesystem::path filepath = "";

        operator bool() const { return type != AssetType::None; }
    };
}