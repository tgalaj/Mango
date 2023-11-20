#pragma once

#include "Event.h"


namespace mango
{
    class Scene;

    struct ActiveSceneChangedEvent : Event
    {
        ActiveSceneChangedEvent(Scene* const scene)
            : scene(scene) {}

        Scene* const scene;
    };
}