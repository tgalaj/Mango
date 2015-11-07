#include "Renderer.h"

Renderer::Renderer()
    : currentShader          (nullptr),
      shouldUpdateCamUniforms(true)
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_FRAMEBUFFER_SRGB);

    glClearColor(0.22f, 0.33f, 0.66f, 1.0f);
}

Renderer::~Renderer()
{
}

void Renderer::setClearColor(float r, float g, float b, float a)
{
    glClearColor(r, g, b, a);
}

Camera * const Renderer::getCamera()
{
    return cam;
}

void Renderer::render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (auto & model : models)
    {
        if (model->shader != currentShader)
        {
            currentShader = model->shader;
            currentShader->apply();
        }

        currentShader->setUniformMatrix4fv("view",     cam->getView());
        currentShader->setUniformMatrix4fv("viewProj", cam->getViewProjection());

        model->render();
    }
}

void Renderer::addModel(Model * m)
{
    models.push_back(m);
}