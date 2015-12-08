#pragma once

#include <deque>

#include "Camera.h"
#include "Model.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Skybox.h"

class Renderer final
{
public:
    friend class VertexEngineCore;
    friend class Scene;

    void setClearColor(float r, float g, float b, float a);
    void createSkybox(const std::string & pos_x,
                      const std::string & neg_x,
                      const std::string & pos_y,
                      const std::string & neg_y,
                      const std::string & pos_z, 
                      const std::string & neg_z);

    Camera * const getCamera();

private:
    Renderer();
    ~Renderer();

    struct ReflectiveObj;

    std::deque<Model *> models;
    std::deque<Model *> reflectiveModels;
    std::deque<ReflectiveObj *>  reflectiveTextures;

    std::vector<DirectionalLight *> dirLights;
    std::vector<PointLight *>       pointLights;
    std::vector<SpotLight *>        spotLights;

    short cachedDirCount;
    short cachedPointCount;
    short cachedSpotCount;

    Camera * cam;
    Shader * currentShader;
    Skybox * skybox;

    bool shouldUpdateCamUniforms;

    void render();
    void setupLightsUniforms();
    void addReflectiveModel(Model * model);

    struct ReflectiveObj
    {
        GLuint to_id, fbo_id, rbo_id;
        int cube_size;
        glm::mat4 proj;

        ReflectiveObj()
        {
            cube_size = 512;
            proj = glm::perspective(90.0f, 1.0f, 0.1f, 100.0f);

            to_id  = 0;
            fbo_id = 0;
            rbo_id = 0;

            glGenTextures(1, &to_id);
            glBindTexture(GL_TEXTURE_CUBE_MAP, to_id);
            glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

            for (int i = 0; i < 6; ++i)
            {
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, cube_size, cube_size, 0, GL_RGBA, GL_FLOAT, NULL);
            }

            glGenFramebuffers(1, &fbo_id);
            glBindFramebuffer(GL_FRAMEBUFFER, fbo_id);

            glGenRenderbuffers(1, &rbo_id);
            glBindRenderbuffer(GL_RENDERBUFFER, rbo_id);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, cube_size, cube_size);
            //glBindRenderbuffer(GL_RENDERBUFFER, 0);

            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo_id);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X, to_id, 0);

            //glBindFramebuffer(GL_FRAMEBUFFER, 0);
            //glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
        }

        ~ReflectiveObj()
        {
            if (to_id != 0)
            {
                glDeleteTextures(1, &to_id);
                to_id = 0;
            }

            if (fbo_id != 0)
            {
                glDeleteFramebuffers(1, &fbo_id);
                fbo_id = 0;
            }

            if (rbo_id != 0)
            {
                glDeleteRenderbuffers(1, &rbo_id);
                rbo_id = 0;
            }
        }
    };
};

