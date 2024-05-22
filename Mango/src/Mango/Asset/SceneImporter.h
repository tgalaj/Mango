//#pragma once
//
//#include "Mango/Asset/Asset.h"
//#include "Mango/Asset/AssetMetadata.h"
//
//#include "Mango/Scene/Scene.h"
//
//namespace mango
//{
//    class SceneImporter
//    {
//    public:
//        // AssetMetadata filepath is relative to the project asset directory
//        static ref<Scene> importScene(AssetHandle handle, const AssetMetadata& metadata);
//
//        // Load from the filesystem
//        static ref<Scene> loadScene(const std::filesystem::path& path);
//
//        static void saveScene(ref<Scene> scene, const std::filesystem::path& path);
//    };
//}