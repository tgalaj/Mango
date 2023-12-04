#pragma once

#include <filesystem>
#include <unordered_map>
#include <vector>

#include "glad/glad.h"

#include "Material.h"

namespace mango
{
    class TransformComponent;

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
        explicit Shader(const std::string & computeShaderFilename);

        Shader(const std::string & vertexShaderFilename,
               const std::string & fragmentShaderFilename);

        Shader(const std::string & vertexShaderFilename,
               const std::string & fragmentShaderFilename,
               const std::string & geometryShaderFilename);

        Shader(const std::string & vertexShaderFilename,
               const std::string & fragmentShaderFilename,
               const std::string & tessellationControlShaderFilename,
               const std::string & tessellationEvaluationShaderFilename);

        Shader(const std::string & vertexShaderFilename,
               const std::string & fragmentShaderFilename,
               const std::string & geometryShaderFilename,
               const std::string & tessellationControlShaderFilename,
               const std::string & tessellationEvaluationShaderFilename);

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

        void setSubroutine(Type shaderType, const std::string & subroutineName);

    private:
        void addAllUniforms();
        void addAllSubroutines();

        void addShader(const std::string & filename, GLuint type) const;
        bool getUniformLocation(const std::string & uniformName);

        std::string loadFile(const std::filesystem::path& filepath) const;
        std::string loadShaderIncludes(const std::string& shaderCode) const;

    private:
        std::unordered_map<std::string, GLuint> m_subroutineIndices;
        std::unordered_map<GLenum, GLuint>      m_activeSubroutineUniformLocations;

        std::unordered_map<std::string, GLint> m_uniformsLocations;
        std::vector<std::string>               m_uniformsNames;
        std::vector<std::string>               m_globalUniformsNames;
        std::vector<GLint>                     m_uniformsTypes;
        std::vector<GLint>                     m_globalUniformsTypes;

        GLuint m_programID;
        bool m_isLinked;

        friend class AssetManager;
    };
}
