#pragma once

#define MG_PROFILE 1

#if MG_PROFILE
    #include "tracy/Tracy.hpp"
    #include "common/TracyColor.hpp"
    
    #include "glad/glad.h"
    #include "tracy/TracyOpenGL.hpp"

    #define MG_PROFILE_FRAME_MARK                  FrameMark
    #define MG_PROFILE_FRAME_MARK_NAMED(name)      FrameMarkNamed(name)
    #define MG_PROGILE_FRAME_MARK_START(name)      FrameMarkStart(name)
    #define MG_PROGILE_FRAME_MARK_END(name)        FrameMarkEnd(name)
    
    #define MG_PROFILE_ZONE_SCOPED                 ZoneScoped
    #define MG_PROFILE_ZONE_SCOPED_C(color)        ZoneScopedC(color)
    #define MG_PROFILE_ZONE_SCOPED_N(name)         ZoneScopedN(name)
    #define MG_PROFILE_ZONE_SCOPED_NC(name, color) ZoneScopedNC(name, color)
    #define MG_PROFILE_ZONE_TEXT(text, size)       ZoneText(text, size)

    // For string literals
    #define MG_PROFILE_LOG_L(text)                 TracyMessageL(text)

    #define MG_PROFILE_LOG(text, size)             TracyMessage(text, size)
    #define MG_PROGILE_PLOT_VALUE(name, value)     TracyPlot(name, value)

    // Specify dynamic color of a zone
    #define MG_PROFILE_ZONE_COLOR(color)           ZoneColor(color)

    // Additional information about the profiled application
    #define MG_PROFILE_APP_INFO(text, size)        TracyAppInfo(text, size)

    // Make sure max image resolution is 320x180 !!
    #define MG_PROFILE_FRAME_IMAGE(image, width, height, offset, flip) FrameImage(image, width, height, offset, flip)

    // OpenGL specific
    #define MG_PROFILE_GL_CONTEXT             TracyGpuContext
    #define MG_PROFILE_GL_ZONE(name)          TracyGpuZone(name)
    #define MG_PROFILE_GL_ZONE_C(name, color) TracyGpuZoneC(name, color)
    
    // Call after swap buffers function call
    #define MG_PROFILE_GL_COLLECT           TracyGpuCollect
#else
    #define MG_PROFILE_FRAME_MARK
    #define MG_PROFILE_FRAME_MARK_NAMED(name)
    #define MG_PROGILE_FRAME_MARK_START(name)
    #define MG_PROGILE_FRAME_MARK_END(name)
    
    #define MG_PROFILE_ZONE_SCOPED
    #define MG_PROFILE_ZONE_SCOPED_C(color)
    #define MG_PROFILE_ZONE_SCOPED_N(name)
    #define MG_PROFILE_ZONE_SCOPED_NC(name, color)
    #define MG_PROFILE_ZONE_TEXT(text, size)

    // For string literals
    #define MG_PROFILE_LOGL(text)

    #define MG_PROFILE_LOG(text, size)
    #define MG_PROGILE_PLOT_VALUE(name, value)

    // Specify dynamic color of a zone
    #define MG_PROFILE_ZONE_COLOR(color)

    // Additional information about the profiled application
    #define MG_PROFILE_APP_INFO(text, size)

    // Make sure max image resolution is 320x180 !!
    #define MG_PROFILE_FRAME_IMAGE(image, width, height, offset, flip)

    // OpenGL specific
    #define MG_PROFILE_GL_CONTEXT
    #define MG_PROFILE_GL_ZONE(name)
    #define MG_PROFILE_GL_ZONE(name, color)
    
    // Call after swap buffers function call
    #define MG_PROFILE_GL_COLLECT
#endif