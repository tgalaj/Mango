#pragma once

#include "Model.h"

class GeomPrimitive
{
public:
    GeomPrimitive()  = delete;
    ~GeomPrimitive() = delete;

    friend class Model;

private:
    static void genCube (VEBuffers &buffers, float radius);
    static void genTorus(VEBuffers &buffers, float innerRadius, float outerRadius, unsigned int slices, unsigned int stacks);
};

