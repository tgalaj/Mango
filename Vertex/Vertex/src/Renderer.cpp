#include "Renderer.h"
#include "ShaderManager.h"

Renderer::Renderer()
    : currentShader          (nullptr),
      shouldUpdateCamUniforms(true),
      cachedDirCount         (-1),
      cachedPointCount       (-1),
      cachedSpotCount        (-1)
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_FRAMEBUFFER_SRGB);

    glClearColor(0.382f, 0.618f, 1.0f, 1.0f);

    /* Initialize engine's default shaders */
    ShaderManager::createShader("ve_basic",
                                "res/shaders/Basic.vert", 
                                "res/shaders/Basic.frag");
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

    if (currentShader)
    {
        setupLightsUniforms();
    }

    for (auto & model : models)
    {
        if (model->shader != currentShader && model->shader != nullptr)
        {
            currentShader = model->shader;
            currentShader->apply();

            setupLightsUniforms();
        }

        currentShader->setUniform3fv      ("camPos",   cam->getPosition());
        currentShader->setUniformMatrix4fv("viewProj", cam->getViewProjection());

        model->render();
    }
}

void Renderer::setupLightsUniforms()
{
    if (cachedDirCount != dirLights.size())
    {
        currentShader->setUniform1ui("dirUsed",   dirLights.size());
        cachedDirCount = dirLights.size();
    }

    if (cachedPointCount != pointLights.size())
    {
        currentShader->setUniform1ui("pointUsed", pointLights.size());
        cachedPointCount = pointLights.size();
    }

    if (cachedSpotCount != spotLights.size())
    {
        currentShader->setUniform1ui("spotUsed",  spotLights.size());
        cachedSpotCount = spotLights.size();
    }

    for (size_t i = 0; i < dirLights.size(); ++i)
    {
        if(dirLights[i]->isDirty)
        {
            std::string idx = std::to_string(i);

            currentShader->setUniform3fv("dirLights[" + idx + "].direction", dirLights[i]->direction);
            currentShader->setUniform3fv("dirLights[" + idx + "].ambient",   dirLights[i]->ambient);
            currentShader->setUniform3fv("dirLights[" + idx + "].diffuse",   dirLights[i]->diffuse);
            currentShader->setUniform3fv("dirLights[" + idx + "].specular",  dirLights[i]->specular);

            dirLights[i]->isDirty = false;
        }
    }

    for (size_t i = 0; i < pointLights.size(); ++i)
    {
        if(pointLights[i]->isDirty)
        {
            std::string idx = std::to_string(i);

            currentShader->setUniform3fv("pointLights[" + idx + "].position",  pointLights[i]->position);
            currentShader->setUniform3fv("pointLights[" + idx + "].ambient",   pointLights[i]->ambient);
            currentShader->setUniform3fv("pointLights[" + idx + "].diffuse",   pointLights[i]->diffuse);
            currentShader->setUniform3fv("pointLights[" + idx + "].specular",  pointLights[i]->specular);

            currentShader->setUniform3fv("pointLights[" + idx + "].attenuations", glm::vec3(pointLights[i]->constantAttenuation, 
                                                                                            pointLights[i]->linearAttenuation,
                                                                                            pointLights[i]->quadraticAttenuation));
            pointLights[i]->isDirty = false;
        }
    }

    for (size_t i = 0; i < spotLights.size(); ++i)
    {
        if(spotLights[i]->isDirty)
        {
            std::string idx = std::to_string(i);

            currentShader->setUniform3fv("spotLights[" + idx + "].position",  spotLights[i]->position);
            currentShader->setUniform3fv("spotLights[" + idx + "].direction", spotLights[i]->direction);
            currentShader->setUniform3fv("spotLights[" + idx + "].ambient",   spotLights[i]->ambient);
            currentShader->setUniform3fv("spotLights[" + idx + "].diffuse",   spotLights[i]->diffuse);
            currentShader->setUniform3fv("spotLights[" + idx + "].specular",  spotLights[i]->specular);

            currentShader->setUniform3fv("spotLights[" + idx + "].attenuations", glm::vec3(spotLights[i]->constantAttenuation, 
                                                                                           spotLights[i]->linearAttenuation,
                                                                                           spotLights[i]->quadraticAttenuation));
            currentShader->setUniform2fv("spotLights[" + idx + "].angles",       glm::vec2(glm::cos(glm::radians(spotLights[i]->innerCutOffAngle)), 
                                                                                           glm::cos(glm::radians(spotLights[i]->outerCutOffAngle))));
            spotLights[i]->isDirty = false;
        }
    }
}
