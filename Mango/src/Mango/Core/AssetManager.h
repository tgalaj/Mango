#pragma once
#include <unordered_map>

#include "Mango/Rendering/Font.h"
#include "Mango/Rendering/Model.h"
#include "Mango/Rendering/Texture.h"

namespace mango
{
    class AssetManager final
    {
    public:
        static std::shared_ptr<Font>    createFont          (const std::string & fontName, const std::string& filename, GLuint fontHeight);
        static std::shared_ptr<Texture> createTexture2D     (const std::string & filename, bool isSrgb = false, GLint numMipmaps = 1);
        static std::shared_ptr<Texture> createTexture2D1x1  (const std::string & textureName, const glm::uvec4 & color);
        static std::shared_ptr<Texture> createCubeMapTexture(const std::string * filenames, bool isSrgb = false, GLint numMipmaps = 1);

        static Model createModel(const std::string & filename);
        static Model createModel();

        static std::shared_ptr<Shader> createShader(const std::string & shaderName,
                                                    const std::string & computeShaderFilename);

        static std::shared_ptr<Shader> createShader(const std::string & shaderName,
                                                    const std::string & vertexShaderFilename,
                                                    const std::string & fragmentShaderFilename);

        static std::shared_ptr<Shader> createShader(const std::string & shaderName,
                                                    const std::string & vertexShaderFilename,
                                                    const std::string & fragmentShaderFilename,
                                                    const std::string & geometryShaderFilename);

        static std::shared_ptr<Shader> createShader(const std::string & shaderName,
                                                    const std::string & vertexShaderFilename,
                                                    const std::string & fragmentShaderFilename,
                                                    const std::string & tessellationControlShaderFilename,
                                                    const std::string & tessellationEvaluationShaderFilename);

        static std::shared_ptr<Shader> createShader(const std::string & shaderName,
                                                    const std::string & vertexShaderFilename,
                                                    const std::string & fragmentShaderFilename,
                                                    const std::string & geometryShaderFilename,
                                                    const std::string & tessellationControlShaderFilename,
                                                    const std::string & tessellationEvaluationShaderFilename);

        static std::shared_ptr<Font>    getFont     (const std::string & fontName);
        static std::shared_ptr<Texture> getTexture2D(const std::string & textureName);
        static std::shared_ptr<Shader>  getShader   (const std::string & shaderName);

    private:
        AssetManager() {}
        ~AssetManager() {}

        static std::unordered_map<std::string, Model>                    m_loadedModels;
        static std::unordered_map<std::string, std::shared_ptr<Texture>> m_loadedTextures;
        static std::unordered_map<std::string, std::shared_ptr<Shader>>  m_loadedShaders;
        static std::unordered_map<std::string, std::shared_ptr<Font>>    m_loadedFonts;
    };
}
