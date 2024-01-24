#pragma once

#include "Scene.h"

#include <filesystem>

namespace mango
{
    class SceneSerializer
    {
    public:
        SceneSerializer() = delete;
        ~SceneSerializer() = delete;

        static bool serialize      (const std::shared_ptr<Scene>& scene, const std::filesystem::path& outFilepath);
        static bool serializeBinary(const std::shared_ptr<Scene>& scene, const std::filesystem::path& outFilepath);

        static std::shared_ptr<Scene> deserialize      (const std::filesystem::path& inFilepath);
        static std::shared_ptr<Scene> deserializeBinary(const std::filesystem::path& inFilepath);
    };
}