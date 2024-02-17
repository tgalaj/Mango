#pragma once

#include "Scene.h"

#include <filesystem>

#define MG_SCENE_EXTENSION ".mango"

namespace mango
{
    class SceneSerializer
    {
    public:
        SceneSerializer() = delete;
        ~SceneSerializer() = delete;

        static bool       serialize  (const ref<Scene>& scene, const std::filesystem::path& outFilepath);
        static ref<Scene> deserialize(const std::filesystem::path& inFilepath);
    };
}