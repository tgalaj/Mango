#pragma once

#include <deque>

#include "Camera.h"
#include "Model.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Skybox.h"
#include "ParticleEffect.h"

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

    std::deque<ParticleEffect *> particle_effects;

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
        GLuint to_color_id, to_depth_id, fbo_id;
        int cube_size;
        glm::mat4 proj;

        ReflectiveObj()
        {
            cube_size = 512;
            float half_size = cube_size * 0.5f;
            proj = glm::perspective(2.0f * glm::atan(half_size / (half_size - 0.5f)), 1.0f, 0.01f, 1000.0f);

            to_color_id = 0;
            to_depth_id = 0;
            fbo_id = 0;

            glGenFramebuffers(1, &fbo_id);
            glBindFramebuffer(GL_FRAMEBUFFER, fbo_id);

            glGenTextures(1, &to_color_id);
            glBindTexture(GL_TEXTURE_CUBE_MAP, to_color_id);

            glTexStorage2D(GL_TEXTURE_CUBE_MAP, 1, GL_RGBA8, cube_size, cube_size);

            glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

            glGenTextures(1, &to_depth_id);
            glBindTexture(GL_TEXTURE_CUBE_MAP, to_depth_id);
            
            glTexStorage2D(GL_TEXTURE_CUBE_MAP, 1, GL_DEPTH_COMPONENT32F, cube_size, cube_size);
                       
            glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

            //glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X, to_color_id, 0);
            glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, to_color_id, 0);
            glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, to_depth_id, 0);

            GLenum color_attachments[] = { GL_COLOR_ATTACHMENT0 };
            glDrawBuffers(1, color_attachments);
        }

        ~ReflectiveObj()
        {
            if (to_color_id != 0)
            {
                glDeleteTextures(1, &to_color_id);
                to_color_id = 0;
            }

            if (to_depth_id != 0)
            {
                glDeleteTextures(1, &to_depth_id);
                to_depth_id = 0;
            }

            if (fbo_id != 0)
            {
                glDeleteFramebuffers(1, &fbo_id);
                fbo_id = 0;
            }
        }
    };
};

