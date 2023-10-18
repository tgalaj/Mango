#pragma once

#include <glad/glad.h>

#include <map>
#include <vector>
#include <entityx/Entity.h>
#include "CoreComponents/TransformComponent.h"
#include "Material.h"

namespace mango
{
    class Shader final
    {
    public:
        enum class Type 
        { 
            VERTEX                 = GL_VERTEX_SHADER, 
            FRAGMENT               = GL_FRAGMENT_SHADER,
            GEOMETRY               = GL_GEOMETRY_SHADER, 
            TESSELATION_CONTROL    = GL_TESS_CONTROL_SHADER,
            TESSELATION_EVALUATION = GL_TESS_EVALUATION_SHADER, 
            COMPUTE                = GL_COMPUTE_SHADER
        };

        Shader();
        explicit Shader(const std::string & compute_shader_filename);

        Shader(const std::string & vertex_shader_filename,
               const std::string & fragment_shader_filename);

        Shader(const std::string & vertex_shader_filename,
               const std::string & fragment_shader_filename,
               const std::string & geometry_shader_filename);

        Shader(const std::string & vertex_shader_filename,
               const std::string & fragment_shader_filename,
               const std::string & tessellation_control_shader_filename,
               const std::string & tessellation_evaluation_shader_filename);

        Shader(const std::string & vertex_shader_filename,
               const std::string & fragment_shader_filename,
               const std::string & geometry_shader_filename,
               const std::string & tessellation_control_shader_filename,
               const std::string & tessellation_evaluation_shader_filename);

        ~Shader();

        bool link();
        void bind() const;
        void updateUniforms(Material & material);
        void updateGlobalUniforms(const TransformComponent & transform);

        void setUniform(const std::string & uniformName, float value);
        void setUniform(const std::string & uniformName, int value);
        void setUniform(const std::string & uniformName, unsigned int value);
        void setUniform(const std::string & uniformName, GLsizei count, float * value);
        void setUniform(const std::string & uniformName, GLsizei count, int * value);
        void setUniform(const std::string & uniformName, GLsizei count, glm::vec3 * vectors);
        void setUniform(const std::string & uniformName, const glm::vec2 & vector);
        void setUniform(const std::string & uniformName, const glm::vec3 & vector);
        void setUniform(const std::string & uniformName, const glm::vec4 & vector);
        void setUniform(const std::string & uniformName, const glm::mat3 & matrix);
        void setUniform(const std::string & uniformName, const glm::mat4 & matrix);
        void setUniform(const std::string & uniformName, glm::mat4 * matrices, unsigned count);

        void setSubroutine(Type shader_type, const std::string & subroutine_name);

    private:
        void addAllUniforms();
        void addAllSubroutines();

        void addShader(std::string const & file_name, GLuint type) const;
        bool getUniformLocation(const std::string & uniform_name);

        std::map<std::string, GLuint> m_subroutine_indices;
        std::map<GLenum, GLuint> m_active_subroutine_uniform_locations;

        std::map<std::string, GLint> m_uniforms_locations;
        std::vector<std::string>     m_uniforms_names;
        std::vector<std::string>     m_global_uniforms_names;
        std::vector<GLint>           m_uniforms_types;
        std::vector<GLint>           m_global_uniforms_types;

        GLuint m_program_id;
        bool m_is_linked;

        friend class CoreAssetManager;
    };
}
