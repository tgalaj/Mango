//#include "mgpch.h"
//#include "SceneImporter.h"
//
//#include "Mango/Project/Project.h"
//#include "Mango/Scene/SceneSerializer.h"
//
//namespace mango
//{
//    ref<Scene> SceneImporter::importScene(AssetHandle handle, const AssetMetadata& metadata)
//    {
//        MG_PROFILE_ZONE_SCOPED;
//
//        return loadScene(Project::getActiveAssetDirectory() / metadata.filepath);
//    }
//
//    ref<Scene> SceneImporter::loadScene(const std::filesystem::path& path)
//    {
//        MG_PROFILE_ZONE_SCOPED;
//
//        return SceneSerializer::deserialize(path);
//    }
//
//    void SceneImporter::saveScene(ref<Scene> scene, const std::filesystem::path& path)
//    {
//        MG_PROFILE_ZONE_SCOPED;
//
//        if (!SceneSerializer::serialize(scene, Project::getActiveAssetDirectory() / path))
//        {
//            MG_CORE_ERROR("Could not save scene to file: {0}", path.string());
//        }
//    }
//
//}