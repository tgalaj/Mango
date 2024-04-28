#pragma once
#include <string>

namespace mango
{
    /*
     * Prefixes description
     * m_ rendering/material properties
     * g_ globals per object
     * s_ uniforms to be skipped and set manually
     */
    #define MATERIAL_UNIFORM_PREFIX "m_"
    #define GLOBAL_UNIFORM_PREFIX   "g_"
    #define SKIP_UNIFORM_PREFIX     "s_"

    /* Matrices */
    #define G_MVP               "g_mvp"
    #define G_MODEL_MATRIX      "g_model"
    #define G_MODEL_VIEW_MATRIX "g_model_view"
    #define G_VIEW_MATRIX       "g_view"
    #define G_PROJECTION_MATRIX "g_projection"
    #define G_NORMAL_MATRIX     "g_normal_matrix"

    /* Camera */
    #define G_CAM_POS "g_cam_pos"

    /* Lights */
    #define S_DIRECTIONAL_LIGHT "s_directional_light"
    #define S_POINT_LIGHT       "s_point_light"
    #define S_SPOT_LIGHT        "s_spot_light"

    /* Textures */
    #define M_TEXTURE_DIFFUSE              "m_texture_diffuse"
    #define M_TEXTURE_SPECULAR             "m_texture_specular"
    #define M_TEXTURE_NORMAL               "m_texture_normal"
    #define M_TEXTURE_EMISSION             "m_texture_emission"
    #define M_TEXTURE_DEPTH                "m_texture_depth"
}