#pragma once

#include <deque>

#include "Camera.h"
#include "Model.h"

class Renderer final
{
public:
    friend class VertexEngineCore;
    friend class Scene;

    void setClearColor(float r, float g, float b, float a);

    Camera * const getCamera();

private:
    Renderer();
    ~Renderer();

    std::deque<Model *> models;

    Camera * cam;
    Shader * currentShader;

    bool shouldUpdateCamUniforms;

    void render();
    void addModel(Model * m);
};

