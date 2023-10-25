#include "mgpch.h"

#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "Mango/Core/CoreAssetManager.h"
#include "Mango/Core/CoreServices.h"
#include "Mango/Utilities/ShaderGlobals.h"
#include "Mango/Utilities/Util.h"

namespace mango
{
    Shader::Shader()
        : m_programID(0),
          m_isLinked (false)
    {
        m_programID = glCreateProgram();

        if (m_programID == 0)
        {
            fprintf(stderr, "Error while creating program object.\n");
        }
    }

    Shader::Shader(const std::filesystem::path & computeShaderFilepath)
        : Shader()
    {
        addShader(computeShaderFilepath, GL_COMPUTE_SHADER);
    }

    Shader::Shader(const std::filesystem::path & vertexShaderFilepath,
                   const std::filesystem::path & fragmentShaderFilepath)
        : Shader()
    {
        addShader(vertexShaderFilepath,   GL_VERTEX_SHADER);
        addShader(fragmentShaderFilepath, GL_FRAGMENT_SHADER);
    }

    Shader::Shader(const std::filesystem::path & vertexShaderFilepath,
                   const std::filesystem::path & fragmentShaderFilepath,
                   const std::filesystem::path & geometryShaderFilepath)
        : Shader(vertexShaderFilepath, fragmentShaderFilepath)
    {
        addShader(geometryShaderFilepath, GL_GEOMETRY_SHADER);
    }

    Shader::Shader(const std::filesystem::path & vertexShaderFilepath,
                   const std::filesystem::path & fragmentShaderFilepath,
                   const std::filesystem::path & tessellationControlShaderFilepath,
                   const std::filesystem::path & tessellationEvaluationShaderFilepath)
        : Shader(vertexShaderFilepath, fragmentShaderFilepath)
    {
        addShader(tessellationControlShaderFilepath,    GL_TESS_CONTROL_SHADER);
        addShader(tessellationEvaluationShaderFilepath, GL_TESS_EVALUATION_SHADER);
    }

    Shader::Shader(const std::filesystem::path & vertexShaderFilepath,
                   const std::filesystem::path & fragmentShaderFilepath,
                   const std::filesystem::path & geometryShaderFilepath,
                   const std::filesystem::path & tessellationControlShaderFilepath,
                   const std::filesystem::path & tessellationEvaluationShaderFilepath)
        : Shader(vertexShaderFilepath,
                 fragmentShaderFilepath,
                 tessellationControlShaderFilepath,
                 tessellationEvaluationShaderFilepath)
    {
        addShader(geometryShaderFilepath, GL_GEOMETRY_SHADER);
    }

    Shader::~Shader()
    {
        if (m_programID != 0)
        {
            glDeleteProgram(m_programID);
            m_programID = 0;
        }
    }

    void Shader::addShader(const std::filesystem::path &filepath, GLuint type) const
    {
        if (m_programID == 0)
        {
            return;
        }

        if (filepath.empty())
        {
            fprintf(stderr, "Error: Shader's file name can't be empty.\n");

            return;
        }

        GLuint shaderObject = glCreateShader(type);

        if (shaderObject == 0)
        {
            fprintf(stderr, "Error while creating %s.\n", filepath.string().c_str());

            return;
        }


        std::string code = Util::loadFile("assets/shaders" / filepath);
        code = Util::loadShaderIncludes(code);

        const char * shaderCode = code.c_str();

        glShaderSource(shaderObject, 1, &shaderCode, nullptr);
        glCompileShader(shaderObject);

        GLint result;
        glGetShaderiv(shaderObject, GL_COMPILE_STATUS, &result);

        if (result == GL_FALSE)
        {
            fprintf(stderr, "%s compilation failed!\n", filepath.string().c_str());

            GLint logLen;
            glGetShaderiv(shaderObject, GL_INFO_LOG_LENGTH, &logLen);

            if (logLen > 0)
            {
                char * log = static_cast<char *>(malloc(logLen));

                GLsizei written;
                glGetShaderInfoLog(shaderObject, logLen, &written, log);

                fprintf(stderr, "Shader log: \n%s", log);
                free(log);
            }
            getchar();
            return;
        }

        glAttachShader(m_programID, shaderObject);
        glDeleteShader(shaderObject);
    }

    void Shader::addAllUniforms()
    {
        /* Get all active uniforms except uniforms in blocks */
        GLenum programInterface = GL_UNIFORM;
        GLint numUniforms = 0;
        glGetProgramInterfaceiv(m_programID, programInterface, GL_ACTIVE_RESOURCES, &numUniforms);

        const GLenum properties[] = { GL_BLOCK_INDEX, GL_TYPE, GL_NAME_LENGTH, GL_LOCATION };
        const GLint propertiesSize = sizeof(properties) / sizeof(properties[0]);

        for(GLint i = 0; i < numUniforms; ++i)
        {
            GLint values[propertiesSize];
            glGetProgramResourceiv(m_programID, programInterface, i, propertiesSize, properties, propertiesSize, nullptr, values);

            /* Skip all uniforms in blocks */
            if(values[0] != -1)
            {
                continue;
            }

            std::vector<char> nameData(values[2]);
            glGetProgramResourceName(m_programID, programInterface, i, nameData.size(), nullptr, &nameData[0]);
            std::string uniformName(nameData.begin(), nameData.end() - 1);

            std::string prefix = uniformName.substr(0, 2);
            if(GLOBAL_UNIFORM_PREFIX == prefix)
            {
                m_globalUniformsTypes.push_back(values[1]);
                m_globalUniformsNames.push_back(uniformName);
                m_uniformsLocations[uniformName] = values[3];
            }
            else if(SKIP_UNIFORM_PREFIX == prefix)
            {
                m_uniformsLocations[uniformName] = values[3];
            }
            else
            {
                m_uniformsTypes.push_back(values[1]);
                m_uniformsNames.push_back(uniformName);
                m_uniformsLocations[uniformName] = values[3];
            }
        }
    }

    void Shader::addAllSubroutines()
    {
        GLenum interfaces[]   = { GL_VERTEX_SUBROUTINE, GL_FRAGMENT_SUBROUTINE };
        GLenum shaderStages[] = { GL_VERTEX_SHADER, GL_FRAGMENT_SHADER };

        GLint interfacesCount = sizeof(interfaces) / sizeof(interfaces[0]);

        for(GLint i = 0; i < interfacesCount; ++i)
        {
            /* Get all active subroutines */
            GLenum programInterface = interfaces[i];

            GLint numSubroutines = 0;
            glGetProgramInterfaceiv(m_programID, programInterface, GL_ACTIVE_RESOURCES, &numSubroutines);

            const GLenum properties[]   = { GL_NAME_LENGTH };
            const GLint  propertiesSize = sizeof(properties) / sizeof(properties[0]);

            GLint count_subroutine_locations = 0;
            glGetProgramStageiv(m_programID, shaderStages[i], GL_ACTIVE_SUBROUTINE_UNIFORM_LOCATIONS, &count_subroutine_locations);
            m_activeSubroutineUniformLocations[shaderStages[i]] = count_subroutine_locations;

            for (GLint j = 0; j < numSubroutines; ++j)
            {
                GLint values[propertiesSize];
                GLint length = 0;
                glGetProgramResourceiv(m_programID, programInterface, j, propertiesSize, properties, propertiesSize, &length, values);

                std::vector<char> nameData(values[0]);
                glGetProgramResourceName(m_programID, programInterface, j, nameData.size(), nullptr, &nameData[0]);
                std::string subroutineName(nameData.begin(), nameData.end() - 1);

                GLuint subroutineIndex = glGetSubroutineIndex(m_programID, shaderStages[i], subroutineName.c_str());

                m_subroutineIndices[subroutineName] = subroutineIndex;
            }
        }
    }

    bool Shader::link()
    {
        glLinkProgram(m_programID);

        GLint status;
        glGetProgramiv(m_programID, GL_LINK_STATUS, &status);

        if (status == GL_FALSE)
        {
            fprintf(stderr, "Failed to link shader program!\n");

            GLint logLen;
            glGetProgramiv(m_programID, GL_INFO_LOG_LENGTH, &logLen);

            if (logLen > 0)
            {
                char* log = (char*)malloc(logLen);
                GLsizei written;
                glGetProgramInfoLog(m_programID, logLen, &written, log);

                fprintf(stderr, "Program log: \n%s", log);
                free(log);
            }
        }
        else
        {
            m_isLinked = true;

            addAllUniforms();
            addAllSubroutines();
        }

        return m_isLinked;
    }

    void Shader::bind() const
    {
        if (m_programID != 0 && m_isLinked)
        {
            glUseProgram(m_programID);
        }
    }

    void Shader::updateUniforms(Material & material)
    {
        for(unsigned i = 0; i < m_uniformsNames.size(); ++i)
        {
            auto uniformName = m_uniformsNames[i];
            auto uniformType = m_uniformsTypes[i];

            switch(uniformType)
            {
                case GL_SAMPLER_2D:
                    if(M_TEXTURE_DIFFUSE == uniformName)
                    {
                        material.getTexture(Material::TextureType::DIFFUSE)->bind(GLuint(Material::TextureType::DIFFUSE));
                    }
                    else
                    if(M_TEXTURE_SPECULAR == uniformName)
                    {
                        material.getTexture(Material::TextureType::SPECULAR)->bind(GLuint(Material::TextureType::SPECULAR));
                    }
                    else
                    if(M_TEXTURE_NORMAL == uniformName)
                    {
                        material.getTexture(Material::TextureType::NORMAL)->bind(GLuint(Material::TextureType::NORMAL));
                    }
                    else
                    if (M_TEXTURE_EMISSION == uniformName)
                    {
                        material.getTexture(Material::TextureType::EMISSION)->bind(GLuint(Material::TextureType::EMISSION));
                    }
                    else
                    if (M_TEXTURE_DEPTH == uniformName)
                    {
                        material.getTexture(Material::TextureType::DEPTH)->bind(GLuint(Material::TextureType::DEPTH));
                    }
                    break;
                case GL_FLOAT_VEC3:
                    setUniform(uniformName, material.getVector3(uniformName));
                    break;
                case GL_FLOAT:
                    setUniform(uniformName, material.getFloat(uniformName));
                    break;
            }
        }
    }

    void Shader::updateGlobalUniforms(const TransformComponent & transform)
    {
        for(unsigned i = 0; i < m_globalUniformsNames.size(); ++i)
        {
            auto uniformName = m_globalUniformsNames[i];
            auto uniformType = m_globalUniformsTypes[i];

            switch(uniformType)
            {
                case GL_FLOAT_MAT4:
                {
                    if (G_MVP == uniformName)
                    {
                        auto view       = CoreServices::getRenderer()->getCamera()->view;
                        auto projection = CoreServices::getRenderer()->getCamera()->projection;

                        auto mvp = projection * view * transform.getWorldMatrix();
                        setUniform(G_MVP, mvp);
                    }
                    else
                    if (G_MODEL_MATRIX == uniformName)
                    {
                        setUniform(G_MODEL_MATRIX, transform.getWorldMatrix());
                    }
                    break;
                }
                case GL_FLOAT_MAT3:
                {
                    if (G_NORMAL_MATRIX == uniformName)
                    {
                        setUniform(G_NORMAL_MATRIX, transform.getNormalMatrix());
                    }
                    break;
                }
                case GL_FLOAT_VEC3:
                {
                    if (G_CAM_POS == uniformName)
                    {
                        setUniform(G_CAM_POS, CoreServices::getRenderer()->getCameraTransform()->getPosition());
                    }
                }
            }
        }
    }

    bool Shader::getUniformLocation(const std::string & uniformName)
    {
        GLint uniformLocation = glGetUniformLocation(m_programID, uniformName.c_str());

        if (uniformLocation != -1)
        {
            m_uniformsLocations[uniformName] = uniformLocation;
            return true;
        }
        else
        {
            fprintf(stderr, "Error! Can't find uniform %s\n", uniformName.c_str());
            return false;
        }
    }

    void Shader::setUniform(const std::string & uniformName, float value)
    {
        if (m_uniformsLocations.count(uniformName))
        {
            glProgramUniform1f(m_programID, m_uniformsLocations[uniformName], value);
        }
        else
        {
            if (getUniformLocation(uniformName))
            {
                glProgramUniform1f(m_programID, m_uniformsLocations[uniformName], value);
            }
        }
    }

    void Shader::setUniform(const std::string & uniformName, int value)
    {
        if (m_uniformsLocations.count(uniformName))
        {
            glProgramUniform1i(m_programID, m_uniformsLocations[uniformName], value);
        }
        else
        {
            if (getUniformLocation(uniformName))
            {
                glProgramUniform1i(m_programID, m_uniformsLocations[uniformName], value);
            }
        }
    }

    void Shader::setUniform(const std::string & uniformName, unsigned int value)
    {
        if (m_uniformsLocations.count(uniformName))
        {
            glProgramUniform1ui(m_programID, m_uniformsLocations.at(uniformName), value);
        }
        else
        {
            if (getUniformLocation(uniformName))
            {
                glProgramUniform1ui(m_programID, m_uniformsLocations[uniformName], value);
            }
        }
    }

    void Shader::setUniform(const std::string & uniformName, GLsizei count, float * value)
    {
        if (m_uniformsLocations.count(uniformName))
        {
            glProgramUniform1fv(m_programID, m_uniformsLocations[uniformName], count, value);
        }
        else
        {
            if (getUniformLocation(uniformName))
            {
                glProgramUniform1fv(m_programID, m_uniformsLocations[uniformName], count, value);
            }
        }
    }

    void Shader::setUniform(const std::string & uniformName, GLsizei count, int * value)
    {
        if (m_uniformsLocations.count(uniformName))
        {
            glProgramUniform1iv(m_programID, m_uniformsLocations[uniformName], count, value);
        }
        else
        {
            if (getUniformLocation(uniformName))
            {
                glProgramUniform1iv(m_programID, m_uniformsLocations[uniformName], count, value);
            }
        }
    }

    void Shader::setUniform(const std::string & uniformName, GLsizei count, glm::vec3 * vectors)
    {
        if (m_uniformsLocations.count(uniformName))
        {
            glProgramUniform3fv(m_programID, m_uniformsLocations[uniformName], count, glm::value_ptr(vectors[0]));
        }
        else
        {
            if (getUniformLocation(uniformName))
            {
                glProgramUniform3fv(m_programID, m_uniformsLocations[uniformName], count, glm::value_ptr(vectors[0]));
            }
        }
    }

    void Shader::setUniform(const std::string & uniformName, const glm::vec2 & vector)
    {
        if (m_uniformsLocations.count(uniformName))
        {
            glProgramUniform2fv(m_programID, m_uniformsLocations[uniformName], 1, glm::value_ptr(vector));
        }
        else
        {
            if (getUniformLocation(uniformName))
            {
                glProgramUniform2fv(m_programID, m_uniformsLocations[uniformName], 1, glm::value_ptr(vector));
            }
        }
    }

    void Shader::setUniform(const std::string & uniformName, const glm::vec3 & vector)
    {
        if (m_uniformsLocations.count(uniformName))
        {
            glProgramUniform3fv(m_programID, m_uniformsLocations[uniformName], 1, glm::value_ptr(vector));
        }
        else
        {
            if (getUniformLocation(uniformName))
            {
                glProgramUniform3fv(m_programID, m_uniformsLocations[uniformName], 1, glm::value_ptr(vector));
            }
        }
    }

    void Shader::setUniform(const std::string & uniformName, const glm::vec4 & vector)
    {
        if (m_uniformsLocations.count(uniformName))
        {
            glProgramUniform4fv(m_programID, m_uniformsLocations[uniformName], 1, glm::value_ptr(vector));
        }
        else
        {
            if (getUniformLocation(uniformName))
            {
                glProgramUniform4fv(m_programID, m_uniformsLocations[uniformName], 1, glm::value_ptr(vector));
            }
        }
    }

    void Shader::setUniform(const std::string & uniformName, const glm::mat3 & matrix)
    {
        if (m_uniformsLocations.count(uniformName))
        {
            glProgramUniformMatrix3fv(m_programID, m_uniformsLocations[uniformName], 1, GL_FALSE, glm::value_ptr(matrix));
        }
        else
        {
            if (getUniformLocation(uniformName))
            {
                glProgramUniformMatrix3fv(m_programID, m_uniformsLocations[uniformName], 1, GL_FALSE, glm::value_ptr(matrix));
            }
        }
    }

    void Shader::setUniform(const std::string & uniformName, const glm::mat4 & matrix)
    {
        if (m_uniformsLocations.count(uniformName))
        {
            glProgramUniformMatrix4fv(m_programID, m_uniformsLocations[uniformName], 1, GL_FALSE, glm::value_ptr(matrix));
        }
        else
        {
            if (getUniformLocation(uniformName))
            {
                glProgramUniformMatrix4fv(m_programID, m_uniformsLocations[uniformName], 1, GL_FALSE, glm::value_ptr(matrix));
            }
        }
    }

    void Shader::setUniform(const std::string & uniformName, glm::mat4 * matrices, unsigned count)
    {
        if (m_uniformsLocations.count(uniformName))
        {
            glProgramUniformMatrix4fv(m_programID, m_uniformsLocations[uniformName], count, GL_FALSE, &matrices[0][0][0]);
        }
        else
        {
            if (getUniformLocation(uniformName))
            {
                glProgramUniformMatrix4fv(m_programID, m_uniformsLocations[uniformName], count, GL_FALSE, &matrices[0][0][0]);
            }
        }
    }

    void Shader::setSubroutine(Type shaderType, const std::string & subroutineName)
    {
        glUniformSubroutinesuiv(GLenum(shaderType), m_activeSubroutineUniformLocations[GLenum(shaderType)], &m_subroutineIndices[subroutineName]);
    }
}
