#pragma once

#include "Model.h"

class GeomPrimitive
{
public:
    GeomPrimitive()  = delete;
    ~GeomPrimitive() = delete;

    friend class Model;

private:
    static void genCube(VEBuffers &buffers, float radius = 1.0f);
};

