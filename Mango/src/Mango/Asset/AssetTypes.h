#pragma once

#include <string_view>

namespace mango
{
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
        Shader,
        Prefab,
        PhysicsMaterial,
        Audio,
        SoundConfig,
        Font,
        Script,
    };

    constexpr inline const char* assetTypeToString(AssetType type)
    {
        switch (type)
        {
        case mango::AssetType::None:            return "None";
        case mango::AssetType::Scene:           return "Scene";
        case mango::AssetType::MeshData:        return "MeshData";
        case mango::AssetType::Mesh:            return "Mesh";
        case mango::AssetType::StaticMesh:      return "StaticMesh";
        case mango::AssetType::Material:        return "Material";
        case mango::AssetType::Texture:         return "Texture";
        case mango::AssetType::EnvMap:          return "EnvMap";
        case mango::AssetType::Prefab:          return "Prefab";
        case mango::AssetType::PhysicsMaterial: return "PhysicsMaterial";
        case mango::AssetType::Audio:           return "Audio";
        case mango::AssetType::SoundConfig:     return "SoundConfig";
        case mango::AssetType::Script:          return "Script";
        }

        return "<Invalid>";

    }

    constexpr inline AssetType assetTypeFromString(std::string_view type)
    {
        if (type == "None")            return AssetType::None;
        if (type == "Scene")           return AssetType::Scene;
        if (type == "MeshData")        return AssetType::MeshData;
        if (type == "Mesh")            return AssetType::Mesh;
        if (type == "StaticMesh")      return AssetType::StaticMesh;
        if (type == "Material")        return AssetType::Material;
        if (type == "Texture")         return AssetType::Texture;
        if (type == "EnvMap")          return AssetType::EnvMap;
        if (type == "Prefab")          return AssetType::Prefab;
        if (type == "PhysicsMaterial") return AssetType::PhysicsMaterial;
        if (type == "Audio")           return AssetType::Audio;
        if (type == "SoundConfig")     return AssetType::SoundConfig;
        if (type == "Script")          return AssetType::Script;

        return AssetType::None;
    }
}