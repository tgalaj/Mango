#pragma once

#include <deque>

#include "Camera.h"
#include "Model.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"

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

    std::vector<DirectionalLight *> dirLights;
    std::vector<PointLight *>       pointLights;
    std::vector<SpotLight *>        spotLights;

    Camera * cam;
    Shader * currentShader;

    bool shouldUpdateCamUniforms;

    void render();
    void setupLightsUniforms();
};

