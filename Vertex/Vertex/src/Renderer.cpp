#include "Renderer.h"
#include "ShaderManager.h"

Renderer::Renderer()
    : cam                    (nullptr),
      currentShader          (nullptr),
      shouldUpdateCamUniforms(true),
      cachedDirCount         (-1),
      cachedPointCount       (-1),
      cachedSpotCount        (-1),
      skybox                 (nullptr)
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_FRAMEBUFFER_SRGB);

    //glClearColor(0.382f, 0.618f, 1.0f, 1.0f);
    glClearColor(1, 1, 1, 1);

    /* Initialize engine's default shaders */
    ShaderManager::createShader("ve_basic",
                                "res/shaders/Basic.vert", 
                                "res/shaders/Basic.frag");

    ShaderManager::createShader("ve_skybox",
                                "res/shaders/Skybox.vert", 
                                "res/shaders/Skybox.frag");

    ShaderManager::createShader("ve_reflective",
                                "res/shaders/Reflection.vert", 
                                "res/shaders/Reflection.frag");
}

Renderer::~Renderer()
{
    delete skybox;
    skybox = nullptr;

    for (int i = 0; i < reflectiveTextures.size(); ++i)
    {
        delete reflectiveTextures[i];
        reflectiveTextures[i] = nullptr;
    }
}

void Renderer::setClearColor(float r, float g, float b, float a)
{
    glClearColor(r, g, b, a);
}

void Renderer::createSkybox(const std::string & pos_x, const std::string & neg_x, const std::string & pos_y, const std::string & neg_y, const std::string & pos_z, const std::string & neg_z)
{
    std::string filenames[] = { pos_x, neg_x, pos_y, neg_y, pos_z, neg_z };

    skybox = new Skybox();
    skybox->loadImages(filenames);

    if(cam)
    {
        skybox->setSize(cam->getFarPlane());
    }
}

Camera * const Renderer::getCamera()
{
    return cam;
}

glm::vec3 vectors[] = { glm::vec3( 1.0f,  0.0f,  0.0f), 
                        glm::vec3(-1.0f,  0.0f,  0.0f), 
                        glm::vec3( 0.0f,  1.0f,  0.001f), 
                        glm::vec3( 0.0f, -1.0f,  0.001f), 
                        glm::vec3( 0.0f,  0.0f,  1.0f), 
                        glm::vec3( 0.0f,  0.0f, -1.0f) };

void Renderer::render()
{
    for(int i = 0; i < reflectiveTextures.size(); ++i)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, reflectiveTextures[i]->fbo_id);
        glViewport(0, 0, reflectiveTextures[i]->cube_size, reflectiveTextures[i]->cube_size);

        for(int j = 0; j < 6; ++j)
        {
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + j, reflectiveTextures[i]->to_id, 0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            for (auto & model : models)
            {
                if (model->shader != currentShader && model->shader != nullptr)
                {
                    currentShader = model->shader;
                    currentShader->apply();
                }

                if (currentShader)
                {
                    currentShader->unlockUBOs();
                    setupLightsUniforms();
                }
               
                currentShader->setUniform3fv      ("camPos", glm::vec3(reflectiveModels[i]->worldTransform[3]));
                currentShader->setUniformMatrix4fv("viewProj", reflectiveTextures[i]->proj * glm::lookAt(glm::vec3(reflectiveModels[i]->worldTransform[3]), vectors[j], glm::vec3(0, -1, 0)) );

                model->render();

                if (currentShader)
                {
                    currentShader->lockUBOs();
                }
            }
        }
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, cam->geViewportWidth(), cam->geViewportHeight());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /* Draw scene normally */
    for (auto & model : models)
    {
        if (model->shader != currentShader && model->shader != nullptr)
        {
            currentShader = model->shader;
            currentShader->apply();
        }

        if (currentShader)
        {
            currentShader->unlockUBOs();
            setupLightsUniforms();
        }

        currentShader->setUniform3fv("camPos", cam->getPosition());
        currentShader->setUniformMatrix4fv("viewProj", cam->getViewProjection());

        model->render();

        if (currentShader)
        {
            currentShader->lockUBOs();
        }
    }

    if (skybox)
    {
        currentShader = ShaderManager::getShader("ve_skybox");
        currentShader->apply();

        currentShader->setUniformMatrix4fv("wvp", cam->getViewProjection() * skybox->world);
        skybox->render(currentShader, cam);
    }

    if (reflectiveModels.size() > 0)
    {
        currentShader = reflectiveModels[0]->shader;
        currentShader->apply();

        currentShader->setUniform3fv("camPos", cam->getPosition());
        currentShader->setUniformMatrix4fv("viewProj", cam->getViewProjection());

        for (int i = 0; i < reflectiveModels.size(); ++i)
        {
            glActiveTexture(GL_TEXTURE7);
            glBindTexture(GL_TEXTURE_CUBE_MAP, reflectiveTextures[i]->to_id);

            reflectiveModels[i]->render();
            glActiveTexture(GL_TEXTURE0);
        }
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

void Renderer::addReflectiveModel(Model * model)
{
    reflectiveModels.push_back(model);
    model->meshes[0]->textures.clear();
    model->setShader("ve_reflective");

    ReflectiveObj * ro = new ReflectiveObj();

    reflectiveTextures.push_back(ro);
}
