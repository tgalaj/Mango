#pragma once

#include "Mango/Core/Base.h"
#include "Mango/Core/UUID.h"

namespace mango
{
    using AssetHandle = UUID;

    enum class AssetType : uint16_t
    {
        None = 0,
        Scene,
        MeshData,
        Mesh, // aka dynamic, animated mesh
        StaticMesh,
        Material,
        Texture,
        EnvMap,
        Prefab,
        PhysicsMaterial,
        Audio,
        SoundConfig,
        Script,
    };

    const char* assetTypeToString  (AssetType type);
    AssetType   assetTypeFromString(const char* type);

    class Asset
    {
    public:
        AssetHandle handle; // automatically generate handle (see UUID constructor)

        virtual AssetType getAssetType() const = 0;
    };
}