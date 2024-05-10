#pragma once

#include "Mango/Events/Event.h"

#include <filesystem>

namespace mango
{
    struct RequestSceneLoadEvent : Event
    {
        RequestSceneLoadEvent(const std::filesystem::path& scenePath)
            :  scenePath(scenePath) {}

        std::filesystem::path scenePath;
    };
}