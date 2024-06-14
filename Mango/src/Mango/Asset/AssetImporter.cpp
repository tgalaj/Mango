#include "mgpch.h"
#include "AssetImporter.h"

#include "Mango/Project/Project.h"

namespace mango
{
    std::unordered_map<AssetType, scope<AssetImporterBase>> AssetImporter::s_importers;

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
        // no op
    }

    ref<Asset> TextureImporter::import(AssetHandle handle, const AssetMetadata & metadata) const
    {
        auto filepath = Project::getAssetDirectory() / metadata.filepath;

        ref<Texture> asset = createRef<Texture>();

        if (filepath.extension() == ".dds")
        {
            asset->createTextureDDS(filepath.string());
        }
        else
        {
            asset->createTexture2d(filepath.string(), true, 12);
        }

        return asset;
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
