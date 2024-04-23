#pragma once
#include "Mango/Core/Base.h"
#include "Mango/Rendering/Mesh.h"

namespace mango
{
    class DebugMesh
    {   
    public:
        static ref<Mesh> createDebugBox();
        static ref<Mesh> createDebugCapsule();
        static ref<Mesh> createDebugSphere();
        static ref<Mesh> createDebugCone();

    private:
        inline static ref<Mesh> s_debugBox     = nullptr;
        inline static ref<Mesh> s_debugCapsule = nullptr;
        inline static ref<Mesh> s_debugSphere  = nullptr;
        inline static ref<Mesh> s_debugCone    = nullptr;
    };
}