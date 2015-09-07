#pragma once

#include <deque>
#include "Model.h"

class Renderer
{
    friend class VertexEngineCore;

public:
    Renderer();
    virtual ~Renderer();

private:
    std::deque<Model *> models;

    void render();
};

