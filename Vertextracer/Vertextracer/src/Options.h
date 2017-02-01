#pragma once
#include <glm/glm.hpp>

struct Options
{
    enum class AAAlgorithm { STOCHASTIC, ADAPTIVE, FXAA };

    std::string output_file_name = "output";
    uint32_t width = 640;
    uint32_t height = 480;
    bool render_single_frame = true;
    uint32_t num_frames = 128;

    uint32_t max_depth = 5;
    glm::highp_dvec3 background_color = glm::highp_dvec3(0.235294f, 0.67451f, 0.843137f);
    glm::highp_dvec3 cam_origin = glm::highp_dvec3(0.0f);
    double cam_pitch = 0.0f;
    double cam_yaw = -90.0f;
    glm::highp_dvec3 cam_up = glm::highp_dvec3(0.0f, -1.0f, 0.0f);
    double fov = 60.0f;
    double shadow_bias = 1e-4f;
    bool enable_aerial_perspective = true;

    AAAlgorithm antyaliasing_type = AAAlgorithm::STOCHASTIC; //TODO: load parameters for antyaliasing
    bool enable_antialiasing = false;

    /* Adaptive AA params */
    uint32_t aa_max_depth = 2;
    double aa_epsilon     = 0.1f;

    /* Stochastic AA params*/
    int num_samples = 4;

    /* FXAA params */
};
