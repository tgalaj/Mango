#include "mgpch.h"
#include "AssetImporter.h"

namespace mango
{
    std::unordered_map<mango::AssetType, mango::scope<mango::AssetImporterBase>> AssetImporter::s_importers;

    void AssetImporter::init()
    {
        s_importers.clear();

        s_importers[AssetType::Scene]   = createScope<SceneImporter>();
        s_importers[AssetType::Texture] = createScope<TextureImporter>();
    }
    
    void AssetImporter::serialize(const AssetMetadata& metadata, const ref<Asset>& asset)
    {
        if (!s_importers.contains(metadata.type))
        {
            MG_CORE_ERROR("No importer available for the asset type: {}", assetTypeToString(metadata.type));
            return;
        }

        s_importers.at(metadata.type)->serialize(metadata, asset);
    }

    ref<Asset> AssetImporter::import(AssetHandle handle, const AssetMetadata & metadata)
    {
        if (!s_importers.contains(metadata.type))
        {
            MG_CORE_ERROR("No importer available for the asset type: {}", assetTypeToString(metadata.type));
            return nullptr;
        }

        return s_importers.at(metadata.type)->import(handle, metadata);
    }

    void TextureImporter::serialize(const AssetMetadata& metadata, const ref<Asset>& asset) const
    {
        // mo op
    }

    ref<Asset> TextureImporter::import(AssetHandle handle, const AssetMetadata & metadata) const
    {
        // TODO
        return nullptr;
    }

    void SceneImporter::serialize(const AssetMetadata& metadata, const ref<Asset>& asset) const
    {
        // TODO
    }

    ref<Asset> SceneImporter::import(AssetHandle handle, const AssetMetadata & metadata) const
    {
        // TODO
        return nullptr;
    }

}
