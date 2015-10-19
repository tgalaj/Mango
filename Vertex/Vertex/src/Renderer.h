#pragma once

#include <deque>

#include "Camera.h"
#include "Model.h"

class Renderer
{
public:
    friend class VertexEngineCore;
    friend class Scene;

    Renderer();
    virtual ~Renderer();

    void setClearColor(float r, float g, float b, float a);

    Camera * const getCamera();

private:
    std::deque<Model *> models;

    Camera * cam;
    Shader * currentShader;

    bool shouldUpdateCamUniforms;

    void render();
    void addModel(Model * m);
};

