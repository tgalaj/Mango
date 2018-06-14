#pragma once
#include "GameObject.h"

namespace Vertex
{
    class Prefab
    {
    public:
        Prefab() {}
        virtual ~Prefab() {}

        virtual std::shared_ptr<GameObject> create() = 0;
    };
}
