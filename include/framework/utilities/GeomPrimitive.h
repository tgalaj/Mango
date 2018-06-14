#pragma once

#include "framework/rendering/Model.h"

namespace Vertex
{
    class GeomPrimitive
    {
    public:
        GeomPrimitive() = delete;
        ~GeomPrimitive() = delete;

        friend class Model;
        friend class Skybox;

    private:
        static void genCube(VertexBuffers & buffers, float radius);
        static void genCubeMap(VertexBuffers & buffers, float radius);
        static void genTorus(VertexBuffers & buffers, float innerRadius, float outerRadius, unsigned int slices, unsigned int stacks);
        static void genCylinder(VertexBuffers & buffers, float height, float r, unsigned int slices);
        static void genCone(VertexBuffers & buffers, float height, float r, unsigned int slices, unsigned int stacks);
        static void genQuad(VertexBuffers & buffers, float width, float height);
        static void genPlane(VertexBuffers & buffers, float width, float height, unsigned int slices, unsigned int stacks);
        static void genSphere(VertexBuffers & buffers, float r, unsigned int slices);
    };
}

