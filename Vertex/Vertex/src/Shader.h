#pragma once

#include <GL\glew.h>
#include <glm\glm.hpp>

#include <map>
#include <string>
#include <vector>

class Shader
{
public:
    friend class Renderer;
    friend class ShaderManager;
    friend class Mesh;

    virtual ~Shader();

    void setUniform1f       (const std::string & uniformName, float value);
    void setUniform1i       (const std::string & uniformName, int value);
    void setUniform1ui      (const std::string & uniformName, unsigned int value);
    void setUniform2fv      (const std::string & uniformName, glm::vec2 & vector);
    void setUniform3fv      (const std::string & uniformName, glm::vec3 & vector);
    void setUniformMatrix3fv(const std::string & uniformName, glm::mat3 & matrix);
    void setUniformMatrix4fv(const std::string & uniformName, glm::mat4 & matrix);

protected:
    Shader(const std::string & vertexShaderFilename,
           const std::string & fragmentShaderFilename,
           const std::string & geometryShaderFilename               = "",
           const std::string & tessellationControlShaderFilename    = "",
           const std::string & tessellationEvaluationShaderFilename = "",
           const std::string & computeShaderFilename                = "");

private:
    struct UBO;

    std::map<std::string, UBO *> uniformBlocksMembers;
    std::map<std::string, GLint> uniformsLocations;
    std::vector<UBO *>           uniformBlocks;

    GLuint program_id;
    bool isLinked;

    bool link();
    void apply();
    bool getUniformLocation(const std::string & uniform_name);
    bool setupUnifomBuffers();
    void unlockUBOs();
    void lockUBOs();

    /* TODO: Decouple UBO from Shader class */
    struct UBO
    {
        std::map<std::string /* name */, GLint /* offset */> uniformMembersOffsets;
        
        GLuint    ubo_id              = -1;
        GLuint    block_id            = -1;
        GLuint    num_backing_buffers = 3;
        GLint     block_size          = 0;
        GLint     ring_index          = 0;
        GLint     binding_index       = -1;
        GLubyte * data                = nullptr;
        bool      isDirty             = true;

        void lockBuffer()
        {
            if (!glIsSync(syncs[ring_index]))
            {
                syncs[ring_index] = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0 /* flags */);

                /* Traverse to the next buffer */
                ring_index = (ring_index + 1) % num_backing_buffers;
            }
        }

        void unlockBuffer()
        {
            if (glIsSync(syncs[ring_index]))
            {
                GLbitfield wait_flags = 0;
                GLuint64 wait_duration = 0;

                do
                {
                    GLenum result = glClientWaitSync(syncs[ring_index], wait_flags, wait_duration);

                    if(result == GL_ALREADY_SIGNALED || result == GL_CONDITION_SATISFIED)
                        break;

                    wait_flags = GL_SYNC_FLUSH_COMMANDS_BIT;
                    wait_duration = 1000000000;
                }
                while(true);

                glDeleteSync(syncs[ring_index]);
                syncs[ring_index] = 0;
            }
        }

        ~UBO()
        {
            if (ubo_id != 0)
            {
                glUnmapNamedBuffer(ubo_id);
                glDeleteBuffers(1, &ubo_id);
                ubo_id = 0;
            }

            for (auto & sync : syncs)
            {
                if (sync != 0)
                {
                    glDeleteSync(sync);
                    sync = 0;
                }
            }
        }

    private:
        GLsync syncs[3];
    };
};

