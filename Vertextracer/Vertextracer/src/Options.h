#pragma once
#include <glm/glm.hpp>
#include <iostream>

struct Options
{
    enum class AAType { STOCHASTIC, ADAPTIVE, FXAA };

    std::string OUTPUT_FILE_NAME  = "output";
    uint32_t WIDTH                = 640;
    uint32_t HEIGHT               = 480;
    bool RENDER_MULTI_FRAMES      = false;
    uint32_t NUM_MULTI_FRAMES_MIN = 0;
    uint32_t NUM_MULTI_FRAMES_MAX = 128;

    glm::highp_dvec3 CAM_ORIGIN       = glm::highp_dvec3(0.0f);
    glm::highp_dvec3 CAM_UP           = glm::highp_dvec3(0.0f, -1.0f, 0.0f);
    double CAM_PITCH                  = 0.0f;
    double CAM_YAW                    = -90.0f;
    double CAM_FOV                    = 60.0f;

    double SHADOW_BIAS                = 1e-4f;
    bool ENABLE_AERIAL_PERSPECTIVE    = true;
    uint32_t REFLECTION_MAX_DEPTH     = 5;
    glm::highp_dvec3 BACKGROUND_COLOR = glm::highp_dvec3(0.235294f, 0.67451f, 0.843137f);

    AAType ANTYALIASING_TYPE = AAType::FXAA;
    bool ENABLE_ANTIALIASING = false;

    /* Adaptive AA params */
    uint32_t ADAPTIVE_AA_MAX_DEPTH = 2;
    double ADAPTIVE_AA_EPSILON     = 0.1f;

    /* Stochastic AA params*/
    int STOCHASTIC_AA_NUM_SAMPLES = 4;

    /* FXAA params */
    double FXAA_SPAN_MAX         = 8.0;
    double FXAA_REDUCE_MIN       = 1.0 / 128.0;
    double FXAA_REDUCE_MUL       = 1.0 / 8.0;
    uint32_t FXAA_NUM_ITERATIONS = 1;

    void printConfiguration() const
    {
        std::cout << "Configuration of Vertextracer\n" << std::endl;
         
        std::cout << "Resolution             = " << WIDTH << " x " << HEIGHT                 << std::endl;
        std::cout << "Render multiple frames = " << (RENDER_MULTI_FRAMES ? "true" : "false") << std::endl;

        if (RENDER_MULTI_FRAMES)
        {
            std::cout << "Number of frames to render = " << NUM_MULTI_FRAMES_MAX - NUM_MULTI_FRAMES_MIN << std::endl;
        }

        std::cout << std::endl;

        std::cout << "Camera origin        = [" << CAM_ORIGIN.x << ", " << CAM_ORIGIN.y << ", " << CAM_ORIGIN.z << "]" << std::endl;
        std::cout << "Camera up            = [" << CAM_UP.x << ", " << CAM_UP.y << ", " << CAM_UP.z << "]"             << std::endl;
        std::cout << "Camera field of view = "  << CAM_FOV                                                             << std::endl;
        std::cout << "Camera pitch         = "  << CAM_PITCH                                                           << std::endl;
        std::cout << "Camera yaw           = "  << CAM_YAW                                                             << std::endl;

        std::cout << std::endl;

        std::cout << "Reflection Max Depth = " << REFLECTION_MAX_DEPTH                       << std::endl;
        std::cout << "Aerial Perspective   = " << (ENABLE_AERIAL_PERSPECTIVE ? "ON" : "OFF") << std::endl;

        std::cout << std::endl;

        if (ENABLE_ANTIALIASING)
        {
            switch (ANTYALIASING_TYPE)
            {
                case AAType::ADAPTIVE:
                {
                    std::cout << "Adaptive AA max depth = " << ADAPTIVE_AA_MAX_DEPTH << std::endl;
                    std::cout << "Adaptive AA epsilon   = " << ADAPTIVE_AA_EPSILON   << std::endl;
                    break;
                }
                case AAType::STOCHASTIC:
                {
                    std::cout << "Stochastic AA num samples = " << STOCHASTIC_AA_NUM_SAMPLES << std::endl;
                    break;
                }
                case AAType::FXAA:
                {
                    std::cout << "FXAA iterations              = " << FXAA_NUM_ITERATIONS << std::endl;
                    std::cout << "FXAA max blur direction span = " << FXAA_SPAN_MAX       << std::endl;
                    std::cout << "FXAA reduce min              = " << FXAA_REDUCE_MIN     << std::endl;
                    std::cout << "FXAA reduce mul              = " << FXAA_REDUCE_MUL     << std::endl;
                    break;
                }
            }
        }

        std::cout << std::endl;
    }
};
