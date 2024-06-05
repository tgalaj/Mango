#pragma once

#include "AssetTypes.h"

#include <unordered_map>

namespace mango
{
    inline static std::unordered_map<std::string, AssetType> s_assetExtensionMap =
    {
        // Mango types
        { ".mango", AssetType::Scene   },

        // Mesh
        { ".gltf", AssetType::MeshData },
        { ".glb",  AssetType::MeshData },
        { ".fbx",  AssetType::MeshData },
        { ".obj",  AssetType::MeshData },

        // Textures
        { ".png",  AssetType::Texture  },
        { ".jpg",  AssetType::Texture  },
        { ".jpeg", AssetType::Texture  },
        { ".dds",  AssetType::Texture  },
        { ".hdr",  AssetType::Texture  },
        { ".exr",  AssetType::Texture  },
        
        // Audio
        { ".wav",  AssetType::Audio    },
        { ".ogg",  AssetType::Audio    },

        // Fonts
        { ".ttf",  AssetType::Font     },
        { ".ttc",  AssetType::Font     },
        { ".otf",  AssetType::Font     },
    };
}