#pragma once

#ifdef MG_ENABLE_GL_DEBUG_MARKERS
    #include "glad/glad.h"
    
    #define MG_BEGIN_GL_MARKER(name)        glPushDebugGroup(GL_DEBUG_SOURCE_THIRD_PARTY, 0,  strlen(name), name)
    #define MG_BEGIN_GL_MARKER_ID(id, name) glPushDebugGroup(GL_DEBUG_SOURCE_THIRD_PARTY, id, strlen(name), name)
    #define MG_END_GL_MARKER                glPopDebugGroup()

#else
    #define MG_BEGIN_GL_MARKER(name)
    #define MG_BEGIN_GL_MARKER_ID(id, name)
    #define MG_END_GL_MARKER
#endif