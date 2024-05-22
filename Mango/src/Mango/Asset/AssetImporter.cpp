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

    bool AssetImporter::import(const AssetMetadata & metadata, ref<Asset> & asset)
    {
        if (!s_importers.contains(metadata.type))
        {
            MG_CORE_ERROR("No importer available for the asset type: {}", assetTypeToString(metadata.type));
            return false;
        }

        return s_importers.at(metadata.type)->import(metadata, asset);
    }

    void TextureImporter::serialize(const AssetMetadata& metadata, const ref<Asset>& asset) const
    {
        // no op
    }

    bool TextureImporter::import(const AssetMetadata & metadata, ref<Asset> & asset) const
    {
        return false;
    }

    void SceneImporter::serialize(const AssetMetadata& metadata, const ref<Asset>& asset) const
    {

    }

    bool SceneImporter::import(const AssetMetadata & metadata, ref<Asset> & asset) const
    {
        return false;
    }

}
