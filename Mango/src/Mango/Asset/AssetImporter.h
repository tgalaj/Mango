#pragma once

#include "AssetMetadata.h"

#include <unordered_map>

namespace mango
{
    class AssetImporterBase;

    class AssetImporter
    {
    public:
        static void       init();
        static void       serialize(const AssetMetadata& metadata, const ref<Asset>&    asset);
        static ref<Asset> import   (      AssetHandle    handle,   const AssetMetadata& metadata);
    
    private:
        static std::unordered_map<AssetType, scope<AssetImporterBase>> s_importers;
    };

    class AssetImporterBase
    {
    public:
        virtual void       serialize(const AssetMetadata& metadata, const ref<Asset>&    asset)    const = 0;
        virtual ref<Asset> import(         AssetHandle    handle,   const AssetMetadata& metadata) const = 0;
    };

    class TextureImporter : public AssetImporterBase
    {
        virtual void       serialize(const AssetMetadata& metadata, const ref<Asset>&    asset)    const override;
        virtual ref<Asset> import   (      AssetHandle    handle,   const AssetMetadata& metadata) const override;
    };

    class SceneImporter : public AssetImporterBase
    {
        virtual void       serialize(const AssetMetadata& metadata, const ref<Asset>&    asset)    const override;
        virtual ref<Asset> import   (      AssetHandle    handle,   const AssetMetadata& metadata) const override;
    };
}