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
        static ref<Mesh> createDebugDirLight();
        static ref<Mesh> createDebugSpotLight();
        static ref<Mesh> createDebugCameraFrustum();

    private:
        inline static ref<Mesh> s_debugBox           = nullptr;
        inline static ref<Mesh> s_debugCapsule       = nullptr;
        inline static ref<Mesh> s_debugSphere        = nullptr;
        inline static ref<Mesh> s_debugCone          = nullptr;
        inline static ref<Mesh> s_debugDirLight      = nullptr;
        inline static ref<Mesh> s_debugSpotLight     = nullptr;
        inline static ref<Mesh> s_debugCameraFrustum = nullptr;
    };
}